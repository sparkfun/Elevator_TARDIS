// "Elevator TARDIS"
// An example sketch for the Lilypad MP3 Player
// Mike Grusin, SparkFun Electronics
// http://www.sparkfun.com

// This sketch implements the mark 2 version of our "Elevator
// TARDIS".

// (For those who aren't fans of the British science fiction
// series "Doctor Who", the TARDIS is the Doctor's time and space
// machine. It's well-known for making a unique ethereal "grinding"
// noise when it takes off and lands.)

// The Elevator TARDIS is an electronic project that sits in 
// SparkFun's elevator. When the elevator starts moving, a MMA8452
// accelerometer detects the movement, and playing the TARDIS noise
// along with pulsing some blue LEDs. NOTE that there is NO hacking
// into the elevator's circuitry (that would be bad!). The ET is
// totally self-contained.

// You can read about the original version of the ET at this link:
// https://www.sparkfun.com/tutorials/319. For full information
// on this version, see https://github.com/sparkfun/Elevator_TARDIS

// Grateful acknowledgements:

// Doctor Who is copyright the BBC. No infringement is intended.

// This sketch uses the SdFat library by William Greiman. A working
// version is included in this repository, or download from:
// http://code.google.com/p/sdfatlib/

// This sketch uses the SFEMP3Shield library by Bill Porter. A working
// version is included in this repository, or download from:
// http://www.billporter.info/

// License:
// We use the "beerware" license for our firmware. You can do
// ANYTHING you want with this code. If you like it, and we meet
// someday, you can, but are under no obligation to, buy me a
// (root) beer in return.

// Have fun! 
// -your friends at SparkFun

// We'll need a few libraries to access all this fine hardware!
// Note that YOU will need to install the SdFat and SFEMP3Shield
// libraries, which are included in the above github repository.
// See: https://learn.sparkfun.com/tutorials/installing-an-arduino-library

#include <SPI.h>            // To talk to the SD card and MP3 chip
#include <SdFat.h>          // SD card file system
#include <SFEMP3Shield.h>   // MP3 decoder chip
#include <Wire.h>           // Wire (I2C) library

// Constants for the LilyPad MP3 Player outputs we'll be using:

const int ROT_LEDR = 10; // Blue TARDIS LED control on D10
const int EN_GPIO1 = A2; // Amp enable + MIDI/MP3 mode select
const int SD_CS = 9;     // Chip Select for SD card

// Accelerometer settings:

#define MMA8452_ADDRESS 0x1D // 0x1D if SA0 is high, 0x1C if low
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG 0x0E
#define WHO_AM_I 0x0D
#define CTRL_REG1 0x2A
#define GSCALE 2 // Sets full-scale range to +/-2, 4, or 8g.
int accel[3];
int zmin, zmax, zmean, zrange;

// Globals used for the blinking blue light:

float bluewave, blueenvelope;
const float TWOPI = 6.2831853;

// Create library objects:

SFEMP3Shield MP3player;
SdFat sd;

// Set debugging = true if you'd like status messages sent 
// to the serial port. Note that this will take over LilyPad MP3
// Player triggers 4 and 5. (You can have normally-open switches
// connected to triggers 4 and 5 and still use the serial port,
// as long as you're careful to NOT ground the triggers while
// you're actually using the serial port.)

boolean debugging = true;

void setup()
{
  byte result;
  int z,n;

  // If debugging is true (see above), initialize the serial port.
  // (The 'F' stores constant strings in flash memory to save RAM)

  if (debugging)
  {
    Serial.begin(9600);
    Serial.println(F("Elevator TARDIS mkII\n(for Lilypad MP3 Player)\nMike Grusin, SparkFun Electronics"));
  }

  // Set up I/O pins:
  
  pinMode(3,INPUT); // Interrupt input (unused in this sketch)
  pinMode(ROT_LEDR,OUTPUT);    // Blue LED control output
  digitalWrite(ROT_LEDR,LOW);  // LOW = LED off

  // The LilyPad uses a single I/O pin to select the
  // mode the MP3 chip will start up in (MP3 or MIDI),
  // and to enable/disable the amplifier chip:
  
  pinMode(EN_GPIO1,OUTPUT);
  digitalWrite(EN_GPIO1,LOW);  // MP3 mode / amp off

  // Initialize the SD card; SS = pin 9, half speed at first

  if (debugging) Serial.print(F("initialize SD card... "));

  result = sd.begin(SD_CS, SPI_HALF_SPEED); // 1 for success
  
  if (result != 1) // Problem initializing the SD card
  {
    if (debugging) Serial.print(F("ERROR\nmissing or unformatted card?\nhalting."));
    errorBlink(1,true); // Halt forever, blink LED if present.
  }
  else
    if (debugging) Serial.println(F("success!"));

  // Start up the MP3 chip:

  if (debugging) Serial.print(F("initialize MP3 chip... "));

  result = MP3player.begin(); // 0 or 6 for success

  // Check the result, see the library readme for error codes.

  if ((result != 0) && (result != 6)) // Problem starting up
  {
    if (debugging)
    {
      Serial.print(F("error code "));
      Serial.print(result);
      Serial.print(F(", halting."));
    }
    errorBlink(2,true); // Halt forever, blink red LED if present.
  }
  else
    if (debugging) Serial.println(F("success!"));

  // Set the VS1053 volume. 0 is loudest, 255 is lowest (off):

  MP3player.setVolume(20,20);
  
  // Turn on the amplifier chip:
  
  digitalWrite(EN_GPIO1,HIGH);
  delay(2);

  // Initialize the accelerometer:

  if (debugging) Serial.print(F("initialize accelerometer... "));

  Wire.begin();

  if (initMMA8452()) // Returns true if success
  {
    if (debugging) Serial.println(F("success!"));
  }
  else
  {
    if (debugging)
    {
      Serial.println(F("ERROR"));
      Serial.println(F("wiring problem?"));
      Serial.println(F("halting."));
    }
    errorBlink(3,true); // Halt forever, blink LED if present.
  }

  // All the hardware initialized successfully!
  // Blink once and continue:

  errorBlink(1,false);

  // Calibrate - take a bunch of accelerometer samples with the box
  // sitting still, and figure out the range of values we're seeing:

  if (debugging) Serial.println(F("calibrating in 10 seconds"));

  // You can optionally play a warning sound:
  
  MP3player.playMP3("calin10.mp3");

  delay(10000);

  MP3player.playMP3("calib.mp3");

  if (debugging) Serial.print(F("calibrating... "));

  digitalWrite(ROT_LEDR,HIGH); // Turn LED on

  zmin = 32767; // max signed int (0x7FFF)
  zmax = -32768; // min signed int (0x8000)

  for (n=0; n < 5000; n++)
  {
    // Read the current acceleration into the "accel" array
    readAccelData(accel);

    // Pull just the Z reading out of the array (x=0, y=1, z=2)
    z = accel[2];

    // Compare to current max and min, update if necessary
    if (z > zmax) zmax = z;
    if (z < zmin) zmin = z;
  }
  digitalWrite(ROT_LEDR,LOW); // Turn LED off

  // Compute noise range and mean:
  
  zrange = (zmax-zmin)/2;
  zmean =  zmin+zrange;
  
  if (debugging)
  {
    Serial.println(F("done."));
    Serial.print(F("max: ")); Serial.println(zmax);
    Serial.print(F("min: ")); Serial.println(zmin);
    Serial.print(F("mean: ")); Serial.println(zmean);
    Serial.print(F("range: ")); Serial.println(zrange);
  }
}

void loop()
{
  static int z, count, samples, sensitivity;

  // We'll now start repeatedly reading the Z acceleration value
  // (down), and test it to see if the elevator is moving.

  // You'll need to find your own "sensitivity" and "samples"
  // numbers for your application:
  
  // We've already calculated zmax and zmin. Between those numbers
  // is the normal "zero" range of the accelerometer.
  
  // "Sensitivity" controls how far outside that zero band
  // a reading needs to be before it counts as an acceleration.
  
  // There may also be momentary shocks as people enter and exit
  // the elevator, etc. A true acceleration will be sustained for
  // a longer period.
  
  // To filter out these shocks, we want to see a certain number
  // of acceleration events in a row. This is the "samples" number.

  // If your project triggers too easily, try increasing the
  // "sensitivity" and/or "samples" numbers.

  sensitivity = 20; // Range outside min/max to be counted
  samples = 40;     // Must be outside range this many times
  
  // Get the current accelerometer data
  
  readAccelData(accel);

  // Pull out just the Z value (0=X, 1=Y, 2=Z)
  
  z = accel[2];
  
  // See if we're outside the zero noise band
  
  if ((z < zmin - sensitivity) || (z > zmax + sensitivity))
    // If we are, increase the count
    count = count + 1;
  else
    // If we're not, reset the count to zero
    count = 0;

  // See if we're experiencing a sustained acceleration:  

  if (count > samples)
  {
    // Yes! If we're not playing the noise, start it up!
    
    if (!MP3player.isPlaying())
    {
      MP3player.playMP3("TARDIS.mp3");
      
      // Also reset the pulsing blue light numbers to zero:
      
      bluewave = 0.0;
      blueenvelope = 0.0;
    }
  }

  // Call the function that handles the pulsing blue light:
  
  bluelight();

  // Slow down a little bit (do this 100 times a second)
  
  delay(10);
}

void bluelight()
{
  // Slowly pulse the blue light.
  // We'll do this by generating a cosine wave (bluewave) that
  // performs the pulsing. We'll also slowly ramp up the maximum
  // brightness (blueenvelope)
 
  static int intensity;
  
  // Increment bluewave value (for cosine wave)
  // To pulse faster, increase the number
  bluewave += 0.05;
  
  // Keep the above value between 0 and two * PI
  if (bluewave > TWOPI) bluewave = 0.0;

  // Increment the brightness value
  // To get brighter faster, increase the number
  blueenvelope += 0.2;
  
  // 127 is the maximum brightness (half of 255)
  if (blueenvelope > 127.0) blueenvelope = 127.0;
  
  // If we're currently playing audio:

  if (MP3player.isPlaying())

    // Compute the intensity normally
    
    intensity = cos(bluewave) * -1 * blueenvelope + blueenvelope;

  // If we're NOT playing audio...

  else

    // And the LED is still lit...

    if (intensity > 0)

      // Finish up the current pulse until the intensity is zero
      // and stay there.

      intensity = cos(bluewave) * -1 * blueenvelope + blueenvelope;
    else
      intensity = 0;
    
  // Write the intensity to the LED output:
    
  analogWrite(ROT_LEDR,intensity);
}

void readAccelData(int *destination)
{
  byte rawData[6]; // x/y/z accel register data stored here

  readRegisters(OUT_X_MSB, 6, rawData); // Read the six raw data registers into data array

  // Loop to calculate 12-bit ADC and g value for each axis
  for(int i = 0; i < 3 ; i++)
  {
    int gCount = (rawData[i*2] << 8) | rawData[(i*2)+1]; //Combine the two 8 bit registers into one 12-bit number
    gCount >>= 4; //The registers are left align, here we right align the 12-bit integer

    // If the number is negative, we have to make it so manually (no 12-bit data type)
    if (rawData[i*2] > 0x7F)
    {
      gCount = ~gCount + 1;
      gCount *= -1; // Transform into negative 2's complement #
    }

    destination[i] = gCount; //Record this gCount into the 3 int array
  }
}

// Initialize the MMA8452 registers
// See the many application notes for more info on setting all of these registers:
// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
boolean initMMA8452()
{
  byte c = readRegister(WHO_AM_I); // Read WHO_AM_I register
  if (c != 0x2A) // WHO_AM_I should always be 0x2A
    return(false);

  MMA8452Standby(); // Must be in standby to change registers

  // Set up the full scale range to 2, 4, or 8g.
  byte fsr = GSCALE;
  if(fsr > 8) fsr = 8; //Easy error check
  fsr >>= 2; // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
  writeRegister(XYZ_DATA_CFG, fsr);

  //The default data rate is 800Hz and we don't modify it in this example code

  MMA8452Active(); // Set to active to start reading
  return(true);
}

// Sets the MMA8452 to standby mode. It must be in standby to change most register settings
void MMA8452Standby()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c & ~(0x01)); //Clear the active bit to go into standby
}

// Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Active()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c | 0x01); //Set the active bit to begin detection
}

// Read bytesToRead sequentially, starting at addressToRead into the dest byte array
void readRegisters(byte addressToRead, int bytesToRead, byte * dest)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  Wire.requestFrom(MMA8452_ADDRESS, bytesToRead); //Ask for bytes, once done, bus is released by default

  while(Wire.available() < bytesToRead); //Hang out until we get the # of bytes we expect

  for(int x = 0 ; x < bytesToRead ; x++)
    dest[x] = Wire.read();
}

// Read a single byte from addressToRead and return it as a byte
byte readRegister(byte addressToRead)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  Wire.requestFrom(MMA8452_ADDRESS, 1); //Ask for 1 byte, once done, bus is released by default

  while(!Wire.available()) ; //Wait for the data to come back
  return Wire.read(); //Return this one byte
}

// Writes a single byte (dataToWrite) into addressToWrite
void writeRegister(byte addressToWrite, byte dataToWrite)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToWrite);
  Wire.write(dataToWrite);
  Wire.endTransmission(); //Stop transmitting
}

void errorBlink(int blinks, boolean forever)
{
  // The following function will blink the LED a given number
  // of times and optionally repeat forever. This allows you to
  // see startup error codes without having to use the serial
  // monitor window.

  int x;

  do
  {
    for (x=0; x < blinks; x++) // Blink the given number of times
    {
      digitalWrite(ROT_LEDR,HIGH); // Turn LED ON
      delay(250);
      digitalWrite(ROT_LEDR,LOW); // Turn LED OFF
      delay(250);
    }
    delay(1500); // Longer pause between blink-groups
  } while(forever); // Loop forever or once
}



