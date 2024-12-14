#include <Ticker.h>
#define D1 5
#define D2 4
#define D3 0
#define D5 14
#define BLUE_LED_PIN 2

#define DIRECTION_IN_PIN D1
#define DIRECTION_OUTA_PIN D2
#define DIRECTION_OUTB_PIN D3
#define PEDAL_PIN A0
#define PWM_OUT_Pin D5

#define PEDAL_THRESHHOLD 15
#define PEDAL_OFFSET 200
#define PEDAL_SCALE 4

int pedalValue = 0;
int pwmValue = 0;
int direction = 0;
int A = 0;
int B = 0;
enum eDriveState {
  IDLE,
  DRIVING,
  STOPPING
};
eDriveState driveState = STOPPING;

Ticker update_analog;
int heartbeat_led = LOW;
int count = 0;

void update() {
  if(count>20){
    count =0;
    char s [80];
    sprintf (s, "PWM value %d, dir: %d\n", pwmValue, direction);
    Serial.print(s);
    heartbeat_led = !heartbeat_led;
    if(driveState == IDLE){

      digitalWrite(BLUE_LED_PIN, 1);  // Turn the LED on
    }else{
      digitalWrite(BLUE_LED_PIN, heartbeat_led);  // Toggle when driving
    }
  }else{
    count++;
  }
  
  pedalValue = analogRead(PEDAL_PIN);
  if(pedalValue<PEDAL_OFFSET){
    pedalValue = 0;
  } else {
    pedalValue = (pedalValue-PEDAL_OFFSET)/PEDAL_SCALE;
  }
  if(digitalRead(DIRECTION_IN_PIN) != direction){
    direction = !direction;
    driveState = STOPPING;
  }
  switch(driveState){
    case IDLE:
      if (pedalValue > PEDAL_THRESHHOLD) {
        driveState = DRIVING;
      } else {
        pwmValue = 0;
      }
    break;
    case DRIVING:
      if(pwmValue < pedalValue){
        pwmValue ++;
      } else if(pwmValue > pedalValue) {
        pwmValue --;
      } else if (pedalValue < PEDAL_THRESHHOLD) {
        driveState = IDLE;
        pwmValue = 0;
      }
    break;
    case STOPPING:
      if(pwmValue == 0){
        digitalWrite(DIRECTION_OUTA_PIN,direction);
        digitalWrite(DIRECTION_OUTB_PIN,!direction);
        if (pedalValue > PEDAL_THRESHHOLD) {
          driveState = DRIVING;
        } else {
          driveState = IDLE;
        }
      }else{
        pwmValue --;
      }
    break;
  }
  
  analogWrite(PWM_OUT_Pin, _min(pwmValue,255));
}

void setup() {
  pinMode(DIRECTION_IN_PIN,INPUT_PULLUP);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(DIRECTION_OUTA_PIN, OUTPUT);
  pinMode(DIRECTION_OUTB_PIN, OUTPUT);
  pinMode(PWM_OUT_Pin, OUTPUT);
  Serial.begin(74880); 
  Serial.print("Booted \n");
  update_analog.attach(0.01, update);  
}

// the loop function runs over and over again forever
void loop() {
  
}