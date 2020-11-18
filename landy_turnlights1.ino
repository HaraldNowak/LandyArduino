#include <Servo.h>
int motorPWM = 0 ;
int lastmotorPWM = 0 ;
int lightPWM = 0 ;
int blinkerPWM = 0;
int blinker = 0 ;
int prepareWarn = 0 ;
int selftest = 0 ;

int normallight=0;
int highperflight=0;
int highperflightsharp=0;

int duration = 500 ;
double simPulseLen = 1200.0 ;
double dutyCycle ;

void setup()
{
  Serial.begin(9600);

  pinMode(13, OUTPUT); // blinker
  pinMode(8, OUTPUT);
  pinMode(12, OUTPUT); // blinker
  
  pinMode(11, INPUT); // blinker input channel
  pinMode(9, INPUT);  // light input channel
  pinMode(6, INPUT);  // motor input channel

  pinMode(2, OUTPUT); // frontlight
  pinMode(4,OUTPUT);// bumper light
  // Calculate duty cycle from pulse len.
  // period = 2000ms
  dutyCycle = 32 ; //*simPulseLen/2040.0 ;
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
    normallight = 1 ;
  } else {
    if( normallight == 1 && highperflight == 0 ) {
      highperflightsharp = 1 ; 
    } else {
      highperflight = 0 ;
      normallight = 0 ;
      highperflightsharp = 0 ;
    }
  }

  if( motorPWM >= 1500 ) {
    if( motorPWM-lastmotorPWM < -10 ) {
      analogWrite( 10, 255 ); // brake light
    } else {
      if(normallight == 1 )
        analogWrite( 10, dutyCycle ); // brake lights off
      else
        analogWrite( 10, 0 ); 
    }
    lastmotorPWM = motorPWM ;
  } else {
      if(normallight == 1 )
        analogWrite( 10, dutyCycle ); // brake lights off
      else
        analogWrite( 10, 0 ); 
  }
   
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
   
  if(blinkerPWM < 100 ) {
    if( selftestnum == -1 ) {
      selftestnum = 10 ;
    }
    if( selftestnum > 0 ) {
      blinker = 2 ;
      selftest = 1 ;
      duration = 250 ;
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
      if( blinker == 0 || blinker == 1 ) {
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
    duration = 400 ;
  }
  
  if( blinker == 1 || blinker == 2 ) {
    digitalWrite(13, HIGH);
    digitalWrite(8, HIGH);
    if(selftest) {
      analogWrite( 10, dutyCycle );
    }
  }
  if( blinker == -1  || blinker == 2 ) {
    digitalWrite(12, HIGH);
  }  
  delay(duration); // Wait for 500 millisecond(s)
  if( blinker == 1 || blinker == 2 ) {
    digitalWrite(13, LOW);
    digitalWrite(8, LOW);
    if(selftest) {
      analogWrite( 10, 255 );
    }
  }
  if( blinker == -1  || blinker == 2 ) {
    digitalWrite(12, LOW);
  }
  if( blinker != 0 ) {  
    delay(duration); // Wait for 500 millisecond(s)
  }
}
