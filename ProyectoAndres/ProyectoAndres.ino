#define rpm_sensor_pin 2
#define master_stop_pin 3
#define trigger_pin 4
#define echo_pin 5
#define motor_pin 6
#define corte_pin 7


#include "max6675.h"

int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 10;
int counter_moving = 0;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

int pasos = 0;
long waiting_time = 100;
long moving_time = 100000;
long timers_states[10] = {1, 2, 3};
int counter_states = 0;
int states[10] = {};
String print_buffer = "";
volatile double velocidad_RPM = 0;
int pwm_size = 1;
int temp = 0;
bool corte = false;
bool motor = true;
long sensor_distancia;
void distancia_begin(int trigger, int echo);
long distancia(int trigger, int echo);

void setup() {
  Serial.begin(9600);
  distancia_begin(trigger_pin, echo_pin); //trigger y echo

  //master stop
  pinMode(master_stop_pin, INPUT);
  digitalWrite(master_stop_pin, HIGH);
  attachInterrupt(digitalPinToInterrupt(master_stop_pin), master_stop_subrutine, FALLING);

  //Sensor de movimiento
  pinMode(rpm_sensor_pin, INPUT);
  digitalWrite(rpm_sensor_pin, HIGH);

  attachInterrupt(digitalPinToInterrupt(rpm_sensor_pin), RPM, RISING);

  //Motor
  pinMode(motor_pin, OUTPUT);

  //Corte
  pinMode(corte_pin, OUTPUT);

  

}

void loop() {
  delay(990);


  //sensado
  velocidad_RPM = (velocidad_RPM / 4);
  velocidad_RPM = velocidad_RPM * 60;
  sensor_distancia = max(30 - distancia(trigger_pin, echo_pin), 0);

  //acction

  if (millis() < waiting_time){
    
    corte = 0;
    motor = 0;
  }else{
    motor = 1;
    }
  if (millis() > (waiting_time + moving_time)){
    counter_moving = counter_moving +1;
    waiting_time = millis();
    motor = 0;
    }
  if (counter_moving >= 3){
    corte = 1;
      if (counter_moving >= 5) {
        corte = 0;
        motor = 0;
        counter_moving = 0;
        }
    }
  
  




  //OUTPUT
  digitalWrite(corte_pin, corte);
  digitalWrite(motor_pin, motor);
  temp = ktc.readCelsius();

  print_buffer = (String)
                 sensor_distancia + ',' +
                 (temp) % 300 + ',' +
                 velocidad_RPM + ',' +
                 corte + ',' +
                 "OK";

  Serial.print(print_buffer);
  print_buffer = "";
  velocidad_RPM = 0;
}


void distancia_begin (int trigger, int echo) {
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
}

long distancia(int trigger, int echo) {
  long d_calculada;
  int duracion;
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  duracion = pulseIn(echo, HIGH);
  d_calculada = (duracion / 2) / 29;
  if (d_calculada < 0) d_calculada = 0;
  if (d_calculada > 30) d_calculada = 30;
  return d_calculada;
}




void RPM() {
  delay(10);
  velocidad_RPM += 1;

}

void master_stop_subrutine() {
  detachInterrupt(digitalPinToInterrupt(rpm_sensor_pin));
  digitalWrite(corte_pin, 1);
  digitalWrite(motor_pin, 1);
  while (1) {
    
    Serial.print("STOP,0,0,0,0,OK");
    delay(1000);
  }
}
