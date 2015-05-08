#include <EEPROM.h>
#include "FastLED.h"
#include <avr/power.h>

#include "data.h"

#define STRAND 4
#define NUM_LEDS 6
#define IR_E 10
#define IR_R A0

#define MAX_BRIGHTNESS 50

CRGB leds [NUM_LEDS];

int serial_init = 0;

void setup()
{
  int i=0;
  pinMode(IR_R, INPUT_PULLUP);
  pinMode(IR_E, OUTPUT);
  digitalWrite(IR_E, HIGH);
  
  //Setup NeoPixel strip, turn everything off.
  FastLED.addLeds<NEOPIXEL, STRAND>(leds, NUM_LEDS);
  for(i=0;i<NUM_LEDS;i++)
  {
      leds[i] = CRGB::Black;
  }
  FastLED.show();

}

void loop()
{

    int got_ir_input = 0;
    
    if(Serial)
    {
        // Do BBS stuff
    }

    //Read IR

    if(got_ir_input) {
        // Update EEPROM with IR data
    }

    for(int i=0;i<3;i++) 
    {
        do_display();
        delay(100);
    }
}

void set_led(int led, char color)
{
    leds[led].r =  color & 0xC0;
    leds[led].g = (color & 0x30) << 2;
    leds[led].b = (color & 0x0C) << 4;
    leds[led].fadeLightBy((4-(color&0x3))<<6);
    
}

void display_hacker(int offset)
{

}

void display_voice(int offset)
{

}

void display_vip(int offset)
{

}

void display_root(int offset)
{
    char b = 0xF8;
    int curr_offset = offset;
    do {
        int pixel = random(0,NUM_LEDS); 
        b = EEPROM.read(curr_offset++);
        set_led(pixel, b);
        FastLED.show();
        delay(10);
    } while(b!=0xF8);
}

void display_oper(int offset)
{
    char b = 0xF8;
    int curr_offset = offset;
    do { 
        for(int i=0;i<NUM_LEDS;i++)
        {
            b = EEPROM.read(curr_offset++);
            if(b==0xF8)
            {
                break;
            }
            set_led(i,b);
        }
        FastLED.show();
        delay(40);
    } while(b!=0xF8);
        
}

void display_team(int offset)
{
    char b = 0xF8;
    int curr_offset = offset;
    do {
        b = EEPROM.read(curr_offset++);
        for(int i=0;i<NUM_LEDS;i++) {
            set_led(i, b);
            FastLED.show();
            delay(10);
        }
    } while(b!=0xF8);
    
}

void clear_display()
{
   for(int i=0;i<NUM_LEDS;i++)
   {
        set_led(i, 0x00);
        FastLED.show();
   } 
}

void do_display()
{
    int badge_type = EEPROM.read(2);
    int offset = 0;

    if(badge_type & FLAG_BTYPE_HACKER)
    {
        offset = EEPROM.read(9);
        display_hacker(COLOR_BLOCK_OFF + offset);

    } else if (badge_type & FLAG_BTYPE_OPER)
    {
        offset = EEPROM.read(7);
        display_oper(COLOR_BLOCK_OFF + offset);

    } else if (badge_type & FLAG_BTYPE_VOICE)
    {
        offset = EEPROM.read(8);
        display_voice(COLOR_BLOCK_OFF + offset);

    } else if (badge_type & FLAG_BTYPE_VIP)
    {
        offset = EEPROM.read(7);
        display_vip(COLOR_BLOCK_OFF + offset);
    } else if (badge_type & FLAG_BTYPE_ROOT)
    {
        offset = EEPROM.read(5);
        display_root(COLOR_BLOCK_OFF + offset);
    } else if (badge_type & FLAG_BTYPE_W88)
    {
        offset = EEPROM.read(4);
        display_oper(COLOR_BLOCK_OFF + offset);
    }

    // Check whether you're on the blue team or red team.

    if(badge_type & FLAG_MODE_BLUE)
    {
      offset = EEPROM.read(10);
      display_team(offset);
    } else if (badge_type & FLAG_MODE_RED)
    {
      offset = EEPROM.read(11);
      display_team(offset);
    }
    clear_display();
}


// Return a pointer to a null terminated string copied from EEPROM
// The return value points to a global buffer overwritten on every call
const int BUFLEN=24;
char buf[BUFLEN+1];
char*
eprom2string(int addr, int len)
{
   char* fill = buf;
   int i;
   for(i = addr; i<addr+len && i<addr+BUFLEN; i++, fill++) {
       *fill = EEPROM.read(i);
   }
   buf[i] = 0;
   return buf;
}

