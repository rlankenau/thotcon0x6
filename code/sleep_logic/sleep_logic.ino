#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <FastLED.h>


#define IR_E 10
#define IR_R A0
#define LED_PIN     4
#define BRIGHTNESS  8
#define NUM_LEDS  6

volatile int f_wdt=1;

CRGB leds[NUM_LEDS];


ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    Serial.println("WDT!");
  }
}



void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
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
  Serial.begin(9600);
  Serial.println("Initialising...");
  delay(100); //Allow for serial print to complete.

   ADCSRA = 0;
  
  pinMode(LED_PIN,OUTPUT);
  pinMode(IR_R, INPUT_PULLUP);
  pinMode(IR_E, OUTPUT);
  digitalWrite(IR_E, HIGH);

  LEDS.addLeds<WS2812B,LED_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);

  /*** Setup the WDT ***/
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
  
  Serial.println("Initialisation complete.");
  delay(100); //Allow for serial print to complete.
}




void loop()
{
  if (!Serial){ //everthing that happens when we're not connected is in this branch
    
     if(f_wdt == 1)
     {
    /* Blink the first LED pixel */
      leds[0] = CRGB::White;
        FastLED.show();
       delay(1000);
      leds[0] = CRGB::Black;
       FastLED.show();
    /* main loop fits in place of this blink */
    
       /*  clear the flag. */
       f_wdt = 0;
    
      /* Re-enter sleep mode. */
       enterSleep();
       }
       
      else
     {
        /* Do nothing. */
     }
  }
  
  // everyting when we're  plugged in happens here
}
