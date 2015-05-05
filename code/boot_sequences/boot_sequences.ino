#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define STRAND 4
#define IR_E 10
#define IR_R A0

Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, STRAND, NEO_GRB + NEO_KHZ800);

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
//#if defined (__AVR_ATtiny85__)
//  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
//#endif
  // End of trinket special code
  
  pinMode(IR_R, INPUT_PULLUP);
  pinMode(STRAND,OUTPUT);
  pinMode(IR_E, OUTPUT);
  digitalWrite(IR_E, HIGH);
  strip.begin();
  strip.setBrightness(15);
  strip.show(); // Initialize all pixels to 'off'
  
  fibWipe(strip.Color(16,16,16), 1000);
  delay(2000);
  for(uint16_t i=0; i<strip.numPixels(); i++)
      strip.setPixelColor(i,(0,0,0));
  strip.show();
  delay(1000);
   for(uint16_t i=0; i<strip.numPixels(); i++)
      strip.setPixelColor(i,(0,16,0));
  strip.show();
  
  Serial.begin(9600);
  delay(3000);
  Serial.println("THOTCON 0x6 Pixel Interface");

}

void fibWipe(uint32_t c, uint16_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
      wait = wait / fib(strip.numPixels());
  }
}

int fib(int n) {

    return n ? 1 : fib(n-1)+fib(n-2);
  }


void loop(){
     

}
