// Arduino EEPROM
#include <EEPROM.h>
// Custom code by Arduino that enables 
// the saving of long data
#include "EEPROMAnything.h"

// Our current calibration status
int Calibrating = -1;

// flag used to determine a read
// complete signal
boolean stringComplete = false;

// Index used to provide inc within 
// calibration (iteration count).
int calibCountOne = 0;

// Struct used to store our 
// settings on the EEPROM
struct config_t
{
  float minX;
  float minY;
  float maxX;
  float maxY;
  boolean Calibrated; 
  float cX;
  float cY;
}configuration;

void setup()
{  
  // Initialise the serial connection with the 
  // default BluetoothMate baudrate of 115200
  Serial.begin(115200); 
  // A quick status report
  Serial.print("Pause for BT bootup");  
  // Pause to allow the BT to bootup.
  delay(6000)  
  // Another quick status report
  Serial.print("Reading EEPROM");  
  // Read in data from the EEPROM
  EEPROM_readAnything(0, configuration);  
  // Pause to allow our data read to complete
  delay(500);  
  // Signal if we are calibrated or not
  Serial.print("Calibrated: ");
  Serial.print(configuration.Calibrated);
  Serial.println();
}

void loop()
{
  if (stringComplete) 
  {
    // Reset the system for further data   
    stringComplete = false;  
    
    // If we are already calibrated then
    // restart the process. Otherwise
    // continue calibration process.  
    if(configuration.Calibrated)
    { 
      // Set the calibrated status to false    
      configuration.Calibrated = false;
      Calibrating = 0;
    }
    else 
    { 
      // Inc the calibration to next step
      Calibrating = Calibrating + 1; 
    }
  }
  
  // If we are not calibrated then calibrate
  // otherwise perform the calibration routine
  if(configuration.Calibrated) { playbackLoop(); }
  else { calibrationLoop(); }
  
  // Add a pause to make things easier to read
  delay(100);
}

void playbackLoop()
{
  // Read in the joystick x  
  float x = analogRead(A0);
  // Read in the joystick y  
  float y = analogRead(A1);
  
  // Apply the x offset
  x -= abs(512-configuration.cX);
  // Apply the y offset
  y -= abs(512-configuration.cY);
  
  // Map the read in values to the calibrated
  // joystick range
  x = map(x, 0, 1023, -100.0, 100.0);
  y = map(y, 0, 1023, -100.0, 100.0);
  
  // Remap using the maximum and minimum x values
  if( x < 0) { x = map(x, configuration.minX, 0, -100, 0); }  
  if( x > 0) { x = map(x, 0, configuration.maxX, 0, 100); }
  
  // Remap using the maximum and minimum y values
  if( y < 0) { y = map(y, configuration.minY, 0, -100, 0); }  
  if( y > 0) { y = map(y, 0, configuration.maxY, 0, 100); }
  
  // Output the joystick data
  Serial.print("J,");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  // Send a line return
  Serial.println();
}

void serialEvent()
{
  // Read while we have data
  while (Serial.available()) 
  {
    // Read a character
    char inChar = Serial.read(); 
    // Check for termination character
    if (inChar == '\n' || inChar == '\r') 
    { stringComplete = true; }
  }
}
