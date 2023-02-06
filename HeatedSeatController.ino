#include "./HeatedSeat.h"

HeatedSeat driver = HeatedSeat(10, 11, 9, 8, A0);
HeatedSeat passenger = HeatedSeat(12, 13, 7, 6, A1);
float driverTemp[5] = {0,0,0,0,0};
float passTemp[5] = {0,0,0,0,0};

void setup() {
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  delay(1500);
  Serial.begin(9600);

  // Fill up temperature array before starting main loop so average is accurate
  for (int i = 0; i < 4; i++) {
    driver.readTemperature();
    passenger.readTemperature();
    delay(20);
  }
}

void loop() {
  // Get temperature readings
  driver.readTemperature();
  passenger.readTemperature();

  // Run each loop
  driver.runLoop();
  passenger.runLoop();
  delay(20);
}
