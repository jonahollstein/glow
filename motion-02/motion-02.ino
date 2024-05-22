#include <FastLED.h>

#define LED_PIN  5

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

// Params for width and height
const uint8_t matrixX = 30;
const uint8_t matrixY = 10;

const bool    kMatrixSerpentineLayout = true;
const bool    kMatrixVertical = false;

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * matrixX) + x;
    } else {
      i = matrixY * (matrixX - (x+1))+y;
    }
  }

  if( kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if( y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (matrixX - 1) - x;
        i = (y * matrixX) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * matrixX) + x;
      }
    } else { // vertical positioning
      if ( x & 0x01) {
        i = matrixY * (matrixX - (x+1))+y;
      } else {
        i = matrixY * (matrixX - x) - (y+1);
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
  if( x >= matrixX) return -1;
  if( y >= matrixY) return -1;
  return XY(x,y);
}


class randomPoint {
  public:
    float xpos = 0.f;
    float ypos = 0.f;
    float vel = 1.f;
    int dir = 0;

    void moveSine(){
      xpos += 1;
      ypos = sin(xpos * )
    }
}

float calcProximity(float ledX, float ledY, float pointX, float pointY) { 
    return prox = sqrt(pow(ledX - pointX, 2) + pow(ledY - pointY, 2));
}

void draw( ledX) {
  float proximity = calcProximity(float)
  if (ledX >= 0 && ledX < matrixX && ledY >= 0 && ledY < matrixY) {
    grid[ledX][ledY] += 1 - proximity;
  }
}



void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);

}

void loop() {


  
  
  leds[ XY(posX, i)]  = CRGB::Red;
  FastLED.show();
  delay(1);

}
