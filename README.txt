"Elevator TARDIS"
Mike Grusin, SparkFun Electronics

Welcome to the Mark 2 version of our "Elevator TARDIS", as seen on TV (https://www.youtube.com/watch?v=Ievh0FoUuvg) and around the internet (http://boingboing.net/2012/02/28/howto-sneak-an-accelerometer-t.html)

For those who aren't fans of the British science fiction
series "Doctor Who", the TARDIS is the Doctor's time and space
machine. It's well-known for making a unique ethereal "grinding"
noise when it takes off and lands.

The Elevator TARDIS is an electronic project that sits in 
SparkFun's elevator. When the elevator starts moving, a MMA8452
accelerometer detects the movement, triggering a LilyPad MP3 Player to play the TARDIS noise and pulse high-powered blue LEDs. NOTE that this requires NO hacking into the elevator's circuitry (that would be bad). The ET is totally self-contained.

You can read about the original version of the ET at this link:
https://www.sparkfun.com/tutorials/319. For full information
on this version, see https://github.com/sparkfun/Elevator_TARDIS

Grateful acknowledgements:

Doctor Who is copyright the BBC. No infringement is intended.

This sketch uses the SdFat library by William Greiman. A working
version is included in this repository, or download from:
http://code.google.com/p/sdfatlib/

This sketch uses the SFEMP3Shield library by Bill Porter. A working
version is included in this repository, or download from:
http://www.billporter.info/

License:
We use the "beerware" license for our firmware. You can do
ANYTHING you want with this code. If you like it, and we meet
someday, you can, but are under no obligation to, buy me a
(root) beer in return.

Have fun! 
-your friends at SparkFun

Quick how-to:

Setting up the hardware:

See the wishlist of required hardware at: https://www.sparkfun.com/wish_lists/88135

Solder the headers (in ones and twos) to the LilyPad MP3 Player using this guide: 

Wire the components together using this guide:

Place the audio files located in: onto your micro-SD card.

Loading the software:

You should install the latest version of the Arduino IDE, available at http://www.arduino.cc.

This software requires several nonstandard libraries:
	SdFat (by William Greiman)
	SFEMP3Shield (by Bill Porter)
	
	Working versions of these libraries as of this date are included in this repository.

TL;DR: The one-step procedure is to drag the contents of the Arduino folder to your personal Arduino sketch folder. This will create a "libraries" folder (containing support libraries), and an "Elevator_TARDIS" folder (containing the main sketch).

To install the libraries manually, copy the "libraries" folder to your personal Arduino sketch directory. (If there is already a "libraries" folder there, go ahead and add the included libraries to it.) You'll need to restart the Arduino IDE to get it to recognize the new libraries.

To install the example sketches, copy the "Elevator_TARDIS" folder to your personal Arduino sketch folder.

Loading the sketches:

To load the Elevator_TARDIS sketch onto the LilyPad MP3 Player:

1. Connect the LilyPad MP3 Player to your computer using a 5V FTDI board or cable (available at http://www.sparkfun.com). Turn the LilyPad MP3 Player ON. The red LED should light up. Note that a Lipo battery or external power source is required to run the LilyPad MP3 Player.

2. Start the Arduino IDE.

3. Select the correct board type from the menu: Tools / Board / "Arduino Pro or Pro Mini (3.3V, 8MHz) w/ATmega328"

4. Select the correct serial port from the menu: Tools / Serial Port / (the port that your FTDI board or cable has created, usually the highest number)

5. Load the desired sketch into the Arduino IDE.

6. Click on the Upload (right arrow) button at the top of the window. The code should compile then upload to the LilyPad MP3 Player. If you have errors, double-check that you installed the required libraries to the correct place, and restarted the Arduino IDE.

Operation:

The sketch will first initialize all the hardware. If the LEDs repeatedly blink, there was an error. You can open a serial monitor window at 9600 baud to get more information.

The sketch will then calibrate the accelerometer. The sketch will wait ten seconds, then calibrate for 5 seconds or so (the LEDs will be on). Ensure that the project is not moved while calibration is taking place.

After calibration, the project is active! If the box detects sustained acceleration up or down, it will play the TARDIS noise and pulse the LEDs. After the audio has finished, it will reset for the next trip through time and space.

Have fun!
- Your friends at SparkFun.

