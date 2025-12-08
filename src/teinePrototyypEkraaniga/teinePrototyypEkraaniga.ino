//Vajalike teekide kasutusele võtt

#include <Servo.h> //motori teek
#include <Adafruit_ILI9341.h> //Ekraanile joonistamiseks vajalik teek #1
#include <Adafruit_GFX.h> //Ekraanile joonistamiseks vajalik teek #2

// MÕÕTMISEKS VAJALIKUD KONSTANDID JA MÕÕTMISPROTSESS
#define SIGNAL_PIN 5
#define SENSOR_PIN A0

const int MEASUREMENT_COUNT 10;   // igal positsioonil
const long referenceMv = 5000;

float distance_arr[MEASUREMENT_COUNT];
float distance;

Servo motor;

//250mV intervallide järgi kauguse interpolatsioon
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

// --- mediaan filter ---
float medianFilter(float arr[], int n) {
  // mõõdetud kaugused sorteeritakse minimaalsest maksimaalseni.
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


// mõõdab ja filtreerib kaugused nurgal i
int measure(int angle) {
  motor.write(angle);
  delay(50); // annab mootorile aega liikuda õigesse asendisse enne mõõtmist.

  int valid = 0;
  for (int n = 0; n < MEASUREMENT_COUNT; n++) {
    int val = analogRead(SENSOR_PIN);
    int mV = (val * referenceMv) / 1023;
    int cm = getDistance(mV);
    if (cm >= 15 && cm <= 130) {
      distance_arr[valid++] = cm;
    }
    delay(50);
  }

  if (valid > 2) {
    float med = medianFilter(distance_arr, valid);
    float sum = 0;
    int count = 0;
    for (int j = 0; j < valid; j++) {
      if (abs(distance_arr[j] - med) < 0.2 * med) { //erineb mediaanist maksimaalselt +/-20%
        sum += distance_arr[j];
        count++;
      }
    }
    float avg = (count > 0) ? sum / count : med;

    return avg;
  } else { // tagastab 0 kui ei saadud vähemlt kolme korrektset mõõtmistulemust
    return 0;
  }
}

//JOONISTAMISEKS VAJALIKUD KONSTANDID JA FUNKTSIOONID
//Joonistamise ettevalmistamine
#define TFT_CS   8
#define TFT_RST  9
#define TFT_DC  10

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

//ekraani mõõtmed
const int16_t W = 320;
const int16_t H = 240;
const int16_t CX = W / 2;
const int16_t CY = H - 10;        // Alumisest servast 10 pikslit eemal
const int16_t RADAR_R = 150;      // raadius pikslites
int16_t prev_angle = 10;

// joonistamisel kasutatavad värvid
const uint16_t COL_BG    = ILI9341_BLACK;
const uint16_t COL_GRID  = ILI9341_DARKGREEN;
const uint16_t COL_SWEEP = ILI9341_GREEN;
const uint16_t COL_POINT = ILI9341_RED;

//punktide salvestamine ennikusse
int points[32];

//Radari põhja/aluse joonistamine
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
//Radar liikumist illustreeriva joone joonistamine ja liigutamine
void drawSweep(int angle){
  //vana kustutamine
  float rad_eelmine = radians(prev_angle);
  int16_t x_eelmine = CX + (int16_t)(RADAR_R * cos(rad_eelmine));
  int16_t y_eelmine = CY - (int16_t)(RADAR_R * sin(rad_eelmine));  
  tft.drawLine(CX, CY, x_eelmine, y_eelmine, COL_BG);

  deletePoint(angle);

  //Radari aluse/põhja joontega kattumisel üle joonistamine.
  if((prev_angle-10) % 20 == 0){
    //joonistame uuesti
    tft.drawLine(CX, CY, x_eelmine, y_eelmine, COL_GRID);
  }
  //ringidega ristumise pikslite uuesti joonistamine
  for(int i=50; i<=RADAR_R; i+=50){
    tft.drawCircle(CX, CY, i, COL_GRID);
  }
  float rad = radians(angle);
  prev_angle = angle;
  int16_t x = CX + (int16_t)(RADAR_R * cos(rad));
  int16_t y = CY - (int16_t)(RADAR_R * sin(rad));
  tft.drawLine(CX, CY, x, y, COL_SWEEP);
  
  drawPoint(angle);
}
//punkti kustutamine sweeperi hetke nurga pealt.
void deletePoint(int angle){
  float rad = radians(angle);
  int16_t x = CX + (int16_t)(points[(angle-10)/5] * cos(rad));
  int16_t y = CY - (int16_t)(points[(angle-10)/5] * sin(rad));
  tft.fillCircle(x, y, 3, COL_BG);
}
//punktide joonistamine vastavalt mõõtmistulemustele
void drawPoint(int angle){
  int distance = measure(angle);
  float rad = radians(angle);
  int16_t x = CX + (int16_t)(distance * cos(rad));
  int16_t y = CY - (int16_t)(distance * sin(rad));
  tft.fillCircle(x, y, 3, COL_POINT);
  points[(angle-10)/5] = distance;
}

//SETUP JA VOID
void setup() {
  pinMode(SIGNAL_PIN, OUTPUT);
  motor.attach(SIGNAL_PIN);
  Serial.begin(9600);

  tft.begin();
  tft.setRotation(1);

  drawGrid();
}

void loop() {
  // liikumine paremale
  for (int i = 10; i <= 170; i += 5) {
    drawSweep(i);
  }
  // liikumine vasakule
  for (int i = 170; i >= 10; i -= 5) {
    drawSweep(i);
  }
}