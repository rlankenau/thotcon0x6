#include <FastLED.h>
#include <IRLib.h>
#include <EEPROM.h>

#define IR_E 10
#define IR_R A0
#define LED_PIN     4
#define BRIGHTNESS  8
#define NUM_LEDS  6

#define SCREEN_MAIN             1
#define SCREEN_LIGHT            2
#define SCREEN_SYSOP            3
#define SCREEN_MESSAGES         4
#define SCREEN_SYSINFO          5
#define SCREEN_COMMANDS         6





//Flags

#define FLAG_BTYPE_W88      0x01
#define FLAG_BTYPE_ROOT     0x02
#define FLAG_BTYPE_OPER     0x04
#define FLAG_BTYPE_VIP      0x08
#define FLAG_BTYPE_VOICE    0x10
#define FLAG_BTYPE_HACKER   0x20
#define FLAG_MODE_BLUE      0x40
#define FLAG_MODE_RED       0x80

//EEPROM offset table
#define BADGE_BLOCK_OFF     0
#define OFFSET_BLOCK_OFF    4
#define COLOR_BLOCK_OFF     12
#define SCREEN_BLOCK_OFF    268


#define COLOR_STREAM_END    0xF8

int p = 0x0;  // pixel
uint8_t b = 4;   // brightness
uint32_t hc = 0x000000; //hex color for pixel

typedef struct eeprom_badge_data_t_stct {
    uint16_t id;
    uint8_t  flags;
    uint8_t  data;
} eeprom_badge_date_t;

typedef struct eeprom_screen_ident_t_stct {
    uint8_t offset;
    uint8_t len;
} eemprom_screen_ident_t_stct;

CRGB leds[NUM_LEDS];

uint8_t screen_state = SCREEN_MAIN;

void setup() {
  pinMode(LED_PIN,OUTPUT);
  pinMode(IR_R, INPUT_PULLUP);
  pinMode(IR_E, OUTPUT);
  digitalWrite(IR_E, HIGH);

  LEDS.addLeds<WS2812B,LED_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
  
  /*for (i=SCREEN_BLOCK_OFF)
  {
      EEPROM.write(SCREEN_BLOCK_OFF);
  
  
  }
  */
  for(uint8_t pixel = 0; pixel < NUM_LEDS; pixel++) { 
     leds[pixel] = CRGB::White;
      FastLED.show();
      leds[pixel] = CRGB::Black;
      delay(500);
   }
   Serial.begin(2400); 
  
}

const int BUFLEN=24;
char buf[BUFLEN+1]={'\0'};

char* eprom2string(int addr, int len)
{
   char* fill = buf;
   int i;
   for(i = addr; i<addr+len && i<addr+BUFLEN; i++, fill++) {
       *fill = EEPROM.read(i);
   }
   buf[i] = 0;
   return buf;
}

void EEPROM_load(char *str,uint16_t off)
{
uint16_t pos = SCREEN_BLOCK_OFF + off;
while(*str){
   EEPROM.write(pos++,(byte)(*str++));
  }
}

void BBS_display_banner()
{
    if (Serial){
         delay(500); 
         Serial.print("\033[2J"); //VT100 clear
         Serial.print("\033[H");  //home position
         //ascii art banner string throwing it in progmem
         Serial.print(F("_________          _______ _________ _______  _______  _          _______            ______ \n\r\\__   __/| \\     /|( ___   \\__   __/(  ____ \\(  ___  )( (    /|  (  __   )|\\     /| / ____ \\\n\r   ) (   | )   ( || (   ) |   ) (   | (    \\/| (   ) ||  \\  ( |  | (  )  |( \\   / )( (    \\/\n\r   | |   | (___) || |   | |   | |   | |      | |   | ||   \\ | |  | | /   | \\ (_) / | (____  \n\r   | |   |  ___  || |   | |   | |   | |      | |   | || (\\ \\) |  | (/ /) |  ) _ (  |  ___ \\ \n\r   | |   | (   ) || |   | |   | |   | |      | |   | || | \\   |  |   / | | / ( ) \\ | (   ) )\n\r   | |   | )   ( || (___) |   | |   | (____--| (___) || )  \\  |  |  (__) |( /   \\ )( (___) )\n\r   )_(   |/     \\|(_______)   )_(   (_______/(_______)|/    )_)  (_______)|/     \\| \\_____/ \n\r                                                                                            \n\r               ______   _______  ______   _______  _______    ______   _______              \n\r              (  ___ \\ (  ___  )(  __  \\ (  ____ \\(  ____ \\  (  ___ \\ (  ____ \\             \n\r     ___  ___ | (   ) )| (   ) || (  \\  )| (    \\/| (    \\/  | (   ) )| (    \\/ ___  ___    \n\r    (___)(___)| (__/ / | (___) || |   ) || |      | (__      | (__/ / | (_____ (___)(___)   \n\r     ___  ___ |  __ (  |  ___  || |   | || | ____ |  __)     |  __ (  (_____  ) ___  ___    \n\r    (___)(___)| (  \\ \\ | (   ) || |   ) || | \\_  )| (        | (  \\ \\       ) |(___)(___)   \n\r              | )___) )| )   ( || (__/  )| (___) || (____--  | )___) )/\\____) |             \n\r              |/ \\___/ |/     \\|(______/ (_______)(_______/  |/ \\___/ \\_______)             \n\r"));
         Serial.println("\t\t\t NGxsIHkwdXIgYjRzMyA0cjMgYjNsMG5nIHQwIHVzCg\r\n\r\n");
  
         
  }
}



void BBS_show_menu()
{  
     if (Serial){
         delay(500);
         switch (screen_state){
           case SCREEN_MAIN:
           {
              Serial.println("\t\t\t\t\t-=[Main Menu]=-\r\n\t[L]ighting Control\t\tSystem [I]nfo\t\t[*] Sysop Menu\r\n\t[M]essage Base\t\t\t[C]ommmands\t\t[Q]uit\r\n\r\n"); //115
              break; 
           }
              
          case SCREEN_LIGHT: {
              Serial.println("\t\t\t\t-=[Lighting Menu]=-\r\n\t[P]ixel Select\t\t[C]olor Select\t\t[B]rightness Select\r\n\t[D]isplay\t\tBac[K]\r\n\r\n"); //101
              Serial.print("\tActive pixel: [");
              Serial.print(p,DEC);
              Serial.print("]\t\tColor :[0x");
              Serial.print(leds[p-1].r,HEX);
              Serial.print(leds[p-1].g,HEX);
              Serial.print(leds[p-1].b,HEX);
              Serial.print("]\tArray Brightness [");
              Serial.print(b,DEC);
              Serial.println("]\r\n\r\n");
              break; 
           }
          case SCREEN_SYSOP:     {
              if (!(EEPROM.read(OFFSET_BLOCK_OFF))){
                 Serial.println("\t\t\t\tBBS SYSOP ACCESS GRANTED!!\r\n\tIR [S]hark\t\tIR Pla[Y]\t\tPoo[F]\r\n\t\t\t\tBac[K]");
              }
              else{
                 screen_state = SCREEN_MAIN;
                 Serial.println("\t\t\tYou are not a sysop...  Sysop permissions required. Bac[K]\r\n\r\n");
              }
              break; 
           }      
           case SCREEN_MESSAGES: {
             // Needs clever content
              break; 
           }
           case SCREEN_SYSINFO: {
              uint16_t free = 0;
              
              Serial.println(F("\t\t\t\t\t-=[System Info]=-\r\n\tSoftware Installed\t\tHardware Modes\t\tBac[K]\r\n\tAVR OS\t\tSRAM\t\tCPU:  Mega32u4\r\n\tPixelDrive\tSRAM\t\tClock: 16Mhz\r\n\tMesgBlock\tSRAM\r\n\tIR Shark\tSRAM\r\n\tEEPROManip\tSRAM\r\n"));
              for (uint16_t i=0; i < 1024; i++)
                 free += ((EEPROM.read(i) == 0xFF) ? 1 : 0);
              Serial.print("\t");
              Serial.print(free,DEC);
              Serial.println("B ROM FREE\r\n");
              break; 
           }
           case SCREEN_COMMANDS: {
              Serial.println("\t\t\t\t-=[System Commands]=-\r\n\t\t\tP[E]ek\t\tP[O]ke\t\tBac[K]\r\n");
              break; 
           }
           default: 
           break;
         }
     }
}

void BBS_display_footer()
{
  if (Serial){  
    Serial.print("|[TL: 00:13:37] | [operator:0x");
    Serial.print((unsigned char)EEPROM.read(BADGE_BLOCK_OFF),HEX);
    Serial.print((unsigned char)EEPROM.read(BADGE_BLOCK_OFF+1),HEX);
    Serial.print("] | [Messages:");
    Serial.print((unsigned char)EEPROM.read(1023),DEC);
    Serial.println("] | [Serial:Connected] |  VT100 |  2400 8N1 |");
  
  }
}

byte BBS_handle_prompt(){
  if(Serial){
   Serial.print("Enter Menu Choice:" );
    
  }
  while (!Serial.available()){}
  return Serial.read();
    
}

void loop(){
  char type = 0x0;  // serial input byte 
  char* buff = 0x0; //byte buff
  
   
  if(Serial)
  {
    Serial.flush();
    BBS_display_banner();
    //Serial.println("Connected to THOTCON 0x6");
    
      BBS_show_menu();
     // wait for the strokes
      BBS_display_footer(); 
      type = BBS_handle_prompt();
      
       
       Serial.println(type);
      switch (type){
        
        case 'l' :
        case 'L' : {
               screen_state = SCREEN_LIGHT;        
               break;
        }
        case 'k' :
        case 'K' : {
               screen_state= SCREEN_MAIN;
               break;
        }
        case '*' : {
               screen_state = SCREEN_SYSOP;
               break;
        }
        case 'i':
        case 'I' : {
               screen_state = SCREEN_SYSINFO;
              break;
        }
        case 'c' :
        case 'C' : {
              if (screen_state == SCREEN_MAIN)
                  screen_state = SCREEN_COMMANDS;
              if (screen_state == SCREEN_LIGHT)
              {
                   Serial.print("\r\nInput Color value for pixel ");
                   Serial.print(p,DEC);
                   Serial.print(" {Red}: ");
                   while(!Serial.available());
                       int red = Serial.parseInt();
                       red = constrain(red,0,255) ;
                       Serial.print(red,DEC);
                       
                       Serial.print("\t {Green}: ");
                       while(!Serial.available());
                       int green = Serial.parseInt();
                       green = constrain(green,0,255);
                       Serial.print(green,DEC);
                        
                       Serial.print("\t {Blue}: ");
                       while(!Serial.available());
                       int blue = Serial.parseInt();
                        blue = constrain(blue,0,255);
                       Serial.println(blue,DEC); 
                       
                               
                               leds[p-1].r = red;
                               leds[p-1].g = green;
                               leds[p-1].b = blue;

                               Serial.print("\r\n Config val: 0x");
                               Serial.print(red, HEX);
                               Serial.print(green, HEX);
                              Serial.println(blue, HEX);
                              delay(500);
                              hc=1;
                   }  
               break;
        }
             // } 
            //  break;
       
        case 'q' :
        case 'Q': {
             Serial.print("\033[2J");
             break;
        }
        case 'p' :
        case 'P' :
        {
            Serial.setTimeout(300);
            Serial.print("Select Pixel [0 - 6]:");
            while (!Serial.available()){}
            //if (Serial.read() == '\n')
                   Serial.readBytesUntil('\n',buff,1);
                   p =  atoi(buff);
                   p = constrain(p,1,6);
            Serial.println(p);
            break;
        }
        case 'b' : 
        case 'B' :
        {
             Serial.print("Enter Brightness [1 - 128]: ");
             Serial.setTimeout(300);
             while(!Serial.available());
             b = Serial.parseInt();
             b = constrain(b,1,128);
             Serial.print("\r\nSetting brightness level ");
             Serial.println(b);
             LEDS.setBrightness(b); 
             break; 
        }
        case 'd' :
        case 'D' :
        {
            if (screen_state == SCREEN_LIGHT)
            {
               if ( p == 0 ){
                  Serial.println("Use [P] to select a pixel!");
                  delay(1000);
                  break;
               }
               if ( hc == 0 ){
                 Serial.println("Use [C] to set a color value");
                 delay(1000);
                  break;
               }
              FastLED.show();  
           }
            else {
              Serial.println("Invalid Entry!");
              Serial.flush();
            }
          
            break;  
        }
              /*case 'H':{
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
        }*/
        case '\033':
        {
           Serial.flush(); 
          Serial.println("Escape is impossible"); 
           break;
        }
        case '?':
        {
          Serial.flush();
          Serial.println("Options: [LIMC*]");
         break; 
        }
        default:{
           Serial.flush(); 
          Serial.println("Invalid choice!");
          
           break;
        }
      } // switch
      
  } // if read
 //} //if Serial 
} // loop()



