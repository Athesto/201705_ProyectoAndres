// Sample Arduino MAX6675 Arduino Sketch

#include "max6675.h"

int ktcCLK = 10;
int ktcCS = 9;
int ktcSO = 8;



MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

  
void setup() {
  Serial.begin(9600);
  // give the MAX a little time to settle
  delay(500);
}

void loop() {
  // basic readout test
  
   Serial.print("Deg C = "); 
   Serial.println(ktc.readCelsius());

 
   delay(500);
}
