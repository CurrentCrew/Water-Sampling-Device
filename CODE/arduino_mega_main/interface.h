#ifndef INTERFACE_H
#define INTERFACE_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 main_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct Vector2 {
  int x;
  int y;
};

class UIElement {
  public:
    struct Vector2 size;
    struct Vector2 pos;
    bool selected;
    bool selectable;
    int index;
    virtual void render(Adafruit_SSD1306 display)
    {}

    virtual void onPress()
    {}

    UIElement(int index, struct Vector2 p, struct Vector2 s, bool sel) 
    {
      this->index = index;
      this->size = s;
      this->pos = p;
      this->selectable = sel;
      this->selected = false;
    }
    
    struct Vector2 getCenter() 
    {
      return {
        this->pos.x + this->size.x / 2,
        this->pos.y + this->size.y / 2
      };
    }

    drawOutline(Adafruit_SSD1306 display, int thickness)
    {
      for (int i = 0; i < thickness; i++)
        display.drawRect(this->pos.x+i, this->pos.y+i, this->size.x-i*2, this->size.y-i*2, SSD1306_WHITE);
    }
};

class UIText : public UIElement {
  public:
    char * text;
    int char_count;
    int text_scale;

    void setText(char * text, int char_count, int text_scale)
    {
      this->text = text;
      this->char_count = char_count;
      this->text_scale = text_scale;
    }

    UIText(int index, struct Vector2 s, struct Vector2 p) : UIElement(index, s, p, false)
    {

    }

    void render(Adafruit_SSD1306 display) override
    {
      UIElement::render(display);

      Vector2 center = this->getCenter();
      display.setTextSize(this->text_scale);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(
        center.x - this->char_count  / 2 * CHAR_WIDTH * text_scale,
        center.y - CHAR_HEIGHT * text_scale / 2);             // Start at top-left corner
      display.println(this->text);
    }
};

void emptyFunction() {}

class UIButton : public UIText {
  public:
    void (*onPressFunction)();
    bool on_press_sent;

    UIButton(int index, struct Vector2 s, struct Vector2 p) : UIText(index, s, p)
    {
      this->selectable = true;
      this->onPressFunction = &emptyFunction;
    }

    void setOnPress(void (*onPressFunction)())
    {
      this->onPressFunction = onPressFunction;
      this->on_press_sent = true;
    }

    void onPress() override
    {
      if (this->on_press_sent)
        this->onPressFunction();
    }

    void render(Adafruit_SSD1306 display) override
    {

      UIText::render(display);

      if (this->selected)
        drawOutline(display, 3);
      else
        drawOutline(display, 1);
    }
};

#define MAX_ELEMENT_COUNT 5
class UIScreen {
  public:
    int element_count;
    UIElement *elements[MAX_ELEMENT_COUNT];
    int selected_element = -1;
    void render(Adafruit_SSD1306 display)
    {
      display.clearDisplay();
      for (int i = 0; i < this->element_count; i++)
      {
        this->elements[i]->render(display);
      }

      // Base Elements

      // State
      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(3,3);             // Start at top-left corner
      display.println(state);

      display.display();
    }

    UIScreen() {
      
    }

    void addElement(UIElement *element)
    {
      if (selected_element == -1 && element->selectable)
      {
        element->selected = true;
        this->selected_element = this->element_count;
      }

      this->elements[this->element_count] = element;
      this->element_count++;


    }

    void move(int dir)
    {
      if (this->element_count == 0)
        return;
      int selected_element_index = this->elements[this->selected_element]->index;

      int element_index = -1;
      int closest_index = 1000;
      if (dir == -1)
        closest_index = -1000;

      for (int i = 0; i < this->element_count; i++)
      {
        if (!this->elements[i]->selectable)
          continue;

        if (i == this->selected_element)
          continue;

        int el_index = this->elements[i]->index;
        int diff = (el_index-selected_element_index)*dir;
        int best_diff = (closest_index-selected_element_index)*dir;

        // If not in the right direction stop
        if (diff < 0 || diff > best_diff)
          continue;

        closest_index = el_index;
        element_index = i;
      }

      // Serial.print("Selected: ");
      // Serial.println(element_index);

      if (element_index == -1)
        return;

      this->elements[this->selected_element]->selected = false;
      this->selected_element = element_index;
      this->elements[this->selected_element]->selected = true;
    }

    void select()
    {
      if (this->selected_element == -1)
        return;

      this->elements[this->selected_element]->onPress();
    }
    
};

UIScreen *current_screen;

void switchScreen(UIScreen *new_screen)
{
  current_screen = new_screen;
  new_screen->render(main_display);
}

void initializeInterface()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!main_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println("SSD1306 allocation success");
}

#endif