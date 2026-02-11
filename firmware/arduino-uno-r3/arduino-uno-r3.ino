/******************************************************************************
 * @file:        arduino-uno-r3.uno
 * @description: Core application logic.
 * @author:      John LeClair (jleclair@condorembeddedtech.com)
 * @date:        2026-02-11

 * @notes:       I wrote Joint Strike Fighter (JSF)-compliant code for many
                 years. Thus my code block braces are in Allman style even 
                 for one line inside the block. I also despise 80 character 
                 line limits.

 ******************************************************************************/
#include <TimerOne.h>    // Timer Interrupt Handling

// Debug Setting.
const bool enableDebug = true;  // Set to true to view raw ADC potentiometer readings. 

// Vacuum Pump Delay
const long defaultPumpShutoffDelay = 12000000;   // 12 seconds before pump shutoff  

// GPIO Inputs
const int potAdcPumpSpeedPin = A3;  // Potentiometer output connected to analog pin 3
const int potAdcDelayPin = A4;
const int switchFootSwitchPin = A1; // Status of foot switch 

// GPIO Outputs
const int mosfetPwmPumpPin = A0;        // Pump MOSFET driver set to analog pin 0
const int mosfetEnableVacuumValve = A2; // Enable/Disable Vacuum Valve.

int potPumpSpeedValue = 0;          // ADC Potentiometers reading 
int potDelayValue = 0;
int footswitchStatus = 0;

// Timer1 ISR:  Vacuum pump shutoff delay timer
void ISR_Vacuum_Pump_Shutoff_Delay(void)
{
  // shutoff vacuum pump
  // TODO: or set a flag to set mosfet pin to zero in main loop()
  //       and make this a very fast upper half handler

  if (enableDebug)
  {
    Serial.println("Shutting off Pump"); 
  }
}

// the setup function runs once when you press reset or power the board
void setup()
{
  // Input Pin Configuration
  pinMode(potAdcPumpSpeedPin, INPUT);
  pinMode(potAdcDelayPin, INPUT);  
  pinMode(switchFootSwitchPin, INPUT);

  // Output Pin Configuration
  pinMode(mosfetPwmPumpPin, OUTPUT);

  // Timer Shutoff Configuration
  Timer1.initialize(defaultPumpShutoffDelay); // Set timer to 1,000,000 microseconds (1 second)
  Timer1.attachInterrupt(ISR_Vacuum_Pump_Shutoff_Delay); // Attach the ISR function

  if (enableDebug)
  {
    Serial.begin(9600);
  }
}

// the loop function runs over and over again forever
void loop() 
{
  potPumpSpeedValue = analogRead(potAdcPumpSpeedPin);
  delayMicroseconds(100);
  
  potDelayValue = analogRead(potAdcDelayPin);

  // Adjust PWM MOSFET pin accordingly.
  analogWrite(mosfetPwmPumpPin, potDelayValue >> 2); // (scaled from 1024 values to 256 values)

  // TODO: Adjust Timer value based on ADC delay setting.

  if (enableDebug)
  {
    Serial.print("potPumpSpeedValue: ");
    Serial.println(potPumpSpeedValue); 

    Serial.print("potDelayValue: ");
    Serial.println(potDelayValue);
    delay(2000);
  }

  delay(2000);
}
