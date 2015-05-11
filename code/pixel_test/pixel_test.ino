#include<FastLED.h>

#define LED_PIN     4
#define BRIGHTNESS  4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 6

CRGB leds[NUM_LEDS];

void setup(){
  
  LEDS.addLeds<WS2812B,LED_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
  
  for(uint8_t pixel = 0; pixel < NUM_LEDS; pixel++) { 
     leds[pixel] = CRGB::White;
      FastLED.show();
     // leds[pixel] = CRGB::Black;
      delay(500);
   }
   
 for (uint8_t i = BRIGHTNESS; i < (4 * BRIGHTNESS); i++)
 {
      LEDS.setBrightness(i);
      delay(100);
      FastLED.show();
 }
 for (uint8_t i = (4 * BRIGHTNESS); i > BRIGHTNESS; i--)
 {
   LEDS.setBrightness(i);
      delay(100);
      FastLED.show();
 }
 LEDS.setBrightness(BRIGHTNESS);
 FastLED.show();
  
}

void loop()
{
  
}
