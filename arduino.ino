#include "FastLED.h"
#include <SoftwareSerial.h>
#include <pixeltypes.h>
#define NUM_LEDS 60
#define LED_TYPE WS2812B 
#define COLOUR_ORDER GRB
#define RESET 5   // Reset Pin location
#define STROBE 4  // we output values to these to control the shield
#define LEFT A0   // inputs from ICs on the shield
#define RIGHT A1  
// Define Colours
#define Blue CRGB(0x0000FF)       // A   
#define Green CRGB(0x008000)      // B
#define HotPink CRGB(0xFF69B4)    // C
#define Orange CRGB(0xFFA500)     // D
#define Red CRGB(0xFF0000)        // E
#define Purple CRGB(0x800080)     // F
#define Yellow CRGB(0xFFFF00)     // G
#define Turquoise CRGB(0x40E0D0)  // H
#define Salmon CRGB(0xFA8072)     // I
#define Fuchsia CRGB(255,0, 255)  // J
#define Gold CRGB(255, 215, 0)    // K
int left[7];   // holds 7 frequencies for the left side
int right[7];  // right side
int band;
int brightness = 60;
const int LED_PIN = 6;
CRGB leds[NUM_LEDS];  // Each index has 3 bytes of data, 1 byte for R, G, B each
SoftwareSerial bluetooth(0, 1); 
String state;
CRGB colour;
int whichBand;

void setup() {
  pinMode(RESET, OUTPUT); // we will be outputting signals (High->Low) to RESETSparkfun & separate the data it had for the previous seven bands from the incoming data
  pinMode(STROBE, OUTPUT); // we do the same w strobe so that we can tell the sparkfun to cycle through the 7 bands of audio information
  pinMode(LEFT, INPUT); // We have left and right audio, 
  pinMode(RIGHT, INPUT);
  digitalWrite(RESET, LOW); // we initialize RESET and STROBE to low
  digitalWrite(STROBE, LOW);
  delay(5);
  Serial.begin(38400);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOUR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  bluetooth.begin(9600);
}

void loop(){
    if (Serial.available() > 0) { // get data from App
    state = Serial.readString();
    switch (state[0]){   // first character: brightness, value: a-f
      case 'a':
        FastLED.setBrightness(42);
        Serial.println("Light Brightness at 0%");
        break;
      case 'b':
        FastLED.setBrightness(84);
        Serial.println("Light Brightness at 20%");
        break;
      case 'c':
        FastLED.setBrightness(126);
        Serial.println("Light Brightness at 40%");
        break;
      case 'd':
        FastLED.setBrightness(168);
        Serial.println("Light Brightness at 60%");
        break;
      case 'e':
        FastLED.setBrightness(210);
        Serial.println("Light Brightness at 80%");
        break;
      case 'f':
        FastLED.setBrightness(252);
        Serial.println("Light Brightness at 100%");
        break;
      default:
        Serial.println("Invalid character");
        break;
    }
    switch (state[1]){ // second character: pattern, value: 0-2
      case '0':
        // Recursion,  Switch on colour
        colour = determineColour(state);
        recursion_lit(0,1, colour);
        Serial.println("Recursion");
        break;
      case '1':
        colour = determineColour(state);
        whichBand = determineFreq(state);
        readShield();
        ontoLights(whichBand, colour, state);
        Serial.println("REACT Setting");
        break;
      case '2':
        rainbow();
        Serial.println("RAINBOW Setting");
        break;
      default:
        Serial.println("Invalid character");
        break;
      } 
    }
}

CRGB determineColour(String state){
  switch (state[3]){
      case 'A':
        return Blue;
        Serial.println("Color of LED is BLUE");
        break;
      case 'B':
        return Green;
        Serial.println("Color of LED is GREEN");
        break;
      case 'C':
        return HotPink;
        Serial.println("Color of LED is HOT PINK");
        break;
      case 'D':
        return Orange;
        Serial.println("Color of LED is ORANGE");
        break;
      case 'E':
        return Red;
        Serial.println("Color of LED is RED");
        break;
      case 'F':
        return Purple;
        Serial.println("Color of LED is PURPLE");
        break;
      case 'G':
        return Yellow;
        Serial.println("Color of LED is YELLOW");
        break;
      case 'H':
        return Turquoise;
        Serial.println("Color of LED is TURQUOISE");
        break;
      case 'I':
        return Fuchsia;
        Serial.println("Color of LED is FUCHSIA");
        break;
      case 'J':
        return Gold;
        Serial.println("Color of LED is GOLD");
        break;
      case 'K':
        return Salmon;
        Serial.println("Color of LED is SALMON");
        break;
      case 'L':
        return Salmon; // Outlier
        Serial.println("Color of LED is RAINBOW");
        break;
      default:
        Serial.println("Invalid character");
        break;
    }
}

int determineFreq(String state){
  switch (state[2]){
      case '3':
        return 0;
        Serial.println("Frequency at 63Hz");
        break;
      case '4':
        return 1;
        Serial.println("Frequency at 160Hz");
        break;
      case '5':
        return 2;
        Serial.println("Frequency at 400Hz");
        break;
      case '6':
        return 3;
        Serial.println("Frequency at 1kHz");
        break;
      case '7':
        return 4;
        Serial.println("Frequency at 2.5kHz");
        break;
      case '8':
        return 5;
        Serial.println("Frequency at 6.25kHz");
        break;
      case '9':
        return 6;
        Serial.println("Frequency at 16kHz");
        break;
      default:
        Serial.println("Invalid character");
        break;
      }
}


void readShield(){
  digitalWrite(RESET, HIGH);
  digitalWrite(RESET, LOW); // Reset ICs to separate from previous bands
  
  for (band = 0; band < 7; band++){
    // this is for it to read the data of one band and then stop it
    digitalWrite(STROBE, HIGH);                   
    delay(1);
    digitalWrite(STROBE, LOW);
    delay(1);
    left[band] = analogRead(LEFT);
    right[band] = analogRead(RIGHT);
  }
}

void fill_rainbow2( struct CRGB * pFirstLED, int numToFill,uint8_t initialhue, uint8_t deltahue ){
    CHSV hsv; // declaring the struct?
    hsv.hue = initialhue; // initializing its member vars
    hsv.val = 255;
    hsv.sat = 255;
    for( int i = 0; i < numToFill; i++) {
        pFirstLED[i] = hsv;
        hsv.hue += deltahue;
    }
}
void ontoLights(int targetBand, CRGB displayColour, String state){
  int react = map(left[targetBand], 0, 1023, 0, NUM_LEDS-1); // map the frequency 'concentration' values of 0 - 1023 to 0 - 60 LEDs
  FastLED.clear();
  if (state[3] == 'L'){
    fill_rainbow2(leds, react, 0, 4); // fill the 'react' amount of LEDs
  }
  else{
      for(int i = 0; i < react; i++){
        leds[i] = displayColour; // fill the 'react' amount of LEDS w a specific colour
      }
  }
  FastLED.show();
}

int recursion_lit(int curr_led, int dir, CRGB colour){
  delay(5);
  FastLED.show();
  if (curr_led == NUM_LEDS - 1 && dir == 1){
    leds[curr_led] = CRGB(0,0,0);  
    return recursion_lit(curr_led - 1, 0, colour);
  }
  else if (curr_led == 0 && dir == 0){
    leds[curr_led] = colour;
    return recursion_lit(curr_led+1, 1, colour);
  }
  else if (dir == 1){
    leds[curr_led] = colour;
    return recursion_lit(curr_led+1,1, colour);
  }
  else if (dir == 0){
    leds[curr_led] = CRGB(0, 0, 0);
    return recursion_lit(curr_led-1, 0, colour);
  }
}

CHSV Rotate(int pos) {
  CHSV colour (pos, 255, 255);
  return colour;
}


void rainbow(){
    for(int j = 0; j < 256; j++) {
      for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = Rotate((i * 256 / NUM_LEDS + j) % 256);   // (5*i + j) % 256 works too   
      }
    FastLED.show();
    delay(1);
    }
}

void graphFrequencies() {
  Serial.print( (left[0] + right[0]) / 2 );
  Serial.print("    ");
  Serial.print( (left[2] + right[2]) / 2 );
  Serial.print("    ");
  Serial.print( (left[4] + right[4]) / 2 );
  Serial.print("    ");
  Serial.print( (left[6] + right[6]) / 2 );
  Serial.print("    ");
  Serial.println();
  delay(20);
}
