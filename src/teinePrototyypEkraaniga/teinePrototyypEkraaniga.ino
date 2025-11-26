#include <Servo.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>

// MÕÕTMISEKS VAJALIKUD KONSTANDID JA MÕÕTMISPROTSESS
#define SIGNAL_PIN 5
#define MOOTMISTE_ARV 10   // number of samples per position

const int sensorPin = A0;
const long referenceMv = 5000;

long pikkus;
float kaugused[MOOTMISTE_ARV];
float kaugus;

Servo mootor;

//interpolation of distance at 250mV intervals
const int TABLE_ENTRIES = 12;
const int INTERVAL  = 250;
static int distance[TABLE_ENTRIES] = {150,140,130,90,60,50,40,35,30,25,20,15};

int getDistance(int mV) {
  if (mV > INTERVAL * TABLE_ENTRIES - 1)      return distance[TABLE_ENTRIES - 1];
  else {
    int index = mV / INTERVAL;
    float frac = (mV % 250) / (float)INTERVAL;
    return distance[index] - ((distance[index] - distance[index + 1]) * frac);
  }
}

// --- median filter ---
float medianFilter(float arr[], int n) {
  // simple bubble sort (ok for n=10)
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        float temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
  if (n % 2 == 0)
    return (arr[n/2 - 1] + arr[n/2]) / 2.0;
  else
    return arr[n/2];
}


// --- measure with filtering at angle i ---
int mooda(int nurk) {
  mootor.write(nurk);
  delay(50); // give servo time to move

  int valid = 0;
  for (int n = 0; n < MOOTMISTE_ARV; n++) {
    int val = analogRead(sensorPin);
    int mV = (val * referenceMv) / 1023;
    int cm = getDistance(mV);
    if (cm >= 15 && cm <= 130) {
      kaugused[valid++] = cm;
    }
    delay(50);
  }

  // --- Send clean serial output for dashboard ---
  if (valid > 2) {
    float med = medianFilter(kaugused, valid);
    // Optional: compute trimmed mean for extra smoothing
    float sum = 0;
    int count = 0;
    for (int j = 0; j < valid; j++) {
      if (abs(kaugused[j] - med) < 0.2 * med) { // keep within ±20% of median
        sum += kaugused[j];
        count++;
      }
    }
    float avg = (count > 0) ? sum / count : med;

    return avg;
  } else {
    return 0;
  }
}

//JOONISTAMISEKS VAJALIKUD KONSTANDID JA FUNKTSIOONID
//Joonistamise ettevalmistamine
#define TFT_CS   8
#define TFT_RST  9
#define TFT_DC  10

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

//Mõõtmed
const int16_t W = 320;
const int16_t H = 240;
const int16_t CX = W / 2;
const int16_t CY = H - 10;        // bottom, with small margin
const int16_t RADAR_R = 150;      // radius in pixels
int16_t eelmine_nurk = 10;

// Colors
const uint16_t COL_BG    = ILI9341_BLACK;
const uint16_t COL_GRID  = ILI9341_DARKGREEN;
const uint16_t COL_SWEEP = ILI9341_GREEN;
const uint16_t COL_POINT = ILI9341_RED;

//punktide salvestamine
int points[32];

void drawGrid(){
  tft.fillScreen(COL_BG);
  for(int i=50; i<=RADAR_R; i+=50){
    tft.drawCircle(CX, CY, i, COL_GRID);
  }
  for (int a = 10; a <= 170; a += 20) {
    float rad = radians(a);
    int16_t x = CX + (int16_t)(RADAR_R * cos(rad));
    int16_t y = CY - (int16_t)(RADAR_R * sin(rad));
    tft.drawLine(CX, CY, x, y, COL_GRID);
  }
}

void drawSweep(int nurk){
  //vana sweep linei kustutamine
  float rad_eelmine = radians(eelmine_nurk);
  int16_t x_eelmine = CX + (int16_t)(RADAR_R * cos(rad_eelmine));
  int16_t y_eelmine = CY - (int16_t)(RADAR_R * sin(rad_eelmine));  
  tft.drawLine(CX, CY, x_eelmine, y_eelmine, COL_BG);

  deletePoint(nurk);

  //gridiga kattumise kohtadel redraw grid
  if((eelmine_nurk-10) % 20 == 0){
    //joonistame uuesti
    tft.drawLine(CX, CY, x_eelmine, y_eelmine, COL_GRID);
  }
  //ringidega ristumise pikslite uuesti joonistamine
  for(int i=50; i<=RADAR_R; i+=50){
    tft.drawCircle(CX, CY, i, COL_GRID);
  }
  // nurk 0..180, 0° = right, 90° = straight up, 180° = left
  float rad = radians(nurk);
  eelmine_nurk = nurk;
  int16_t x = CX + (int16_t)(RADAR_R * cos(rad));
  int16_t y = CY - (int16_t)(RADAR_R * sin(rad));
  tft.drawLine(CX, CY, x, y, COL_SWEEP);
  
  drawPoint(nurk);
}
//punkti kustutamine sweeperi hetke nurga pealt.
void deletePoint(int nurk){
  float rad = radians(nurk);
  int16_t x = CX + (int16_t)(points[(nurk-10)/5] * cos(rad));
  int16_t y = CY - (int16_t)(points[(nurk-10)/5] * sin(rad));
  tft.drawPixel(x, y, COL_BG);
}
//punktide joonistamine vastavalt mõõtmistulemustele
void drawPoint(int nurk){
  int kaugus = mooda(nurk);
  float rad = radians(nurk);
  int16_t x = CX + (int16_t)(kaugus * cos(rad));
  int16_t y = CY - (int16_t)(kaugus * sin(rad));
  tft.drawPixel(x, y, COL_POINT);
  points[(nurk-10)/5] = kaugus;
}

//SETUP JA VOID
void setup() {
  pinMode(SIGNAL_PIN, OUTPUT);
  mootor.attach(SIGNAL_PIN);
  Serial.begin(9600);

  tft.begin();
  tft.setRotation(1);

  drawGrid();
}

void loop() {
  // Sweep right
  for (int i = 10; i <= 170; i += 5) {
    drawSweep(i);
  }
  // Sweep left
  for (int i = 170; i >= 10; i -= 5) {
    drawSweep(i);
  }
}