#include <FastLED.h>
#include <avr/power.h>


#define STRAND 4
#define NUM_LEDS 6
#define IR_E 10
#define IR_R A0

#define MAX_BRIGHTNESS 50

CRGB leds [NUM_LEDS];

int serial_init = 0;

void setup()
{

	pinMode(IR_R, INPUT_PULLUP);
	pinMode(IR_E, OUTPUT);
	digitalWrite(IR_E, HIGH);

	//Setup NeoPixel strip, turn everything off.
    FastLED.addLEDs<NEOPIXEL, STRAND>(leds, NUM_LEDS);
	for(int i=0;i<NUM_LEDS;i++)
    {
        leds[i]=CRGB::Black;
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
    }
}

void set_led(int led, char color)
{
    leds[i].r =  b & 0xC0;
    leds[i].g = (b & 0x30) << 2;
    leds[i].b = (b & 0x0C) << 4;
    leds[i].fadeLightBy((4-(b&0x3))<<6);
    
}

void display_hacker(int offset)
{

}

void display_oper(int offset)
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

}

void display_w88(int offset)
{
    char b = 0xF8;
    do { 
        for(int i=0;i<NUM_LEDS;i++)
        {
            b = EEPROM.read(offset);
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

void display_blue(int offset)
{

}

void display_red(int offset)
{

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
        display_w88(COLOR_BLOCK_OFF + offset);
    }

    // Check whether you're on the blue team or red team.

    if(badge_type & FLAG_MODE_BLUE)
    {
    
    } else if (badge_type & FLAG_MODE_RED)
    {
       offset = EEPROM.read(
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

