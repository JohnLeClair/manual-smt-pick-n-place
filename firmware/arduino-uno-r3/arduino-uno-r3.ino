/******************************************************************************
 * @file:        arduino-uno-r3.ino
 * @description: Arduino sketch to run the vacuum pump device.
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

 ******************************************************************************/
#include <TimerOne.h>    // Timer Interrupt Handling

// Debug Setting.
// Being a good C++ citizen using const instead of conditionally compiled. The loop() function 
// can check this flag however times as necessary since this loop is many, many orders of magnitude away
// from anything real-time. 
const bool enableDebug = true;  // Set to true to view raw ADC potentiometer readings. 

const int PWM_OFF = 0; // Turns off MOSFET powering pump motor.

// Vacuum Pump Delay
const unsigned long defaultPumpShutoffDelay = 10000000;   // 10 seconds before pump shutoff  

// GPIO Input Pins
const int potAdcPumpSpeedPin = A3;  // Potentiometer output connected to analog pin 3
const int potAdcDelayPin = A4;
const int switchFootSwitchPin = 4;  // Digital Status of foot switch 

// GPIO Output Pins
const int mosfetPwmPumpPin = 3;        // PWDM Pump MOSFET driver set to analog pin 0
const int mosfetEnableVacuumValvePin = 2; // Digital Enable/Disable Vacuum Valve.

int potPumpSpeedValue = 0;          // ADC Potentiometer Pump Speed Reading
int potDelayValue = 0;              // ADC Potentioneter Pump Shutoff Delay Reading     
int footswitchStatus = 0;           // Foot Switch Status.
unsigned long pumpShutoffTimerDelay = defaultPumpShutoffDelay;  // delay calculated based on potentiometer position.

bool footswitchStateChangeDepressed = true;  // flag representing a footswitch state transition from on to off. Run 
                                             // it first loop() iteration.

bool initialPumpCutoffFlag = true;  // Flag to keep pump off until first footswitch pressing.                                             

// Timer1 ISR:  Vacuum pump shutoff delay timer
void ISR_Vacuum_Pump_Shutoff_Delay(void)
{
  // shutoff vacuum pump
  analogWrite(mosfetPwmPumpPin, PWM_OFF);

  // TODO: The ISR is more time critical (not really in this case :-) ) - remove after verifying timer works. 
  if (enableDebug)
  {
    Serial.println("*** Shutting off Pump ***"); 
  }
}


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

  // Timer Shutoff Configuration
  Timer1.initialize(defaultPumpShutoffDelay); // Set timer to 1,000,000 microseconds (1 second)
  Timer1.attachInterrupt(ISR_Vacuum_Pump_Shutoff_Delay); // Attach the ISR function

  // Starting up device to following states:
  //   - pump motor:  off
  //   - vacuum valve (normally closed): Flow OFF
  analogWrite(mosfetPwmPumpPin, PWM_OFF);
  digitalWrite(mosfetEnableVacuumValvePin, LOW); 

  if (enableDebug)
  {
    Serial.begin(9600);
  } 
}


// the loop function runs over and over again forever
// Nothing needs to react super fast - so a simple round robin plus a single ISR
// is just fine. 
void loop() 
{
  // Read ADC potentiometers
  potPumpSpeedValue = analogRead(potAdcPumpSpeedPin);
  delayMicroseconds(100); // give ADC time to read next channel.
  potDelayValue = analogRead(potAdcDelayPin);

  // Check for depressed foot switch
  footswitchStatus = digitalRead(switchFootSwitchPin);

  // TODO: REMOVE
  analogWrite(mosfetPwmPumpPin, potPumpSpeedValue >> 2); // (scaled from 1024 values to 256 values)

#if(0) // todo: Enable this when footswitch is hooked up.
  if (footswitchStatus == HIGH)
  {
    // We need vacuum flow enabled and pump on if not already in this state.
    // Adjust PWM MOSFET pin accordingly.
    analogWrite(mosfetPwmPumpPin, potPumpSpeedValue >> 2); // (scaled from 1024 values to 256 values)

    footswitchStateChangeDepressed = false;

    initialPumpCutoffFlag = false; // TODO: maybe share this int footswitchStateChangeDepressed logic. 

    // Disable timer because as long as footswitch is depressed, the pump will always be enabled.
    Timer1.stop();
  }
  else
  {
    // Footswitch just depressed ?
    if (footswitchStateChangeDepressed)
    {
      // TODO: probably should do some debouncing here
      // Restart the pump turn off timer.
      // convert period for now (TODO from 1024 ADC values, to number of seconds by just mod 100 and then convert to microseconds.
      // ie: 0 to 10 second delay range. Close enough for now.
      Timer1.setPeriod(((potDelayValue / 100) + 1) * 1000000); // adding 1 second for good measure. It will come back to haunt me.
      Timer1.start();

      footswitchStateChangeDepressed = false;
    }
  }
#endif

  if (enableDebug)
  {
    Serial.print("potPumpSpeedValue: ");
    Serial.println(potPumpSpeedValue); 

    Serial.print("potDelayValue: ");
    Serial.println(potDelayValue);

    Serial.print("footswitchStatus: ");
    Serial.println(footswitchStatus);

    Serial.println("");

    delay(2000);
  }
}
