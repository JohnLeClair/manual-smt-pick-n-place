/******************************************************************************
 * @file:        arduino-test-actuators.ino
 * @description: Arduino sketch to test ADC, PWM and GPIO.
 * @author:      John LeClair (jleclair@condorembeddedtech.com)
 * @date:        2026-02-11
 * @license:     none. Use how ever you want.
 *
 * @notes:       1) I wrote Joint Strike Fighter (JSF)-compliant code for many
                 years. Thus my code block braces are in Allman style even 
                 for one line inside the block. I also despise 80 character 
                 line limits. So I will maintain 120 character limit.

                 2) Need to install TimerOne library version 1.1.1

                 3) I put very little thought into the design of this software. 

 45******************************************************************************/

const int PWM_OFF = 0; // Turns off MOSFET powering pump motor.
const int PWM_ON_100_PERCENT = 255; 
const int PWM_ON = 255;

// GPIO Input Pins
const int potAdcPumpSpeedPin = A3;  // Analog. Potentiometer output connected to analog pin 3
const int potAdcDelayPin =     A4;  // Analog. Potentiometer out connected to analog pin 4
const int switchFootSwitchPin = 5;  // Digital. Status of foot switch 

// GPIO Output Pins
const int mosfetPwmPumpPin           = 3; // PWDM Pump MOSFET 1 driver set to analog pin 0
const int mosfetEnableVacuumValvePin = 4; // Digital MOSFET 2 Enable/Disable Vacuum Valve.

// Status variables
int potPumpSpeedValue = 0;          // ADC Potentiometer Pump Speed Reading
int potDelayValue = 0;              // ADC Potentioneter Pump Shutoff Delay Reading
int footswitchStatus = 0;     

// the setup function runs once when you press reset or power the board
void setup()
{
  // Input Pin Configuration
  pinMode(potAdcPumpSpeedPin, INPUT);
  pinMode(potAdcDelayPin, INPUT);  
  pinMode(switchFootSwitchPin, INPUT); // Switch no depressed, status is high.

  // Output Pin Configuration
  pinMode(mosfetPwmPumpPin, OUTPUT);
  pinMode(mosfetEnableVacuumValvePin, OUTPUT);

  // Starting up device to following states:
  //   - pump motor:  off
  //   - vacuum valve (normally closed): Flow OFF
  analogWrite(mosfetPwmPumpPin, PWM_OFF);
  digitalWrite(mosfetEnableVacuumValvePin, LOW); 

  Serial.begin(9600); 
}


// the loop function runs over and over again forever
// Nothing needs to react super fast - so a simple round robin plus a single ISR
// is just fine. 
void loop() 
{
  // Read ADC potentiometers
  potPumpSpeedValue = analogRead(potAdcPumpSpeedPin);
  delayMicroseconds(100); // give ADC time to read next channel.light
  potDelayValue = analogRead(potAdcDelayPin);

  // Check for depressed foot switch
  footswitchStatus = digitalRead(switchFootSwitchPin);

  analogWrite(mosfetPwmPumpPin, potPumpSpeedValue >> 2); // (scaled from 1024 values to 256 values)

  // We need vacuum flow enabled and pump on if not already in this state.
  // Adjust PWM MOSFET pin accordingly.
//  analogWrite(mosfetPwmPumpPin, potPumpSpeedValue >> 2); // (scaled from 1024 values to 256 values)

  Serial.print("potPumpSpeedValue: ");
  Serial.println(potPumpSpeedValue); 

  Serial.print("potDelayValue: ");
  Serial.println(potDelayValue);

  Serial.print("footswitchStatus: ");
  Serial.println(footswitchStatus);
  Serial.println("");

  if (potDelayValue > 800)
    digitalWrite(mosfetEnableVacuumValvePin, HIGH);
  else 
    digitalWrite(mosfetEnableVacuumValvePin, LOW);
}
