#include <Ticker.h>
#define D1 5
#define D2 4
#define D3 0
#define D6 12
#define D5 14
#define BLUE_LED_PIN 2

#define DIRECTION_DRIVE_PIN D1
#define DIRECTION_REVERSE_PIN D6
#define DIRECTION_OUTA_PIN D2
#define DIRECTION_OUTB_PIN D3
#define PEDAL_PIN A0
#define PWM_OUT_Pin D5

#define PEDAL_THRESHHOLD 10
#define PEDAL_OFFSET 215
#define PEDAL_SCALE 4
#define MAX_PWM_VALUE 200

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
  int cur_direction, diff;
  cur_direction = !digitalRead(DIRECTION_DRIVE_PIN) + ((!digitalRead(DIRECTION_REVERSE_PIN))<<1);
  if(count>20){
    count =0;
    char s [80];
    sprintf (s, "PWM value %d, dir: %d %d %d\n", pwmValue, cur_direction,digitalRead(DIRECTION_DRIVE_PIN), digitalRead(DIRECTION_REVERSE_PIN));
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
  
  if(cur_direction != direction){
    direction = cur_direction;
    driveState = STOPPING;
  }
  switch(driveState){
    case IDLE:
      if (pedalValue > PEDAL_THRESHHOLD && direction>0 && direction<3) {
        driveState = DRIVING;
      } else {
        pwmValue = 0;
      }
    break;
    case DRIVING:
      
      if(pwmValue < pedalValue){
        diff = pedalValue - pwmValue;
        if(diff>50){
          pwmValue += 50;
        } else if(diff>10){
          pwmValue += 10;
        } else {
          pwmValue ++;
        }
      } else if(pwmValue > pedalValue) {
        diff = pwmValue - pedalValue;
        if(diff>50){
          pwmValue -= 50;
        } else if(diff>10){
          pwmValue -= 10;
        } else {
          pwmValue --;
        }
      } else if (pedalValue < PEDAL_THRESHHOLD) {
        driveState = IDLE;
        pwmValue = 0;
      }
    break;
    case STOPPING:
      if(pwmValue == 0){
        if( direction == 1){
          digitalWrite(DIRECTION_OUTA_PIN, 1);
          digitalWrite(DIRECTION_OUTB_PIN, 0);
        } else if(direction == 2){
          digitalWrite(DIRECTION_OUTA_PIN, 0);
          digitalWrite(DIRECTION_OUTB_PIN, 1);
        }
        if (pedalValue > PEDAL_THRESHHOLD && direction>0 && direction<3) {
          driveState = DRIVING;
        } else {
          driveState = IDLE;
        }
      }else{
        pwmValue --;
      }
    break;
  }
  
  analogWrite(PWM_OUT_Pin, _min(pwmValue,MAX_PWM_VALUE));
}

void setup() {
  pinMode(DIRECTION_DRIVE_PIN,INPUT_PULLUP);
  pinMode(DIRECTION_REVERSE_PIN,INPUT_PULLUP);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(DIRECTION_OUTA_PIN, OUTPUT);
  pinMode(DIRECTION_OUTB_PIN, OUTPUT);
  pinMode(PWM_OUT_Pin, OUTPUT);
  Serial.begin(74880); 
  Serial.print("Booted \n");
  update_analog.attach(0.05, update);  
}

// the loop function runs over and over again forever
void loop() {
  
}