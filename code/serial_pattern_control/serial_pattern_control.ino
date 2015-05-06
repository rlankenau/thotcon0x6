#include <FastLED.h>
#include <IRLib.h>

#define IR_E 10
#define IR_R A0
#define LED_PIN     4
#define BRIGHTNESS  8
#define NUM_LEDS  6

CRGB leds[NUM_LEDS];

uint8_t fib[] = {1,1,2,3,5,8};

void setup() {
  pinMode(LED_PIN,OUTPUT);
  
  pinMode(IR_R, INPUT_PULLUP);
  
  pinMode(IR_E, OUTPUT);
  digitalWrite(IR_E, HIGH);
  
  //delay(3000);
  
  LEDS.addLeds<WS2812B,LED_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
  
  for(uint8_t pixel = 0; pixel < NUM_LEDS; pixel++) { 
     leds[pixel] = CRGB::White;
      FastLED.show();
      leds[pixel] = CRGB::Black;
      delay(250*fib[NUM_LEDS-pixel]);
   }

  Serial.begin(9600);  

}

void loop()
{
  char type = 0x0;
  if(Serial)
  {
    Serial.flush();
    Serial.println("Connected to THOTCON 0x6");
    Serial.println("Enter Badge Type:" );
    Serial.print(" [HVOXR] :");
    while (!Serial.available()){}
       
      type = Serial.read();
       Serial.print("Displaying ");
       Serial.println(type);
      switch (type){
        case 'H':{
           Serial.println("Hacker");
           for(uint8_t pixel = 0; pixel < NUM_LEDS; pixel++) { 
              leds[pixel] = CRGB::White;
              FastLED.show();
              leds[pixel] = CRGB::Black;
              delay(250*fib[NUM_LEDS-pixel]);
           }
           break;
        }
        Serial.flush();
        case 'V':{
          Serial.println("Voice");
          for(uint8_t pixel = 0; pixel < NUM_LEDS; pixel++) { 
              leds[pixel] = CRGB::Blue;
              FastLED.show();
              leds[pixel] = CRGB::Black;
              delay(250*fib[NUM_LEDS-pixel]);
           }
           break;
        }
        Serial.flush();
        case 'O':
        {
          Serial.println("Oper");
          for(uint8_t pixel = 0; pixel < NUM_LEDS; pixel++) { 
              leds[pixel] = CRGB::Red;
              FastLED.show();
              leds[pixel] = CRGB::Black;
              delay(250*fib[NUM_LEDS-pixel]);
           }
           break;
        }
        Serial.flush();
      case 'X':
      {
         Serial.println("VIP");
         for(uint8_t pixel = 0; pixel < NUM_LEDS; pixel++) { 
              leds[pixel] = CRGB::Green;
              FastLED.show();
              leds[pixel] = CRGB::Black;
              delay(250*fib[NUM_LEDS-pixel]);
         }
         Serial.flush();
         break;
      }
        case 'R':
        {
         Serial.println("Root");
         for(uint8_t pixel = 0; pixel < NUM_LEDS; pixel++) { 
              leds[pixel] = CRGB::Yellow;
              FastLED.show();
              leds[pixel] = CRGB::Black;
              delay(250*fib[NUM_LEDS-pixel]);
           }
           Serial.flush();
           break;
        }
        default:{
           Serial.println("Invalide Type!");
           Serial.println("Select [HVOXR]");
           Serial.flush();
           break;
        }
      } // switch
      
  } // if read
 //} //if Serial 
} // loop()
