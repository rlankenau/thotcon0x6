#include <Adafruit_NeoPixel.h>
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

