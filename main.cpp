#include <Arduino.h>
#include <avr/wdt.h>
// put function declarations here:
int8_t ledpin = 12;
int8_t buttonpin = 2;
int8_t buzzerpin = 5;

enum State {
  STATE_IDLE,
  STATE_DEBOUNCE_PRESSED,
  STATE_PRESSED,
  STATE_LONG_PRESSED,
  STATE_REPEATED,
  STATE_DEBOUNCE_RELEASED
};State current_state = STATE_IDLE;

unsigned long press_start_time = 0;
unsigned long repeat_start_time = 0;
unsigned long button_time = 0;
unsigned long debounce_time = 20;

//int buttonstate = digitalRead(buttonpin);


    

void setup() {
  cli();                     // Disable global interrupts
  wdt_reset();               // Reset watchdog
  sei(); 
  MCUSR = 0;
  wdt_disable(); 
  wdt_enable(WDTO_4S);       // Enable watchdog with 4 second timeout

  
  pinMode(ledpin, OUTPUT);
  digitalWrite(ledpin, LOW);               // safe defalt state at boot
  pinMode(buttonpin, INPUT_PULLUP);        // safe defalt state at boot
  pinMode(buzzerpin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  wdt_reset(); 
  switch(current_state){
    case STATE_IDLE:
    if (digitalRead(buttonpin) == LOW){
    button_time = millis();
    current_state = STATE_DEBOUNCE_PRESSED;

    }
    break;
    case STATE_DEBOUNCE_PRESSED:    
    if(millis() - button_time > debounce_time){
      if(digitalRead(buttonpin)== LOW){
        digitalWrite(ledpin, HIGH);
        press_start_time = millis();
        current_state = STATE_PRESSED;
      }else{
        digitalWrite(ledpin, LOW);
        current_state = STATE_IDLE;
      }
    }
      break;
    
  case STATE_PRESSED:
  if(digitalRead(buttonpin) == LOW){
    if(millis() - press_start_time > 1000){
      digitalWrite(ledpin, HIGH);
    current_state = STATE_LONG_PRESSED;
    }
    // digitalWrite(ledpin, HIGH);
    //millis();
  }else{
       digitalWrite(ledpin, !digitalRead(ledpin));
      //button_time = millis();
      //press_start_time = millis();
      current_state = STATE_DEBOUNCE_RELEASED;
  }
  break;

  case STATE_LONG_PRESSED:
  if(digitalRead(buttonpin) == LOW){
    if(millis() - press_start_time > 1000){
      
      Serial.println("millis");
      if(millis() - repeat_start_time > 500){
    repeat_start_time = millis();
    digitalWrite(ledpin, !digitalRead(ledpin));
    }
    if(millis() - press_start_time > 3000 ){
      current_state = STATE_REPEATED;
    }
    

    }
    //
  }else{
    digitalWrite(ledpin, LOW);
    digitalWrite(buzzerpin, LOW);

    current_state = STATE_DEBOUNCE_RELEASED;     
  }
  
  break;
  case STATE_REPEATED:
  
  digitalWrite(ledpin, LOW);
  if(digitalRead(buttonpin) == LOW){
    if(millis() - repeat_start_time > 3000){
      repeat_start_time= millis();
    digitalWrite(ledpin, HIGH);
    if(millis() - button_time > 500){
    button_time = millis();
    digitalWrite(buzzerpin, LOW);
    digitalWrite(buzzerpin, HIGH);
    current_state = STATE_DEBOUNCE_RELEASED;
    }
  } 
  break;
  case STATE_DEBOUNCE_RELEASED:
  if (digitalRead(buttonpin)== HIGH){
    button_time = millis();
    digitalWrite(ledpin,LOW);
    digitalWrite(buzzerpin, LOW);
    current_state = STATE_IDLE;
  }
break;
    }
  }  
}