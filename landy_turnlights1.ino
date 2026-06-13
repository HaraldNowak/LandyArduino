#include <Servo.h>
int motorPWM = 0 ;
int lastmotorPWM = 0 ;
int lightPWM = 0 ;
int blinkerPWM = 0;
int blinker = 0 ;
int prepareWarn = 0 ;
int selftest = 0 ;

// Global Timing Variables for non-blocking operations
unsigned long lastBlinkChange = 0; // Time of the last blink state change (non-blocking)
int blinkInterval = 250;               // Interval for one LED HIGH/LOW phase (ms, defaults to a safe value)
bool blinkState = false ;

unsigned long lastBrakeChange=0;
unsigned long lastRevChange=0;
unsigned long breakMinDelay=1000;
unsigned long revMinDelay = 500;

int normallight=0;
int highperflight=0;
int rooflight=0;
int highperflightsharp=0;
int rooflightsharp=0;

int duration = 500 ;
double simPulseLen = 1200.0 ;
double dutyCycle ;


void setup()
{
  Serial.begin(9600);

  pinMode(13, OUTPUT); // roof light
  pinMode(8, OUTPUT);  // blinker right
  pinMode(12, OUTPUT); // blinker left

  pinMode(11, INPUT); // blinker input channel
  pinMode(9, INPUT);  // light input channel
  pinMode(6, INPUT);  // motor input channel

  pinMode(7, OUTPUT);  // reverse light
  pinMode(2, OUTPUT); // frontlight
  pinMode(4,OUTPUT);// bumper light
  
  // Calculate duty cycle from pulse len.
  // period = 2000ms
  dutyCycle = 32; //*simPulseLen/2040.0 ;
}

int selftestnum = -1 ;

void loop()
{
  lightPWM = pulseIn(9, HIGH);
  motorPWM = pulseIn(6, HIGH);
  blinkerPWM = pulseIn(11, HIGH);
  //Serial.print(dutyCycle);
  //Serial.print(" ");
  //Serial.println(motorPWM);

  if( lightPWM < 1000 ) {
    if( highperflightsharp == 1 ) {
      highperflight = 1 ;
    }
    if( rooflightsharp == 1 ) {
      rooflight = 1 ;
    }    
    normallight = 1 ;
  } else {
    if( normallight == 1 && highperflight == 0 ) {
      highperflightsharp = 1 ; 
    } else {
      if( normallight == 1 && highperflight == 1 && rooflight == 0 ) {
        rooflightsharp = 1 ; 
      } else {
        highperflight = 0 ;
        normallight = 0 ;
        rooflight = 0 ;
        highperflightsharp = 0 ;
        rooflightsharp = 0 ;
      }
    }
  }

  if( motorPWM < 1500 ) {
    digitalWrite(7, HIGH);
    lastRevChange = millis() ;
  } else {
    if( millis() - lastRevChange > revMinDelay ) {
      digitalWrite(7, LOW);
    }
  }
  if( motorPWM >= 1500 ) {
    if( motorPWM-lastmotorPWM < -10 ) {
      analogWrite( 10, 255 ); // brake light
      lastBrakeChange = millis() ;
    } else {
      if(millis()-lastBrakeChange > breakMinDelay) {
        if(normallight == 1 )
          analogWrite( 10, dutyCycle ); // brake lights off
        else
          analogWrite( 10, 0 );
      } 
    }
    lastmotorPWM = motorPWM ;
  } else {
      if(millis()-lastBrakeChange > breakMinDelay) {
        if(normallight == 1 )
          analogWrite( 10, dutyCycle ); // brake lights off
        else
          analogWrite( 10, 0 ); 
      }
  }
   
  // Directional Lights (Pins 2 & 4)
  if( normallight == 1 ) {
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW );
  }
  if( highperflight == 1 ) {
    digitalWrite(4, HIGH);
  } else {
    digitalWrite(4, LOW);
  }
  if( rooflight == 1 ) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
   
  // Blinker State Machine (This remains blocking for state updates but is fast)
  if(blinkerPWM < 100 ) {
    if( selftestnum == -1 ) {
      selftestnum = 10 ;
    }
    if( selftestnum > 0 ) {
      blinker = 2 ;
      selftest = 1 ;
      blinkInterval = 250 ; // Adjust interval for test state
      selftestnum-- ;
    } else {
      blinker = 0 ;
      selftest = 0 ;
      analogWrite( 10, dutyCycle );
    }
  } else {
    selftestnum = -1 ;
    selftest = 0 ;
    if(blinkerPWM > 1650 ) {
      if( blinker == 0 || blinker == 1  ) {
        blinker = 1;
      } else {
        if( prepareWarn == 1 ) {
          blinker = 2;
        } else {
          prepareWarn = 1 ;
          blinker = 1 ;
        }
      }
    }
    else if( blinkerPWM < 1350 ) {
      if( blinker == 0 || blinker == -1 ) {
        blinker = -1 ;
      } else {
        if( prepareWarn == 1 ) {
          blinker = 2 ;
        } else {
          prepareWarn = 1 ;
          blinker = -1 ;
        }
      }
    }   
    else {
      blinker = 0 ;
      prepareWarn = 0 ;
    }
    blinkInterval = 400 ; // Adjust interval for normal operation
  }
  
  // NON-BLOCKING BLINKING OUTPUT (Replaces all delay calls below)

  //Serial.print(String("isBlinking:")+String(blinker)+String(">\n"));
  if (millis() - lastBlinkChange >= blinkInterval) {
      lastBlinkChange = millis();
      blinkState = !blinkState;
      //Serial.print("blink\n");
      
      // Right blinker toggle logic (Pin 8)
      if (blinker == 1 || blinker == 2) {
          digitalWrite(8, blinkState ? HIGH : LOW);
      }

      // Left blinker toggle logic (Pin 12)
      if (blinker == -1 || blinker == 2) {
          digitalWrite(12, blinkState ? HIGH : LOW);
      }
  }
  // Immediate off:
  if( blinker == 0 || blinker == -1 ) {
    digitalWrite(8, LOW);
  }
  if( blinker == 0 || blinker == 1 ) {
    digitalWrite(12, LOW);
  }
  delay(20);
  // All explicit delay() calls have been removed. The loop now runs purely state-driven.
}