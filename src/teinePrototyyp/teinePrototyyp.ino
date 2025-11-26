#include <Servo.h>

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

void setup() {
  pinMode(SIGNAL_PIN, OUTPUT);
  mootor.attach(SIGNAL_PIN);
  Serial.begin(9600);
}

// --- measure with filtering at angle i ---
void mooda(int nurk) {
  mootor.write(nurk);
  delay(100); // give servo time to move

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

    // --- Send clean serial output for dashboard ---
    Serial.print(nurk);
    Serial.print(",");
    Serial.println(avg);
  } else {
    Serial.print(nurk);
    Serial.print(",");
    Serial.println(0);
  }
}

void loop() {
  // Sweep right
  for (int i = 10; i <= 170; i += 5) {
    mooda(i);
  }
  // Sweep left
  for (int i = 170; i >= 10; i -= 5) {
    mooda(i);
  }
}