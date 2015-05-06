#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define STRAND 4
#define IR_E 10
#define IR_R A0


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, STRAND, NEO_GRB + NEO_KHZ800);
int serial_init = 0;

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code
  
  pinMode(IR_R, INPUT_PULLUP);
  
  pinMode(IR_E, OUTPUT);
  digitalWrite(IR_E, HIGH);
  strip.begin();
  strip.setBrightness(20);
  strip.show(); // Initialize all pixels to 'off'
  
  colorWipe(strip.Color(255,0,0), 20);
  Serial.begin(2400);


}

void loop() {
  

  
  if(Serial) {
      if(!serial_init)
      {
        serial_init = 1;
        Serial.println("Welcome to Thotcon BBS v6.0\n");

      }
      int minimum=1024;
      int maximum=0;
      for(int i=0;i<10000;i++)
      {

        int reading = analogRead(IR_R);
        maximum = max(reading, maximum);
        minimum = min(reading, minimum);
        int numPixels = reading/(1024/6);
        for(uint16_t j=0; j<numPixels; j++) {
          strip.setPixelColor(j, strip.Color(0,255,0));
          strip.show();
          Serial.print("[");
          Serial.print(minimum);
          Serial.print(',');
          Serial.print(maximum);
          Serial.print("] ");
          Serial.println(reading);
        }
      }
      colorWipe(strip.Color(0,255,0), 20);
      delay(100);
      clearPixels(100);
      colorWipe(strip.Color(0,255,0), 20);
      delay(100);
      clearPixels(100);
      colorWipe(strip.Color(0,255,0), 20);
      delay(100);
      clearPixels(100);
    char c = Serial.read();
    while(c) {
    switch(c) {
      case 'R':
        colorWipe(strip.Color(255,0,0), 50);
        break;
      case 'G':
        colorWipe(strip.Color(0,255,0), 50);
        break;
      case 'B':
        colorWipe(strip.Color(0,0,255), 50);
        break;
      default:
        clearPixels(0);
        goto loop_begin;
    }
      delay(1000);
      clearPixels(0);
      delay(100);
      c = Serial.read();
    }
  } else {
      colorWipe(strip.Color(255,0,0), 20);
      delay(100);
            clearPixels(100);

      colorWipe(strip.Color(255,0,0), 20);
      delay(100);
            clearPixels(100);

      colorWipe(strip.Color(255,0,0), 20);
      delay(100);
            clearPixels(100);

  }

loop_begin:
  Serial.println("Loop start");
  digitalWrite(IR_E, LOW);
  delay(1000);
  digitalWrite(IR_E,HIGH);
  delay(1000);
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127,   0,   0), 50); // Red
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue

  rainbow(1);
  rainbowCycle(1);
  theaterChaseRainbow(1);
  clearPixels(0);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void clearPixels(uint8_t wait) {
  for(int i=0;i<strip.numPixels();i++)
    strip.setPixelColor(i,0);
  strip.show();
  delay(wait); 
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

