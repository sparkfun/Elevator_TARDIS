// "Elevator TARDIS"
// An example sketch for the Lilypad MP3 Player
// Mike Grusin, SparkFun Electronics
// http://www.sparkfun.com

// This sketch implements the mark 2 version of our "Elevator
// TARDIS". You can see the first version at this link:
// https://www.sparkfun.com/tutorials/319). For full information
// on this version, see: 

// Uses the SdFat library by William Greiman, which is supplied
// with this archive, or download from http://code.google.com/p/sdfatlib/

// Uses the SFEMP3Shield library by Bill Porter, which is supplied
// with this archive, or download from http://www.billporter.info/

// License:
// We use the "beerware" license for our firmware. You can do
// ANYTHING you want with this code. If you like it, and we meet
// someday, you can, but are under no obligation to, buy me a
// (root) beer in return.

// Have fun! 
// -your friends at SparkFun

// Revision history:
// 1.0 initial release MDG 2012/11/01


// We'll need a few libraries to access all this hardware!

#include <SPI.h>            // To talk to the SD card and MP3 chip
#include <SdFat.h>          // SD card file system
#include <SFEMP3Shield.h>   // MP3 decoder chip
#include <Wire.h>           // Wire (I2C) library

// Constants for a few outputs we'll be using:

const int ROT_LEDR = 10; // Red LED in rotary encoder (optional)
const int EN_GPIO1 = A2; // Amp enable + MIDI/MP3 mode select
const int SD_CS = 9;     // Chip Select for SD card

#define DEVICE 0x53      //ADXL345 device address
#define REGISTER 0x32    //first axis-acceleration-data register on the ADXL345
#define BYTES_TO_READ 6  //num of bytes we are going to read each time (two bytes for each axis)
byte buff[BYTES_TO_READ];//6 bytes buffer for saving data read from the device

//float bluelight;
//float blueenvelope;
const float TWOPI = 6.283185307179586476925286766559;

// Create library objects:

SFEMP3Shield MP3player;
SdFat sd;

// Set debugging = true if you'd like status messages sent 
// to the serial port. Note that this will take over trigger
// inputs 4 and 5. (You can leave triggers connected to 4 and 5
// and still use the serial port, as long as you're careful to
// NOT ground the triggers while you're using the serial port).

boolean debugging = true;


void setup()
{
  int x, index;
  SdFile file;
  byte result;
  char tempfilename[13];
  int range;

  // If debugging is true, initialize the serial port:
  // (The 'F' stores constant strings in flash memory to save RAM)

  if (debugging)
  {
    Serial.begin(9600);
    Serial.println(F("Elevator TARDIS mkII\n(for Lilypad MP3 Player)\nMike Grusin, SparkFun Electronics"));
  }

  pinMode(3,INPUT);

  // If serial port debugging is inconvenient, you can connect
  // a LED to the red channel of the rotary encoder to blink
  // startup error codes:
  
  pinMode(ROT_LEDR,OUTPUT);
  digitalWrite(ROT_LEDR,LOW);  // LOW = off

  // The board uses a single I/O pin to select the
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

    MP3player.playMP3("prob.mp3");

    errorBlink(1,true); // Halt forever, blink LED if present.
  }
  else
    if (debugging) Serial.println(F("success!"));

  // Start up the MP3 library

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
    errorBlink(result,true); // Halt forever, blink red LED if present.
  }
  else
    if (debugging) Serial.println(F("success!"));

  // Initialize the accelerometer

  if (debugging) Serial.print(F("initialize accelerometer... "));

  Wire.begin();

  if ((readFrom(DEVICE, 0x00, 1, &result)) && (result == 0xE5))
  {
    writeTo(DEVICE, 0x2D, 0);  // reset
    writeTo(DEVICE, 0x2D, 8);  // measurement mode
    writeTo(DEVICE, 0x38, 0);  // fifo off
    writeTo(DEVICE, 0x31, 0);  // 2g range, active high interrupts
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

  // Set the VS1053 volume. 0 is loudest, 255 is lowest (off):

  MP3player.setVolume(30,30);
  
  // Turn on the amplifier chip:
  
  digitalWrite(EN_GPIO1,HIGH);
  delay(2);

  // Calibrate - take a bunch of accelerometer samples
  // and figure out the range.

  if (debugging) Serial.println(F("calibrating in 10 seconds"));
  MP3player.playMP3("calin10.mp3");
  errorBlink(1,false);

  delay(10000);

  if (debugging) Serial.print(F("calibrating... "));
  MP3player.playMP3("calib.mp3");
  digitalWrite(ROT_LEDR,HIGH); // Turn LED ON

  int z;
  int zmin = 0x7FFF; // max signed int (32767)
  int zmax = 0x8000; // min signed int (-32768)
  unsigned int n;

  for (n=0; n < 10000; n++)
  {
    // read the current acceleration from the ADXL345:
    //read the acceleration data from the ADXL345
    readFrom(DEVICE, REGISTER, BYTES_TO_READ, buff);
    //each axis reading is 10 bits in two bytes. combine these into one int.
    z = (((int)buff[5]) << 8) | buff[4];

    if (z > zmax) zmax = z;
    if (z < zmin) zmin = z;
  }

  if (debugging) Serial.println(F("done."));
  digitalWrite(ROT_LEDR,LOW); // Turn LED ON

  Serial.print("max: "); Serial.println(zmax);
  Serial.print("min: "); Serial.println(zmin);
  Serial.print("mean: "); Serial.println(((zmax-zmin)/2)+zmin);
  Serial.print("range: "); Serial.println((zmax-zmin)/2);
  
  range = (zmax-zmin)/2;

  // Set up for interrupt wakeup

   writeTo(DEVICE, 0x24, range*3);  // activity threshold
   writeTo(DEVICE, 0x27, 144);  // z axis, ac-coupled
   writeTo(DEVICE, 0x2F, 0);  // int to INT1
   writeTo(DEVICE, 0x2E, 16); // enable activity int
  //  read 0x30 to clear interrupt
}


void loop()
{
//  if (!MP3player.isPlaying())
//    MP3player.playMP3("TARDIS.mp3");
//
//  Mp3whilePlaying();
  byte result;

  while(1)
  {
    if (digitalRead(3))
    {
      digitalWrite(ROT_LEDR,HIGH);
      delay(100);
      readFrom(DEVICE, 0x30, 1, &result);
      digitalWrite(ROT_LEDR,LOW);
    }
  }



  
  delay(5);

  int x, y, z;
  static int count;

  // read the current acceleration from the ADXL345:
//  if readFrom(DEVICE, REGISTER, BYTES_TO_READ, buff); //read the acceleration data from the ADXL345
  //each axis reading is 10 bits in two bytes. combine these into one int.
  x = (((int)buff[1]) << 8) | buff[0];   
  y = (((int)buff[3])<< 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];
  
  // from experimentation, we know that 1g is about 230.
  // We'll look for values a little outside that range.
  // We also want to look for a sustained acceleration, not single spikes,
  // so we'll wait until we have 10 values in a row that fall outside that range.

  Serial.print(x);
  Serial.print("  ");
  Serial.print(y);
  Serial.print("  ");
  Serial.println(z);


/*  
  if ((z > 238) || (z < 220))
    count++;
  else
    count = 0;
    
  if (count > 10) // bingo! let's play the noise and light the lights
  {
	// reset the light values to 0
    bluelight = 0.0;
    blueenvelope = 0.0;

	// turn on the amplifier
    digitalWrite(5,HIGH);

	// start playing the sound
    sprintf(trackName, "TARDIS.MP3");
    playMP3(trackName);

	// done playing, turn off the amplifier
    digitalWrite(5,LOW);

	// if the light is still some level of "on", fade it to off
    while (bluelight > 0)
    {
      Mp3whilePlaying();
      delay(10);
    }
  }
  // ADXL delay
  delay(10);
*/
}


void Mp3whilePlaying() // called repeatedly by MP3 player code between data transfers.
// typically runs every few ms, don't dawdle more than 100ms here or the MP3 chip will starve!
{
  static float bluelight, blueenvelope;
  
  // slowly pulse the blue light.
  // we'll do this by generating a cosine wave from the incrementing value bluelight
  // we'll also slowly brighten the blue light using the blueenvelope value
  
  // increment bluelight value (for cosine wave)
  bluelight += 0.04;
  
  // keep the above value between 0 and two * PI
  if (bluelight > TWOPI) bluelight = 0.0;

  // increment the brightness value
  blueenvelope += 0.2;
  
  // 127 is maximum brightness
  if (blueenvelope > 127.0) blueenvelope = 127.0;
  
  // generate the brightness of the light using bluelight and blueenvelope
  analogWrite(ROT_LEDR,cos(bluelight) * -1 * blueenvelope + blueenvelope);
}

boolean readFrom(int device, byte address, int num, byte buff[]) 
{
  int result;
  
  Wire.beginTransmission(device); //start transmission to device 
  Wire.write(address);        //sends address to read from
  if (Wire.endTransmission()) return(false); //end transmission
  
  Wire.beginTransmission(device); //start transmission to device (initiate again)
  Wire.requestFrom(device, num);    // request 6 bytes from device
  
  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  { 
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  if (Wire.endTransmission()) return(false); //end transmission
  return(true);
}

void writeTo(int device, byte address, byte val) 
{
  Wire.beginTransmission(device); //start transmission to device 
  Wire.write(address);        // send register address
  Wire.write(val);        // send value to write
  Wire.endTransmission(); //end transmission
}


void errorBlink(int blinks, boolean forever)
{
  // The following function will blink the red LED in the rotary
  // encoder (optional) a given number of times and repeat forever.
  // This is so you can see any startup error codes without having
  // to use the serial monitor window.

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

