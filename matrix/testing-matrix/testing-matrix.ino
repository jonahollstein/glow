#include <FastLED.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define LED_PIN  5

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

// Params for width and height
const uint8_t kMatrixWidth = 30;
const uint8_t kMatrixHeight = 10;

const bool    kMatrixSerpentineLayout = true;
const bool    kMatrixVertical = false;

// Set these to your desired credentials.
const char *ssid = "glow-starling-01";
const char *password = "starling";


//calculate LED ID in serpentine matrix
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * kMatrixWidth) + x;
    } else {
      i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
    }
  }

  if( kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if( y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (kMatrixWidth - 1) - x;
        i = (y * kMatrixWidth) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * kMatrixWidth) + x;
      }
    } else { // vertical positioning
      if ( x & 0x01) {
        i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
      } else {
        i = kMatrixHeight * (kMatrixWidth - x) - (y+1);
      }
    }
  }
  
  return i;
}

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}

int posX = 0;
int posY = 0;

void setup() {
  Serial.begin();
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
}

void loop() {
  for(int x = 0; x < kMatrixWidth; x++){
    for(int y = 0; y < kMatrixHeight; y++){
      leds[ XY(x, y)]  = CRGB::Black;
    }
  }
  
  leds[ XY(posX, posY)]  = CRGB::Red;
  FastLED.show();
  delay(100);
  posY = (sin(posX)*10);
  posX ++;


}
