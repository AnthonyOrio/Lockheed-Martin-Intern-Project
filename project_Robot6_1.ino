#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeMegaPi.h>


//Encoder Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeEncoderOnBoard Encoder_3(SLOT3);
MeEncoderOnBoard Encoder_4(SLOT4);

MeBluetooth bluetooth(PORT_5);
MeGyro gyro(PORT_8);

void isr_process_encoder1(void)
{
      if(digitalRead(Encoder_1.getPortB()) == 0){
            Encoder_1.pulsePosMinus();
      }else{
            Encoder_1.pulsePosPlus();
      }
}

void isr_process_encoder2(void)
{
      if(digitalRead(Encoder_2.getPortB()) == 0){
            Encoder_2.pulsePosMinus();
      }else{
            Encoder_2.pulsePosPlus();
      }
}

void isr_process_encoder3(void)
{
      if(digitalRead(Encoder_3.getPortB()) == 0){
            Encoder_3.pulsePosMinus();
      }else{
            Encoder_3.pulsePosPlus();
      }
}

void isr_process_encoder4(void)
{
      if(digitalRead(Encoder_4.getPortB()) == 0){
            Encoder_4.pulsePosMinus();
      }else{
            Encoder_4.pulsePosPlus();
      }
}

void move(int direction, int speed)
{
      int leftSpeed = 0;
      int rightSpeed = 0;
      if(direction == 1){
            leftSpeed = -speed;
            rightSpeed = speed;
      }else if(direction == 2){
            leftSpeed = speed;
            rightSpeed = -speed;
      }else if(direction == 3){
            leftSpeed = speed;
            rightSpeed = speed;
      }else if(direction == 4){
            leftSpeed = -speed;
            rightSpeed = -speed;
      }
      Encoder_1.setTarPWM(rightSpeed);
      Encoder_2.setTarPWM(leftSpeed);
}
void moveDegrees(int direction,long degrees, int speed_temp)
{
      speed_temp = abs(speed_temp);
      if(direction == 1)
      {
            Encoder_1.move(degrees,(float)speed_temp);
            Encoder_2.move(-degrees,(float)speed_temp);
      }
      else if(direction == 2)
      {
            Encoder_1.move(-degrees,(float)speed_temp);
            Encoder_2.move(degrees,(float)speed_temp);
      }
      else if(direction == 3)
      {
            Encoder_1.move(degrees,(float)speed_temp);
            Encoder_2.move(degrees,(float)speed_temp);
      }
      else if(direction == 4)
      {
            Encoder_1.move(-degrees,(float)speed_temp);
            Encoder_2.move(-degrees,(float)speed_temp);
      }
    
}

double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;
MeLineFollower linefollower_6(6);
MeUltrasonicSensor ultrasonic_7(7);
String bluetoothStr;
double distPerRev, distTotal;
double zAngle;
unsigned long timeRun;

void setup(){
    //Set Pwm 8KHz
    TCCR1A = _BV(WGM10);
    TCCR1B = _BV(CS11) | _BV(WGM12);
    TCCR2A = _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS21);
    
    attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
    Encoder_1.setPulse(8);
    Encoder_1.setRatio(46.67);
    Encoder_1.setPosPid(1.8,0,1.2);
    Encoder_1.setSpeedPid(0.18,0,0);
    attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
    Encoder_2.setPulse(8);
    Encoder_2.setRatio(46.67);
    Encoder_2.setPosPid(1.8,0,1.2);
    Encoder_2.setSpeedPid(0.18,0,0);

    Serial.begin(9600);
    bluetooth.begin(9600);
}

void loop(){
    char inData;

    /* Calculate running time. */
    timeRun = millis() * 1000;

    /* Determine drive direction. */
    if(((linefollower_6.readSensors())==(0))){
        Encoder_1.runSpeed(180);
        Encoder_2.runSpeed(160);
        distPerRev = ((2.0 * PI) * 160);
    }else{
        if(((linefollower_6.readSensors())==( 3 ))){
            Encoder_1.runSpeed(-70);
            Encoder_2.runSpeed(-50);
        }else{
            if(((linefollower_6.readSensors())==(2 ))){
                Encoder_1.runSpeed(-70);
                Encoder_2.runSpeed(50);
            }else{
                Encoder_1.runSpeed(70);
                Encoder_2.runSpeed(-50);
            }
        }
         distPerRev = ((2.0 * PI) * 50);
    }

    /* Calculate the distance fround from the revolutions. */
    /* This gives us the distance in inches. 2.0 is the wheel size. */
    distTotal = distPerRev * timeRun;
    bluetoothStr = String(distTotal + ',');

    /* Get the Z Angle and append it to the BlueTooth string. */
    zAngle = gyro.getAngleZ();
    bluetoothStr.concat(String(zAngle));

    /* If robot is 6 inches from wall make a turn. */
    if((ultrasonic_7.distanceCm()) < (15.24)){
        Encoder_1.runSpeed(70);
        Encoder_2.runSpeed(-50);
    }

    /* Calculates the distance from the wall */
    long distFromWall = ultrasonic_7.measure();
    /* Assembles the string for BlueTooth */
    bluetoothStr.concat(String(distFromWall + "\0"));

    /* This might be incorrect. We need to figure out how to send a string */
    if (bluetooth.available())
    {
      bluetooth.print(bluetoothStr);
    }
    
    _loop();
}

void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime)_loop();
}

void _loop(){
    Encoder_1.loop();
    
    Encoder_2.loop();
    
    
}
