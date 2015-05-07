#include <FastLED.h>
#include <IRLib.h>

#define IR_E 10
#define IR_R A0
#define LED_PIN     4
#define BRIGHTNESS  8
#define NUM_LEDS  6



CRGB leds[NUM_LEDS];

uint8_t fib[] = {1,1,2,3,5,8};
uint8_t clr = 126;

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
   Serial.begin(2400); 
  
}

void BBS_display_banner()
{
    if (Serial){
         delay(500); 
         Serial.print("\033[2J"); //VT100 clear
         Serial.print("\033[H");  //home position
         //ascii art banner string throwing it in progmem
         Serial.print(F("_________          _______ _________ _______  _______  _          _______            ______ \n\r\\__   __/| \\     /|( ___   \\__   __/(  ____ \\(  ___  )( (    /|  (  __   )|\\     /| / ____ \\\n\r   ) (   | )   ( || (   ) |   ) (   | (    \\/| (   ) ||  \\  ( |  | (  )  |( \\   / )( (    \\/\n\r   | |   | (___) || |   | |   | |   | |      | |   | ||   \\ | |  | | /   | \\ (_) / | (____  \n\r   | |   |  ___  || |   | |   | |   | |      | |   | || (\\ \\) |  | (/ /) |  ) _ (  |  ___ \\ \n\r   | |   | (   ) || |   | |   | |   | |      | |   | || | \\   |  |   / | | / ( ) \\ | (   ) )\n\r   | |   | )   ( || (___) |   | |   | (____--| (___) || )  \\  |  |  (__) |( /   \\ )( (___) )\n\r   )_(   |/     \\|(_______)   )_(   (_______/(_______)|/    )_)  (_______)|/     \\| \\_____/ \n\r                                                                                            \n\r               ______   _______  ______   _______  _______    ______   _______              \n\r              (  ___ \\ (  ___  )(  __  \\ (  ____ \\(  ____ \\  (  ___ \\ (  ____ \\             \n\r     ___  ___ | (   ) )| (   ) || (  \\  )| (    \\/| (    \\/  | (   ) )| (    \\/ ___  ___    \n\r    (___)(___)| (__/ / | (___) || |   ) || |      | (__      | (__/ / | (_____ (___)(___)   \n\r     ___  ___ |  __ (  |  ___  || |   | || | ____ |  __)     |  __ (  (_____  ) ___  ___    \n\r    (___)(___)| (  \\ \\ | (   ) || |   ) || | \\_  )| (        | (  \\ \\       ) |(___)(___)   \n\r              | )___) )| )   ( || (__/  )| (___) || (____--  | )___) )/\\____) |             \n\r              |/ \\___/ |/     \\|(______/ (_______)(_______/  |/ \\___/ \\_______)             \n\r"));
         Serial.println("\t\t\tNGxsIHkwdXIgYjRzMyA0cjMgYjNsMG5nIHQwIHVzCg==\r\n\r\n");
         Serial.println("|[TL: 00:13:37] | [operator:0x0224] | [Messages:08]  |   [Serial:Connected]|    2400 8N1 |");
  }
}

void BBS_main_menu()
{
     if (Serial){
         delay(500);
     }
}
void loop(){
   char type = 0x0;
  if(Serial)
  {
    Serial.flush();
    BBS_display_banner();
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
              delay(500);
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
              delay(500);
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
              delay(500);
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
              delay(500);
         }
         Serial.flush();
         break;
      }
        case 'R':
        {
         Serial.println("Root");
         Serial.flush();
         for(uint8_t pixel = 0; pixel < NUM_LEDS; pixel++) { 
              Serial.println(pixel);
              leds[pixel] = CRGB::Yellow;
              FastLED.show();
              leds[pixel] = CRGB::Black;
              delay(500); 
              
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

