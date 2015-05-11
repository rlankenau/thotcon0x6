#include <EEPROM.h>
#include "FastLED.h"
#include <IRLib.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <avr/interrupt.h>

#include "data.h"

#define SCREEN_MAIN             1
#define SCREEN_LIGHT            2
#define SCREEN_SYSOP            3
#define SCREEN_MESSAGES         4
#define SCREEN_SYSINFO          5
#define SCREEN_COMMANDS         6

#define STRAND 4
#define NUM_LEDS 6
#define IR_E 10
#define IR_R A0


/*
 * microsec - time spent trying to read
 */
uint32_t readIR(int32_t microsecs)
{
   char input[5];
   uint32_t checksum = 0x00000000;
   int pos = 0;
   int level = HIGH, last_level;
   int time_in = 0;
   while (microsecs > 0)
   {
      last_level = level;
      level = digitalRead(IR_R);
      if(level!=last_level)
      {
        
        if(level == LOW)
        {
          //rising - start of a new pulse
          time_in = 0;
        } 
        else
        {
          //falling - end of a pulse
          if(time_in > 100 && time_in < 300) {
             // Short pulse, leave it at 0.
             Serial.print("0");
             pos++;
          } else if (time_in > 500) {
             // long pulse, set it to 1;
             input[pos/8] |= (0x80 >> (pos%8));
             Serial.print("1");
             pos++;
          } else {
             pos = 0;
             memset(input, 0, 5);
             time_in = 0;
             level = HIGH;
             last_level = HIGH;
             Serial.print("Err:");
             Serial.println(time_in);
          }
          if(pos == 39)
          {
            /* Complete code - check checksum. */
            for(int i=0;i<4;i++)
            {
              checksum+=input[i];
            }
            if((checksum&0xFF) == (unsigned char)input[4])
            {
              uint32_t result;
              // Success!
              if(Serial) {
                Serial.println("Got code!");
                Serial.print(input[0], HEX);
                Serial.print(input[1], HEX);
                Serial.print(input[2], HEX);
                Serial.print(input[3], HEX);
                Serial.println();
              }
              result = (uint32_t)input;
              return result;
            } else {
               if(Serial)
              {
                Serial.println("BAD code!");
                Serial.print(input[0], HEX);
                Serial.print(input[1], HEX);
                Serial.print(input[2], HEX);
                Serial.print(input[3], HEX);
                Serial.println();                
              } 
            }
          }
        }
      }
      else
      {
        time_in+=25;
        if(level == HIGH)
        {
          // If it's been high for more than 1 ms, we're not in a code.
          if(time_in > 1000)
          {
              //reset
              memset(input, 0, 5);
              pos = 0;
              time_in = 0;
             level = HIGH;
             last_level = HIGH;
          }
        }
      }
      delayMicroseconds(22);
      microsecs -= 25;

   }
   return 0xFFFFFFFF;
}

void sendMicros(int32_t microsecs)
{
    while (microsecs > 0) {
        // 38 kHz is about 13 microseconds high and 13 microseconds low
       digitalWrite(IR_E, LOW);  // this takes about 3 microseconds to happen
       delayMicroseconds(10);         // hang out for 10 microseconds
       digitalWrite(IR_E, HIGH);   // this also takes about 3 microseconds
       delayMicroseconds(10);         // hang out for 10 microseconds

       // so 26 microseconds altogether
       microsecs -= 26;
  }
}

void sendShort()
{
   sendMicros(200); 
}

void sendLong()
{
  sendMicros(600);
}

void send32(uint32_t buf)
{
  char *bytes = (char *)&buf;
  uint32_t checksum = 0x00;
  checksum = bytes[0] + bytes[1] + bytes[2] + bytes[3];
  char check = checksum & (char)0xFF;
  
  
  for(int count=0;count<3;count++) 
  {
    for(int i=0;i<32;i++)
    {
      if((0x80000000>>i) & buf)
      {
        sendLong();
      } 
      else 
      {
        sendShort(); 
      }
      delayMicroseconds(100);
    }
    delayMicroseconds(5000);
  }
  
  for(int i=0;i<8;i++)
  {
    if((0x80 >> i) & check)
    {
      sendLong();
    } 
    else 
    {
      sendShort();  
    }
    delayMicroseconds(100);
  }
}



#define BRIGHTNESS 4

#define TERM (char)0xF8
#define SYSOP_BYTE (char)0x03
#define SYSOP_BIT (char)0x80

#define MAX_BRIGHTNESS 50

CRGB leds [NUM_LEDS];
uint8_t screen_state = SCREEN_MAIN;
int peekAddress = 3;
int peekLen = 32;
int p = 0x1;
uint8_t b = 4;
uint32_t hc = 0x00000000;

int serial_init = 0;
int terminated_serial = 0;

IRrecv My_Receiver(IR_R);
IRdecode My_Decoder;

void pixel_test()
{

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

volatile int f_wdt=1;




ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    Serial.begin(9600);
    f_wdt=1;
  }
  
  else
  {
    Serial.println("WDT!");
  }
}

void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}


void setup()
{
  int i=0;
  pinMode(IR_R, INPUT_PULLUP);
  pinMode(IR_E, OUTPUT);
  digitalWrite(IR_E, HIGH);
  
   ADCSRA = 0;
  
  /* 
  
  // Clear the reset flag. 
  MCUSR &= ~(1<<WDRF);
  
  // In order to change WDE or the prescaler, we need to
  // set WDCE (This will allow updates for 4 clock cycles).
   
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // set new watchdog timeout prescaler value 
  WDTCSR = 1<<WDP0 | 1<<WDP3; // 8.0 seconds 
  
  // Enable the WD interrupt (note no reset). 
  WDTCSR |= _BV(WDIE);
  
  */
  
  //Setup NeoPixel strip, turn everything off.
  FastLED.addLeds<NEOPIXEL, STRAND>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  for(i=0;i<NUM_LEDS;i++)
  {
      leds[i] = CRGB::Black;
  }
  FastLED.show();
  
  Serial.setTimeout(500);
  Serial.begin(2400);
  terminated_serial = 0;
  pixel_test();

}

void loop()
{
    //delayMicroseconds(random(50000,500000));  for testing ease
    char ir_data[4];
    uint32_t ir_in;
    
    char type = 0x0;  // serial input byte 
    char* buff = 0x0; //byte buff
    
    /*
    
    
    
    
    if (!Serial)
  { //everthing that happens when we're not connected is in this branch
    
     if(f_wdt == 1)
     {
    // neo blink
      leds[0] = CRGB::White;
        FastLED.show();
       delay(1000);
      leds[0] = CRGB::Black;
       FastLED.show();
    //
    
       //  clear the flag. 
       f_wdt = 0;
    
      // Re-enter sleep mode. 
       enterSleep();
       }
    
    
    
    
    
    
    */
    
    while(Serial && !terminated_serial)
    {
        Serial.flush();
        BBS_display_banner();
        BBS_show_menu();

        // wait for the strokes
        BBS_display_footer(); 
        type = BBS_handle_prompt();
      
       
        Serial.println(type);
      
        
        switch (type){
        
        case 'l' :
        case 'L' : { 
              if (screen_state == SCREEN_MAIN) 
                  screen_state = SCREEN_LIGHT;        
               break;
        }
        
        case 'k' :
        case 'K' : {
               screen_state= SCREEN_MAIN;
               break;
        }
        
        case '*' : {
               if (screen_state == SCREEN_MAIN) 
                  screen_state = SCREEN_SYSOP;
               break;
        }
        
        case 'i':
        case 'I' : {
              if (screen_state == SCREEN_MAIN) 
                  screen_state = SCREEN_SYSINFO;
              break;
        }
        
        case 'c' :
        case 'C' : {
              if (screen_state == SCREEN_MAIN) {
                  screen_state = SCREEN_COMMANDS;
              }
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
        case 'q' :
        case 'Q': {
             Serial.print("\033[2J");
             terminated_serial=1;
             break;
        }
        
        case 'p' :
        case 'P' :
        {
          if (screen_state == SCREEN_LIGHT) {
            Serial.setTimeout(500);
            Serial.print("Select Pixel [1 - 6]:");
            while (!Serial.available()){}
            //if (Serial.read() == '\n')
                   Serial.readBytesUntil('\n',buff,1);
                   p =  atoi(buff);
                   p = constrain(p,1,6);
            Serial.println(p);
          }
          break;
          
        }
        
        case 's' :
        case 'S' :
        {
             if (screen_state == SCREEN_SYSOP)
             {
                My_Receiver.resume(); 
           
                while (!My_Receiver.GetResults(&My_Decoder)); 
                    Serial.println(F(" Sharks swim best in quiet waves: "));
                    
                    My_Decoder.decode();
                    My_Decoder.DumpResults();
                    delay(1000);
                   My_Receiver.resume(); 
             
              delay(1000);
              Serial.println(F("Press any key to store signal "));
              while(!Serial.available());
              
           }
           break;
        }
        
        case 'b' : 
        case 'B' :
        {
             if (screen_state == SCREEN_LIGHT) {
             Serial.print(F("Enter Brightness [1 - 128]: "));
             Serial.setTimeout(500);
             while(!Serial.available());
             b = Serial.parseInt();
             b = constrain(b,1,128);
             Serial.print(F("\r\nSetting brightness level "));
             Serial.println(b);
             LEDS.setBrightness(b); 
             }
             break;
             
             
        }
        
        case 'd' :
        case 'D' :
        {
            if (screen_state == SCREEN_LIGHT)
            {
               if ( p == 0 ){
                  Serial.println(F("Use [P] to select a pixel!"));
                  delay(1000);
                  break;
               }
               if ( hc == 0 ){
                 Serial.println(F("Use [C] to set a color value"));
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
        case 'e':
        case 'E':
          ddt_peekpoke();
          break;
        case '\033':
        {
           Serial.flush(); 
          Serial.println("Escape is impossible"); 
           break;
        }
        
        case '?':
        {
          Serial.flush();
          Serial.println("Type a letter for you menu choice: [LIMC*]");
         break; 
        }
        
        default:{
           Serial.flush(); 
          Serial.println("Invalid choice!");
          
           break;
        }
      } // switch

    }

    //Read IR
    ir_in = readIR(10000000);

    if(ir_in!=0xFFFFFFFF) {
        // Update EEPROM with IR data
    }

    if(random(1,100) > (95 - (EEPROM.read(3) & 0x3F)))
    {
        char chance = EEPROM.read(3);
        if(chance < (char)0xFF) {
          chance+=1;
          EEPROM.write(3, chance);
        }
        
        for(int i=0;i<3;i++) {
          display_root(random(0,1018));
        }
        delay(1000);  
    }


    for(int i=0;i<3;i++) 
    {
        do_display();
        delay(100);
    }
    FastLED.clear(true);
    FastLED.show();
    //Send IR
    for(int i=0;i<4;i++)
      ir_data[i] = EEPROM.read(i);
    send32((uint32_t)ir_data);
}

void set_led(int led, char color)
{
    leds[led].r =  color & 0xC0;
    leds[led].g = (color & 0x30) << 2;
    leds[led].b = (color & 0x0C) << 4;
    leds[led] >>= (5-(color&0x3));
    
}

void display_voice(int offset)
{
    for(int i=0;i<3;i++) {
      char color = TERM;
      char curr_offset = offset;
      do {
          for(int i =0;i<NUM_LEDS;i++, curr_offset++)
          {
              color = EEPROM.read(curr_offset);
              if(color==TERM)
                  break;
              set_led(i, color);
          }
          FastLED.show();
          delay(200);
      } while(color!=TERM);
      FastLED.clear();
      delay(500);
    }
    
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
    char b = TERM;
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
    } while(curr_offset < 0x03FF);
}

void display_oper(int offset)
{

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
    char b = TERM;
    int curr_offset = offset;
    do { 
        for(int i=0;i<NUM_LEDS;i++)
        {
            b = EEPROM.read(curr_offset);
            if(b==TERM)
            {
                break;
            }
            set_led(i,b);
            curr_offset++;
        }
        FastLED.show();
        delay(1000);
    } while(b!=TERM && curr_offset < 0xD4);
        
}

void display_team(int offset)
{
    for(int i=0;i<3;i++) {
      char b = TERM;
      int curr_offset = offset;
      do {
          b = EEPROM.read(curr_offset++);
          if(b==TERM)
            break;
          for(int i=0;i<NUM_LEDS;i++) {
              set_led(i, b);
              FastLED.show();
              delay(10);
          }
      } while(b!=TERM);
    }
    
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


void
ddt_peekpoke()
{
  char command;
  peekAddress = 3;
  peekLen = 32;
  Serial.write("1K DDT-W VER 0.1\r\n");
  Serial.write("commands: \r\ndisplay: d[addr][,len]\r\nsubstitute: s[addr]\r\n\t k to exit\r\n");
  while(true) {
    Serial.write("- ");
    while(Serial.available() <= 0) ;
    command = Serial.read();
    Serial.print(command);
    switch(command) {
      case 'd': // Display memory - addr xx xx xx cccccccc
      case 'D':
        ddt_display();
        break;
      case 's':
      case 'S': // Enter memory modification: display addr and value, modify with new or cr, end on .
        ddt_substitute();
        break;
      case 'k':
      case 'K':
        return;
    }
    while(Serial.read()>0) ; // ToDo: get rid of when commands written
  }
}
// Read a line terminated by a cr
// returns only on first CR or 15 characters
// return pointer to null-terminated string
// Line can be max 15 chars long
char line[16];
char*
ddt_readLine()
{
  while(Serial.available()==0) ;
  for(int i = 0; i<15; i++) {
    line[i] = Serial.read();
    Serial.write(line[i]);
    if(line[i]=='\r') {
      line[i+1] = '\0';
      Serial.write(line);
      Serial.println();
      return line;
    }
    while(Serial.available()==0) ;
  }
  line[15] = '\0';
  return line;
}
void
ddt_display() {
  // '\n', or 'xx{0,3}\n' or 'xx,x{0,3}\n'
  ddt_readLine();
  char* l=line;
  if(isxdigit(*l)) {
    // a hex address
    peekAddress = 0;
    int i;
    for(i = 0; i<3 && isxdigit(l[i]); i++) {
      peekAddress <<= 4;
      byte x;
      if(l[i] < 0x3a) {
        x = l[i]-0x30;
      } else if (l[i] < 0x47) {
        x = l[i]-0x41;
      } else if (l[i] < 0x67) {
        x = l[i]-0x61;
      }
      peekAddress |= x;
    }
    if (l[i++] == ',') {
      // peek length
      peekLen = atoi(&l[i]);
    }
  }
  ddt_printLines(peekAddress,peekLen);
  peekAddress+=peekLen;
}

// ddt format print starting at addr for len bytes
// aaaa xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx cccccccccccccccc
void
ddt_printLines(int addr, int len)
{
  int current = addr;
  int end = addr+len;
  char clist[17];
  int i;
  while(current < end) {
    // initialize the character list at end of line
    for(i=0; i<16; i++) clist[i] = ' ';
    clist[16] = '\0';
     
    ddt_printAddr(current-current%16);
    for(i=0; i<current%16; i++) {
      clist[i] = ' ';
      Serial.print("   ");
    }
    do {
      char e_val = EEPROM.read(current);
      clist[i] = isprint(e_val) ? e_val : '.';
      Serial.print(byte(e_val&0xff), HEX);
      Serial.print(' ');
      current++;
      i++;
    } while(current%16 != 0 && current < end);
    Serial.print(clist);
    Serial.println();
  }
}

void
ddt_printAddr(int addr)
{
  char addr_str[5];
  char lead[9];
  strlcpy(lead, "0000", 9);
  itoa(addr, addr_str, 16);
  strlcat(lead, addr_str , 9);
  Serial.print(&lead[strlen(addr_str)]);
  Serial.write(' ');
}

void
ddt_substitute()
{
  Serial.read(); // remove initial 'cr'
  Serial.println();
  while(true) {
    if(peekAddress < 3 || peekAddress > 1023) peekAddress = 3;
    ddt_printAddr(peekAddress);
    Serial.print(EEPROM.read(peekAddress), HEX);
    Serial.print(" : ");
    int hexVal=-1;
    int readChar = -1;
    while (readChar != '\r') {
      while(Serial.available() == 0) ;
      readChar = Serial.read();
      Serial.print(char(readChar));
      switch(readChar) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          readChar = readChar - 0x30;
          if(hexVal==-1) hexVal = 0;
          hexVal = (hexVal << 4) | (readChar & 0xf);
          break;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
          readChar = readChar - 'A' + 10;
          if(hexVal==-1) hexVal = 0;
          hexVal = (hexVal << 4) | (readChar & 0xf);
          break;
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
          readChar = readChar - 'a' + 10;
          if(hexVal==-1) hexVal = 0;
          hexVal = (hexVal << 4) | (readChar & 0xf);
          break;
        case '.':
          hexVal = -1;
          break;
        case '\r':
          hexVal = (hexVal==-1) ? -2 : hexVal;
          break;
        default:
          return;
      }
    }
    Serial.println();
    if(hexVal==-1) return;
    if(hexVal>=0) EEPROM.write(peekAddress, char(hexVal));
    hexVal = -1;
    peekAddress++;
  }
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
              if (EEPROM.read(SYSOP_BYTE) & SYSOP_BIT){
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
              Serial.println("\t\t\t\t-=[System Commands]=-\r\n\t\t\t[E]EPROManip\t\tBac[K]\r\n");
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

