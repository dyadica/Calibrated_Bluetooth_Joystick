void calibrationLoop()
{
  switch(Calibrating)
    {
      case 0: clearEEPROM(); break;
      case 1: calibOneLoop(); break;
      case 2: calibTwoLoop(); break;
      case 3: saveEEPROM(); break;
    }
}

void calibOneLoop()
{ 
  // Tell the user what they need to do. First
  // we need to log the centeral x/y values of
  // the stationary joystick.  
  Serial.println("Leave the stick centred");
  // Get the stationary values N (20) times and
  // then work out the average values.
  if(calibCountOne < 20)
  {
    // Get the x/y values
    configuration.cX = configuration.cX + analogRead(A0);
    configuration.cY = configuration.cY + analogRead(A1);
    // Update the iteration count
    calibCountOne = calibCountOne + 1;
  }
  else
  {
    configuration.cX = configuration.cX / 20; 
    configuration.cY = configuration.cY / 20;
    
    Calibrating = 2;
    calibCountOne = 0;
  }
}

void calibTwoLoop()
{
  Serial.println("Move the stick to the extremes");
  
  // Read in the joystick x  
  float x = analogRead(A0);
  // Read in the joystick y  
  float y = analogRead(A1);
  
  // Apply the x center offset
  x -= abs(512-configuration.cX);
  // Apply the y center offset
  y -= abs(512-configuration.cY);
  
  // map the read in values to the calibrated
  // joystick range
  x = map(x, 0, 1023, -100.0, 100.0);
  y = map(y, 0, 1023, -100.0, 100.0);
  
  configuration.minX = min(configuration.minX, x);
  configuration.minY = min(configuration.minY, y);
  
  configuration.maxX = max(configuration.maxX, x);
  configuration.maxY = max(configuration.maxY, y);
  
  Serial.print("MinX: ");
  Serial.print(configuration.minX);
  Serial.println();
  Serial.print("MaxX: ");
  Serial.print(configuration.maxX);
  Serial.println();
  Serial.print("MinY: ");
  Serial.print(configuration.minY);
  Serial.println();
  Serial.print("MaxY: ");
  Serial.print(configuration.maxY);
  Serial.println();
}

void clearEEPROM()
{
   // Set the configuration store calibration to false.
   configuration.Calibrated = false;      
   // Show some status
   Serial.println("Clearing calibration data!");      
   // write a 0 to all 512 bytes of the EEPROM
   for (int i = 0; i < 512; i++) { EEPROM.write(i, 0); }      
   // Show some status
   Serial.println("Calibration data cleared!");
   // Pause a little to make things easyer to read.
   delay(500);
   // Reset the center x and y values
   configuration.cX = 0;
   configuration.cY = 0;
   // Reset the maximum and minimum x/y
   // joystick values.
   configuration.minX = 0;
   configuration.minY = 0;
   configuration.maxX = 0;
   configuration.maxY = 0;
   // Inc calibration to the next step.
   Calibrating = 1;
}

void saveEEPROM()
{
  Serial.println("Saving calibration data!");
  // Set calibration to calibrated
  configuration.Calibrated = true;
  // Save data to the EEPROM
  EEPROM_writeAnything(0, configuration);
  // Pause to ensure that the save has completed
  delay(500);
// Show some status
  Serial.println("Calibration data saved!");
  //Pause a little to make things easyer to read.
  delay(500);
  
  Calibrating = -1;
}
