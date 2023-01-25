
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:

int redPin = 9;
int greenPin = 8;
int yellowPin = 10;
int wee = 10;
boolean flip = false;


int lightPink[3] = {178, 10, 10};
int red[3] = {255, 0, 0};
int orange[3] = {255, 25, 0};
int blank[3] = {0,0,0};

int ledArray[LED_COUNT][3];

// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pinMode(redPin, INPUT);
  pinMode(greenPin, INPUT);
  pinMode(yellowPin, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);

  Serial.print("\non");

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(100); // Set BRIGHTNESS to about 1/5 (max = 255)

  setColor(blank);

}

void loop() {
  Serial.print("\nYellow ");
  Serial.print(digitalRead(yellowPin));
  Serial.print("\nRED ");
  Serial.print(digitalRead(redPin));
  Serial.print("\nGreen ");
  Serial.print(digitalRead(greenPin));


  //colorWipe(strip.Color(255,   0,   0), 50); // Red
  //colorWipe(strip.Color(  0, 255,   0), 50); // Green
  //colorWipe(strip.Color(  0,   0, 255), 50); // Blue

  if (!digitalRead(redPin)) {
    doubleColorWipe(lightPink, 50);
    doubleColorWipe(red, 10);
    doubleColorWipe(orange, 50);
  }
  else {
    randomColorWipe(strip.Color(0, 127, 128), wee);
    randomColorWipe(strip.Color(10, 10, 128), wee);
    randomColorWipe(strip.Color(40, 10, 120), wee);
  }



}

void colorWipe(int color[], int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    setColor(i, color);         //  Set pixel's color (in RAM)
    delay(wait);                           //  Pause for a moment
  }
}

void randomColorWipe(int color[], int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    setColor(random(0, 60), color);         //  Set pixel's color (in RAM)
    delay(wait);                           //  Pause for a moment
  }
}

void doubleColorWipe(int color[], int wait) {
  if (!flip) {
    for (int i = 0; i < strip.numPixels() / 2; i++) {
      setColor(i, color);
      setColor(strip.numPixels() - i - 1, color);
      delay(wait);
    }
    flip = true;
  } else {
    for (int i = 0; i < strip.numPixels() / 2; i++) {
      setColor(strip.numPixels() / 2 - i, color);
      setColor(strip.numPixels() / 2 + i, color);
      delay(wait);
    }
    flip = false;
  }
}

void bubble(int color[], int wait) {
  boolean done;
  boolean ledDone[LED_COUNT]; 
  for (int i = 0; i < LED_COUNT; i++) 
    ledDone[i] = false;

  do {
    for (int i = 0; i < LED_COUNT; i++) {
      if (random(2) == 0) ledDone[i] = !moveToColor(i, color);

      done = true;
      for (boolean led: ledDone) {
        if (!led) {
          done = false;
          break;  
        }
      }
      delay(wait);
    }
  }
  while (!done);
}

void funkyDoubleColorWipe(int color[], int wait) {
  if (!flip) {
    for (int i = 0; i < strip.numPixels() / 2; i++) {
      setColor(i, color + (i * 4));
      setColor(strip.numPixels() - i - 1, color + (i * 4));
      delay(wait);
    }
    flip = true;
  } else {
    for (int i = 0; i < strip.numPixels() / 2; i++) {
      setColor(strip.numPixels() / 2 - i, color + (i * 4));
      setColor(strip.numPixels() / 2 + i, color + (i * 4));
      delay(wait);
    }
    flip = false;
  }
  wee = random(40, 100);
}

void theaterChase(int color[], int wait) {
  for (int a = 0; a < 10; a++) { // Repeat 10 times...
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        setColor(c, color); // Set pixel 'c' to value 'color'
      }
      delay(wait);  // Pause for a moment
    }
  }
}








void setColor(int color[]) {
  int r = color[0], g = color[1], b = color[2];

  for (int i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();

  for (int index = 0; index < sizeof(ledArray); index++) {
    for (int i = 0; i < sizeof(ledArray[0]); ++i)
      ledArray[index][i] = color[i];
  }
}

void setColor(int index, int color[]) {
  int r = color[0], g = color[1], b = color[2];

  strip.setPixelColor(index, strip.Color(r, g, b));
  strip.show();
  for (int i = 0; i < sizeof(ledArray[0]); ++i)
    ledArray[index][i] = color[i];
}

void addColor(int index, int colorMod[]) {
  int r, g, b;

  ledArray[index][0] += colorMod[0];
  r = ledArray[index][0];
  ledArray[index][1] += colorMod[1];
  g = ledArray[index][1];
  ledArray[index][2] += colorMod[2];
  b = ledArray[index][2];

  strip.setPixelColor(index, strip.Color(r, g, b));
  strip.show();
}

boolean moveToColor(int wantedColor[]) {
  boolean changed = false;
  for (int i = 0; i < sizeof(ledArray); i++) {
    int r = ledArray[i][0], g = ledArray[i][1], b = ledArray[i][2];

    if (ledArray[i][0] < wantedColor[0]) {
      ledArray[i][0]++;
      r = ledArray[i][0];
      changed = true;
    }
    else if (ledArray[i][0] > wantedColor[0]) {
      ledArray[i][0]--;
      r = ledArray[i][0];
      changed = true;
    }

    if (ledArray[i][1] < wantedColor[1]) {
      ledArray[i][1]++;
      g = ledArray[i][1];
      changed = true;
    }
    else if (ledArray[i][1] > wantedColor[1]) {
      ledArray[i][1]--;
      g = ledArray[i][1];
      changed = true;
    }

    if (ledArray[i][2] < wantedColor[2]) {
      ledArray[i][2]++;
      b = ledArray[i][2];
      changed = true;
    }
    else if (ledArray[i][2] > wantedColor[2]) {
      ledArray[i][2]--;
      b = ledArray[i][2];
      changed = true;
    }

    strip.setPixelColor(i, strip.Color(r, g, b));
    strip.show();
  }
  return changed;
}

boolean moveToColor (int i, int wantedColor[]) {
  boolean changed = false;
  int r = ledArray[i][0], g = ledArray[i][1], b = ledArray[i][2];

  if (ledArray[i][0] < wantedColor[0]) {
    ledArray[i][0]++;
    r = ledArray[i][0];
    changed = true;
  }
  else if (ledArray[i][0] > wantedColor[0]) {
    ledArray[i][0]--;
    r = ledArray[i][0];
    changed = true;
  }

  if (ledArray[i][1] < wantedColor[1]) {
    ledArray[i][1]++;
    g = ledArray[i][1];
    changed = true;
  }
  else if (ledArray[i][1] > wantedColor[1]) {
    ledArray[i][1]--;
    g = ledArray[i][1];
    changed = true;
  }

  if (ledArray[i][2] < wantedColor[2]) {
    ledArray[i][2]++;
    b = ledArray[i][2];
    changed = true;
  }
  else if (ledArray[i][2] > wantedColor[2]) {
    ledArray[i][2]--;
    b = ledArray[i][2];
    changed = true;
  }

  strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();

  return changed;
}
