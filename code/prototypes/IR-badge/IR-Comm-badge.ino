//#include <Adafruit_NeoPixel.h>
#include <EEPROMex.h>
//#include <EEPROMVar.h>
#include <NewTone.h>
//#include "ircodes.h"
#include <avr/power.h>


/* Raw IR commander
 
 This sketch/program uses the Arduno and a PNA4602 to 
 decode IR received.  It then attempts to match it to a previously
 recorded IR signal
 
 Code is public domain, check out www.ladyada.net and adafruit.com
 for more tutorials! 
 */


// We need to use the 'raw' pin reading methods
// because timing is very important here and the digitalRead()
// procedure is slower!
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
#define IRpin_PIN      PIND
#define IRpin          4
//#define erasePin       2
#define ledPin         6
#define IRledPin1      13
#define IRledPin2      12
//#define soundPin       12
#define DEBUG
//#define NUMPIXELS 7

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS,ledPin, NEO_GRB + NEO_KHZ800);


// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 60000
#define NUMPULSES 50

// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 10 
//#define RESOLUTION 40 

// What percent we will allow in variation to match the same code
#define FUZZINESS 10

// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[NUMPULSES][2];  // pair is high and low pulse 
uint8_t currentpulse = 0; // index for pulses we're storing
int Signal[NUMPULSES];  // array where initial signal is stored for compare to set weapon value.  

void setup(void) {
  //clock_prescale_set(clock_div_1);
  Serial.begin(9600);
  Serial.println("Ready to for IR-Comm");
  //Serial.println(CLKPR);
  pinMode(ledPin, OUTPUT);
  pinMode(IRledPin1, OUTPUT);
  pinMode(IRledPin2, OUTPUT);
  digitalWrite(IRledPin1,LOW);
  digitalWrite(IRledPin2,LOW);
}

//void loop (void){
//  Serial.println("Sending code");
//  SendNikonCode();
//  delay (15 * 1000);
//}

void loop(void) {
  int numberpulses;
//  //First_signal
   int Signal_1 [14] = {226,2660,50,146,54,336,56,6304,226,2620,50,146,52,336};  
   int Signal_2 [14] = {440,1422,23,285,75,95,29,6153,442,1420,23,280,66,100}; 
//  int Signal_3 [20] = {236,56,120,54,120,56,118,56,60,56,62,54,60,56,62,54,120,56};  
//  int Signal_4 [20] = {236,54,62,54,62,56,60,56,60,56,62,54,62,54,62,54,120,56};  
  Serial.println("Sending Signal 1");
  Send_Signal_1();
  Serial.println("Sending Signal 2");
  Send_Signal_2();
  Serial.println("Listening for IR");
  numberpulses = listenForIR();   // we don't actually use the return value here.  
  //printpulses();
  //convert_to_array();
  //Serial.println("Got one signal I need one more to match against:");
  //delay(500);
  //numberpulses = listenForIR();
  //Serial.println("I got a second signal let me see if they match");
  if(IRcompare(numberpulses, Signal_1,25)){
      Serial.println("I heard Signal 1");
      blinkPattern(ledPin,3,500);
  }
  else if(IRcompare(numberpulses, Signal_2,25)){
      Serial.println("I heard Signal 2");
      blinkPattern(ledPin,6,300);
  }
//  else if(IRcompare(numberpulses, Signal_3,25)){
//      Serial.println("I heard Signal 2");
//  }
//  else if(IRcompare(numberpulses, Signal_4,25)){
//      Serial.println("I heard Signal 4");
//  }
//  else {
//      Serial.println("No signal matched");
//  }  
}
 
//KGO: added size of compare sample. Only compare the minimum of the two
boolean IRcompare(int numpulses, int MY_Signal[], int refsize) {
  //int count = min(10,refsize);
  int count = 7;
  #ifdef DEBUG
    Serial.print("count set to: ");
    Serial.println(count);
  #endif
  for (int i=0; i< count-1; i++) {
    int oncode = pulses[i][1] * RESOLUTION / 10;
    int offcode = pulses[i+1][0] * RESOLUTION / 10;
    
#ifdef DEBUG    
    Serial.print(oncode); // the ON signal we heard
    Serial.print(" - ");
    Serial.print(MY_Signal[i*2 + 0]); // the ON signal we want 
#endif   
    
    // check to make sure the error is less than FUZZINESS percent
    if ( abs(oncode - (MY_Signal[i*2 + 0]) <= (MY_Signal[i*2 + 0]) * FUZZINESS / 100)) {
#ifdef DEBUG
      Serial.print(" (ok)");
#endif
    } else {
#ifdef DEBUG
      Serial.print(" (x)");
#endif
      // we didn't match perfectly, return a false match
      return false;
    }
    
    
#ifdef DEBUG
    Serial.print("  \t"); // tab
    Serial.print(offcode); // the OFF signal we heard
    Serial.print(" - ");
    Serial.print(MY_Signal[i*2 + 1]); // the OFF signal we want 
#endif    
    
    if ( abs(offcode - (MY_Signal[i*2 + 1]) <= (MY_Signal[i*2 + 1]) * FUZZINESS / 100)) {
#ifdef DEBUG
      Serial.print(" (ok)");
#endif
    } else {
#ifdef DEBUG
      Serial.println(" (x)");
#endif
      // we didn't match perfectly, return a false match
      return false;
    }
    
#ifdef DEBUG
    Serial.println();
#endif
  }
  // Everything matched!
  return true;
}

int listenForIR(void) {
  currentpulse = 0;
  while (1) {
    uint16_t highpulse, lowpulse;  // temporary storage timing
    highpulse = lowpulse = 0; // start out with no pulse length
  
    while (IRpin_PIN & (1 << IRpin)) {
       // pin is still HIGH

       // count off another few microseconds
       highpulse++;
       delayMicroseconds(RESOLUTION);

       // If the pulse is too long, we 'timed out' - either nothing
       // was received or the code is finished, so print what
       // we've grabbed so far, and then reset
       
       // KGO: Added check for end of receive buffer
       if (((highpulse >= MAXPULSE) && (currentpulse != 0))|| currentpulse == NUMPULSES) {
         return currentpulse;
       }
    }
    // we didn't time out so lets stash the reading
    pulses[currentpulse][0] = highpulse;
  
    // same as above
    while (! (IRpin_PIN & _BV(IRpin))) {
       // pin is still LOW
       lowpulse++;
       delayMicroseconds(RESOLUTION);
        // KGO: Added check for end of receive buffer
        if (((lowpulse >= MAXPULSE)  && (currentpulse != 0))|| currentpulse == NUMPULSES) {
         return currentpulse;
       }
    }
    pulses[currentpulse][1] = lowpulse;

    // we read one high-low pulse successfully, continue!
    currentpulse++;
  }
}
void printpulses(void) {
  Serial.println("\n\r\n\rReceived: \n\rOFF \tON");
  for (uint8_t i = 0; i < currentpulse; i++) {
    Serial.print(pulses[i][0] * RESOLUTION, DEC);
    Serial.print(" usec, ");
    Serial.print(pulses[i][1] * RESOLUTION, DEC);
    Serial.println(" usec");
  }
  
  // print it in a 'array' format
  Serial.println("int IRsignal[] = {");
  Serial.println("// ON, OFF (in 10's of microseconds)");
  for (uint8_t i = 0; i < currentpulse-1; i++) {
    Serial.print("\t"); // tab
    Serial.print(pulses[i][1] * RESOLUTION / 10, DEC);
    Serial.print(", ");
    Serial.print(pulses[i+1][0] * RESOLUTION / 10, DEC);
    Serial.println(",");
  }
  Serial.print("\t"); // tab
  Serial.print(pulses[currentpulse-1][1] * RESOLUTION / 10, DEC);
  Serial.print(", 0};");
}


void convert_to_array (void) {
  // print it in a 'array' format
  //Serial.println("int IRsignal[] = {");
  Serial.println("// ON, OFF (in 10's of microseconds)");
  for (uint8_t i = 0; i < currentpulse-1; i++) {
    //Serial.print("\t"); // tab
    Signal[i*2 + 0] = pulses[i][1] * RESOLUTION / 10;
    Serial.print(Signal[i*2 + 0]);
    Serial.print("  ");
    Signal[i*2 + 1] = pulses[i+1][0] * RESOLUTION / 10;
    Serial.println(Signal[i*2 + 1]);
  }
  Serial.println("\n");
}

void print_signal (void) {
  //print it in a 'array' format
  Serial.println("int IRsignal[] =");
  Serial.print("{");
  for (uint8_t i = 0; i < currentpulse-1; i++) {
    //Serial.print("\t"); // tab
    Signal[i*2 + 0] = pulses[i][1] * RESOLUTION / 10;
    Serial.print(Signal[i*2 + 0]);
    Serial.print(", ");
    //Signal[i*2 + 1] = pulses[i+1][0] * RESOLUTION / 10;
    Serial.print(Signal[i*2 + 1]);
  }
  Serial.print("}");
  Serial.println("\n");
}

//void hitSFX(){
//  for (int i = 0; i < 3; i++){
//        NewTone(soundPin, 2093, 100);
//        delay(50);
//        //NewTone(soundPin, 1568, 100);
//        //delay(100);
//        NewTone(soundPin, 1175, 100);
//        delay(50);
//      }
//}
//

void blinkPattern(int tempPin, int numBlinks, int tempDuration){
  for (int i = 1; i <= numBlinks;i++){ 
    digitalWrite(tempPin, HIGH);
    //hitSFX();
    delay(tempDuration);
    digitalWrite(tempPin, LOW);
    //hitSFX();
    delay(tempDuration);
  }
}

// This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
 
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   //digitalWrite(IRledPin1, LOW);  // this takes about 3 microseconds to happen
   digitalWrite(IRledPin2, HIGH);
   delayMicroseconds(10);         // hang out for 10 microseconds, you can also change this to 9 if its not working
   //digitalWrite(IRledPin1, HIGH);   // this also takes about 3 microseconds
   digitalWrite(IRledPin2, LOW);
   delayMicroseconds(10);         // hang out for 10 microseconds, you can also change this to 9 if its not working
 
   // so 26 microseconds altogether
   microsecs -= 26;
  }
 
  sei();  // this turns them back on
}
 
void Send_Signal_1() {
  // This is the code for my particular Nikon, for others use the tutorial
  // to 'grab' the proper code from the remote
 
  pulseIR(2000);
  delay(27);
  pulseIR(440);
  delayMicroseconds(1500);
  pulseIR(460);
  delayMicroseconds(3440);
  pulseIR(480);
  //digitalWrite(IRledPin1,LOW);
  //digitalWrite(IRledPin2,LOW);
 
 
  delay(65); // wait 65 milliseconds before sending it again
 
  pulseIR(2000);
  delay(27);
  pulseIR(440);
  delayMicroseconds(1500);
  pulseIR(460);
  delayMicroseconds(3440);
  pulseIR(480);
  //digitalWrite(IRledPin1,LOW);
  //digitalWrite(IRledPin2,LOW);
  
}

void Send_Signal_2() {
  // This is the code for my particular Nikon, for others use the tutorial
  // to 'grab' the proper code from the remote
 
  pulseIR(4000);
  delay(15);
  pulseIR(200);
  delayMicroseconds(3000);
  pulseIR(660);
  delayMicroseconds(1000);
  pulseIR(260);

 
 
  delay(65); // wait 65 milliseconds before sending it again
 
  pulseIR(4000);
  delay(15);
  pulseIR(200);
  delayMicroseconds(3000);
  pulseIR(660);
  delayMicroseconds(1000);
  pulseIR(260);

  
}

//void neo_LED (int tempDuration){
//  for(int i=0;i<NUMPIXELS;i++){
//
//    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
//    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
//
//    pixels.show(); // This sends the updated pixel color to the hardware.
//
//    delay(tempDuration); // Delay for a period of time (in milliseconds).
//
//  }  
//   
//}


