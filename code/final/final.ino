#include <EEPROM.h>
#include "FastLED.h"
#include <avr/power.h>

#include "data.h"

#define STRAND 4
#define NUM_LEDS 6
#define IR_E 10
#define IR_R A0

#define TERM (char)0xF8

#define MAX_BRIGHTNESS 50

CRGB leds [NUM_LEDS];

int serial_init = 0;

void setup()
{
  int i=0;
  pinMode(IR_R, INPUT_PULLUP);
  pinMode(IR_E, OUTPUT);
  digitalWrite(IR_E, HIGH);
  
  //Serial.begin(2400);
  
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
        //Serial.println("Loop start");
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
    leds[led] >>= (4-(color&0x3));
    
}

char voice_order[] = {0x05,0x06,0x04,0x01,0x03,0x02};

void display_voice(int offset)
{
    char color = 0xF8;
    char curr_offset = offset;
    do {
        for(int i =0;i<NUM_LEDS;i++, curr_offset++)
        {
            color = EEPROM.read(curr_offset);
            if(color==0xF8)
                break;
            set_led(voice_order[i], color);
        }
        FastLED.show();
        delay(200);
    } while(color!=0xF8);
    FastLED.clear();
    
}

void display_vip(int offset)
{
    int curr_offset = offset;
    for(int i=0;i<NUM_LEDS;i++)
    {
        char color = EEPROM.read(curr_offset);
        /* Counterclockwise fill */
        int prev_j = -1;
        for(int j=NUM_LEDS-1;j>=i;j--)
        {
            if(prev_j!=-1)
                set_led((prev_j+5)%6, 0x00);
            set_led((j+5)%6, color);
            FastLED.show();
            prev_j = j;
            delay(100);
        }

        delay(200);
        curr_offset++;
    }

}

void display_root(int offset)
{
    char b = 0xF8;
    int curr_offset = offset;
    do {
        int pixel = random(0,NUM_LEDS); 
        b = EEPROM.read(curr_offset);
        if(b==TERM)
            break;
        set_led(pixel, b);
        FastLED.show();
        delay(50);
        pixel = random(0, NUM_LEDS);
        set_led(pixel, 0x00);
        curr_offset++;
    } while(curr_offset < 0xD4);
}

void display_oper(int offset)
{
    char b = 0xF8;
    CRGB tmp[NUM_LEDS];

    for(int i=0;i<NUM_LEDS;i++)
    {
        set_led(i, EEPROM.read(offset + i));    
        tmp[i] = leds[i];
    }

    FastLED.show();
    delay(200);
    memset(leds, 0, sizeof(leds));
    FastLED.show();
    delay(200);

    memcpy(leds, tmp, sizeof(leds));
    FastLED.show();
    delay(200);
    memset(leds, 0, sizeof(leds));
    FastLED.show();
    delay(200);

    memcpy(leds, tmp, sizeof(leds));
    FastLED.show();
    delay(200);
    memset(leds, 0, sizeof(leds));
    FastLED.show();
    delay(500);

    memcpy(leds, tmp, sizeof(leds));
    FastLED.show();
    delay(500);
    memset(leds, 0, sizeof(leds));
    FastLED.show();
    delay(200);

    memcpy(leds, tmp, sizeof(leds));
    FastLED.show();
    delay(500);
    memset(leds, 0, sizeof(leds));
    FastLED.show();
    delay(200);

    memcpy(leds, tmp, sizeof(leds));
    FastLED.show();
    delay(500);
    memset(leds, 0, sizeof(leds));
    FastLED.show();
    delay(500);

    memcpy(leds, tmp, sizeof(leds));
    FastLED.show();
    delay(200);
    memset(leds, 0, sizeof(leds));
    FastLED.show();
    delay(200);

    memcpy(leds, tmp, sizeof(leds));
    FastLED.show();
    delay(200);
    memset(leds, 0, sizeof(leds));
    FastLED.show();
    delay(200);

    memcpy(leds, tmp, sizeof(leds));
    FastLED.show();
    delay(200);
    memset(leds, 0, sizeof(leds));
    FastLED.show();
    delay(200);

    delay(1000);
}

void display_w88(int offset)
{
    char b = 0xF8;
    int curr_offset = offset;
    do { 
        for(int i=0;i<NUM_LEDS;i++)
        {
            b = EEPROM.read(curr_offset);
            if(b==0xF8)
            {
                break;
            }
            set_led(i,b);
            curr_offset++;
        }
        FastLED.show();
        delay(1000);
    } while(b!=0xF8 && curr_offset < 0xD4);
        
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

void do_display()
{
    int badge_type = EEPROM.read(2);
    if(Serial)
    {
      //Serial.print("Badge type is ");
      //Serial.println(badge_type, HEX);
    }
    int offset = 0;

    if(badge_type & FLAG_BTYPE_HACKER)
    {
        offset = EEPROM.read(9);
        display_w88(COLOR_BLOCK_OFF + offset);

    } else if (badge_type & FLAG_BTYPE_OPER)
    {
        offset = EEPROM.read(6);
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
        display_w88(COLOR_BLOCK_OFF + offset);
    }

    // Check whether you're on the blue team or red team.

    if(badge_type & FLAG_MODE_BLUE)
    {
      offset = EEPROM.read(10);
      display_team(COLOR_BLOCK_OFF + offset);
    } else if (badge_type & FLAG_MODE_RED)
    {
      offset = EEPROM.read(11);
      display_team(COLOR_BLOCK_OFF + offset);
    }
    FastLED.clear();
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

