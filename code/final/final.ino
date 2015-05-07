#include <FastLED.h>
#include <avr/power.h>


#define STRAND 4
#define IR_E 10
#define IR_R A0

#define MAX_BRIGHTNESS 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, STRAND, NEO_GRB + NEO_KHZ800);
int serial_init = 0;

void setup()
{

	pinMode(IR_R, INPUT_PULLUP);
	pinMode(IR_E, OUTPUT);
	digitalWrite(IR_E, HIGH);

	//Setup NeoPixel strip, turn everything off.
	strip.begin();
	strip.setBrightness(20);
	strip.show();

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

    do_display();

}

void display_hacker(int offset)
{
    0x0F0F0F0FC3C3

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
    char b = EEPROM.read(offset);
    while(b!=0xF8)
    {
        strip.
    }
        
}



void do_display()
{
    int badge_type = EEPROM.read(2);
    int offset = 0;

    if(badge_type &= FLAG_BTYPE_HACKER)
    {
        offset = EEPROM.read(9);
        display_hacker(offset);

    } else if (badge_type &= FLAG_BTYPE_OPER)
    {
        offset = EEPROM.read(7);
        display_oper(offset);

    } else if (badge_type &= FLAG_BTYPE_VOICE)
    {
        offset = EEPROM.read(8);
        display_voice(offset);

    } else if (badge_type &= FLAG_BTYPE_VIP)
    {
        offset = EEPROM.read(7);
        display_vip(offset);
    } else if (badge_type &= FLAG_BTYPE_ROOT)
    {
        offset = EEPROM.read(5);
        display_root(offset);
    } else if (badge_type &= FLAG_BTYPE_W88)
    {
        offset = EEPROM.read(4);
        display_w88(offset);
    }

    // Check whether you're on the blue team or red team.


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

