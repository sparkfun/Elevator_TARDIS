##Elevator TARDIS
**Mike Grusin, SparkFun Electronics**

Welcome to the Mark 2 version of our "Elevator TARDIS", [as seen on TV] (https://www.youtube.com/watch?v=Ievh0FoUuvg) and [around the internet] (http://boingboing.net/2012/02/28/howto-sneak-an-accelerometer-t.html).

For those who aren't fans of the British science fiction series ["Doctor Who"](http://en.wikipedia.org/wiki/Doctor_who), the TARDIS is the Doctor's time and space machine. It's well-known for making a unique ethereal "grinding" noise when it takes off and lands, possibly because he left the parking brake on.

The Elevator TARDIS is an electronic project that sits in SparkFun's elevator. When the elevator starts moving, an [accelerometer](https://www.sparkfun.com/products/10955) detects the movement, triggering a [LilyPad MP3 Player](https://www.sparkfun.com/products/11013) to play the TARDIS noise, and pulse high-powered blue LEDs. Note that the Elevator TARDIS is completely self contained. It requires NO hacking into the elevator's circuitry (that would be bad, don't do it!). You can read the story of the original Elevator TARDIS at this link: https://www.sparkfun.com/tutorials/319.

Also note that this type of sensor-triggered light and noisemaker is useful for all kinds of projects. Your own theme song when you walk around? A bike that makes screeching noises when you stop? Something that says "put me down!" when you pick it up? Use your imagination!

###Grateful acknowledgements

Doctor Who is copyright the BBC. No infringement is intended.

This sketch uses the SdFat library by William Greiman. A working version is included in this repository, or download from: http://code.google.com/p/sdfatlib/

This sketch uses the SFEMP3Shield library by Bill Porter. A working version is included in this repository, or download from: http://www.billporter.info/

###License

We use the "beerware" license for our firmware. You can do ANYTHING you want with this code. If you like it, and we meet
someday, you can, but are under no obligation to, buy me a (root) beer in return.

Have fun!<br>
\- your friends at SparkFun

##Quick how-to

###Install the software

####Install the Arduino IDE

If you haven't, [install the Arduino IDE](https://learn.sparkfun.com/tutorials/installing-arduino-ide) (Integrated Development Environment), available at http://www.arduino.cc.

####Download the repository

A "repository" is a collection of code and other files associated with a project. You can download the Elevator TARDIS repository by clicking the ["Download ZIP"](https://github.com/sparkfun/Elevator_TARDIS/archive/master.zip) button on the [repository's main page](https://github.com/sparkfun/Elevator_TARDIS). Once you've downloaded it, unzip it to a convenient place.

####Install the libraries and TARDIS sketch

This software requires several nonstandard libraries:

* SdFat (by William Greiman, http://code.google.com/p/sdfatlib/)
* SFEMP3Shield (by Bill Porter, http://www.billporter.info/)

Working versions of these libraries are included in the repository. To install them, you will need to move them to a "libraries" folder within your Arduino sketch folder. You'll also need to move the "Elevator_TARDIS" folder into your Arduino sketch folder.

The one-step procedure is to drag the contents of the Arduino folder from this repository to your personal Arduino sketch folder. This will create a "libraries" folder (containing support libraries), and an "Elevator_TARDIS" folder (containing the main sketch). If there is already a "libraries" folder there, go ahead and add the included libraries to it. You'll need to restart the Arduino IDE to get it to recognize the new libraries.

See our tutorial for more information: [Installing an Arduino Library](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)

###Assemble the hardware

<p>1. See the wishlist of required hardware at: https://www.sparkfun.com/wish_lists/88135.

* You will also need one or two speakers. Almost any speaker will work, the larger the speaker the better it will sound.
* You'll need a resistor for the LEDs, anything from 20 ohms to 100 ohms will work.
* You will also need soldering tools and solder, and basic workbench tools.

<p>2. Place the audio files located in the "audio" folder of the repository onto your micro-SD card. (Don't change the filenames unless you also change them in the code.)

<p>3. Solder the breakaway headers (in ones and twos) to the LilyPad MP3 Player using this guide:

![Headers](https://raw.githubusercontent.com/sparkfun/Elevator_TARDIS/master/documentation/lilypad%20header%20points.png)

<p>4. Also solder headers onto the Accelerometer and MOSFET board.

<p>5. Using the F/F Jumper Wires, wire the components together using this guide:

![Layout](https://raw.githubusercontent.com/sparkfun/Elevator_TARDIS/master/documentation/layout%20full.png)

For the "Y" connections on the LEDs, you can cut the jumper wires and splice them together. We recommend insulating the bare connections with heat shrink tubing, electrical tape, etc. to prevent shorts.

<p>6. You will probably want to load the software (see below) and make sure everything works while it's sprawled out on a table. Once you know it works, you can mount everything in a box or case of your choosing using standoffs and screws, hot glue, foam tape, etc.

###Load the sketch

To load the Elevator_TARDIS sketch onto the LilyPad MP3 Player:

1. Connect the LilyPad MP3 Player to your computer using a 5V FTDI board or cable. Turn the LilyPad MP3 Player ON. The red LED should light up. Note that a Lipo battery or external power source is required to run the LilyPad MP3 Player.

2. Start the Arduino IDE.

3. Select the correct board type from the menu: Tools / Board / "Arduino Pro or Pro Mini (3.3V, 8MHz) w/ATmega328".

4. Select the correct serial port from the menu: Tools / Serial Port / (the port that your FTDI board or cable has created, usually the highest number).

5. Load the Elevator_TARDIS sketch into the Arduino IDE.

6. Click on the Upload (right arrow) button at the top of the window. The code should compile then upload to the LilyPad MP3 Player.

If there are compilation errors, double-check that you installed the required libraries to the correct place, and restarted the Arduino IDE.

If you have uploading errors, ensure that the LilyPad MP3 Player is turned on, that you've selected the correct board type and serial port, and that you're using a 5V (not a 3.3V) FTDI board.

###Operation

The sketch will first initialize all the hardware. If the LEDs repeatedly blink, there was an error (wiring problem, no SD card, etc.). You can open a serial monitor window at 9600 baud to get more information.

The sketch will then calibrate the accelerometer. The sketch will wait ten seconds, then calibrate for 5 seconds or so (the LEDs will be on while calibration takes place). Ensure that the project is not moved while calibration is taking place.

After calibration, the project is active! If the box detects sustained acceleration up or down, it will play the TARDIS noise and pulse the LEDs. If the box is too sensitive or not sensitive enough, you can change the sensitivity values in the code (see the beginning of the main() loop). After the audio has finished, it will reset for the next trip through time and space. 

###A final word of caution:

SparkFun is one of a very few companies where one can put a scary-looking jumble of circuit boards in an elevator without someone immediately calling Homeland Security. Even so, it’s dicey - what would the elevator repair people do if they ran across this? There have been several high-profile incidents recently where makers have done things they thought were perfectly innocent, that resulted in a disproportionately large response from the law-enforcement community. Unfortunately, this is the environment we live in, and until it changes, think three times before deploying smart pranks like this. Then have fun!

*"To the rational mind, nothing is inexplicable, only unexplained"*<br>\- The Doctor.

Have fun!<br>
\- your friends at SparkFun