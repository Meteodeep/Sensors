/******************************************************************************
  SparkFun Photon Weather Shield basic example
  Joel Bartlett @ SparkFun Electronics
  Original Creation Date: May 18, 2015
  Updated August 21, 2015
  This sketch prints the temperature, humidity, and barometric pressure OR
  altitude to the Serial port.

  The library used in this example can be found here:
  https://github.com/sparkfun/SparkFun_Photon_Weather_Shield_Particle_Library

  Hardware Connections:
	This sketch was written specifically for the Photon Weather Shield,
	which connects the HTU21D and MPL3115A2 to the I2C bus by default.
  If you have an HTU21D and/or an MPL3115A2 breakout,	use the following
  hardware setup:
      HTU21D ------------- Photon
      (-) ------------------- GND
      (+) ------------------- 3.3V (VCC)
       CL ------------------- D1/SCL
       DA ------------------- D0/SDA

    MPL3115A2 ------------- Photon
      GND ------------------- GND
      VCC ------------------- 3.3V (VCC)
      SCL ------------------ D1/SCL
      SDA ------------------ D0/SDA

  Development environment specifics:
  	IDE: Particle Dev
  	Hardware Platform: Particle Photon
                       Particle Core

  This code is beerware; if you see me (or any other SparkFun
  employee) at the local, and you've found our code helpful,
  please buy us a round!
  Distributed as-is; no warranty is given.
*******************************************************************************/
#include "SparkFun_Photon_Weather_Shield_Library.h"

float humidity = 0;
float tempf = 0;
float pascals = 0;
float baroTemp = 0;

long lastPrint = 0;

int tempc = 0;
int tempfint = 0;
int baroc = 0;
int barofint = 0;

//Create Instance of HTU21D or SI7021 temp and humidity sensor and MPL3115A2 barometric sensor
Weather sensor;

//---------------------------------------------------------------
void setup()
{
    Serial.begin(9600);   // open serial over USB at 7200 baud

    // Make sure your Serial Terminal app is closed before powering your device
    // Now open your Serial Terminal, and hit any key to continue!
    Serial.println("Press any key to begin");
    //This line pauses the Serial port until a key is pressed
    while(!Serial.available()) Spark.process();

    //Initialize the I2C sensors and ping them
    sensor.begin();

    /*You can only receive accurate barometric readings or accurate altitude
    readings at a given time, not both at the same time. The following two lines
    tell the sensor what mode to use. You could easily write a function that
    takes a reading in one made and then switches to the other mode to grab that
    reading, resulting in data that contains both accurate altitude and barometric
    readings. For this example, we will only be using the barometer mode. Be sure
    to only uncomment one line at a time. */
    sensor.setModeBarometer();//Set to Barometer Mode
    //baro.setModeAltimeter();//Set to altimeter Mode

    //These are additional MPL3115A2 functions that MUST be called for the sensor to work.
    sensor.setOversampleRate(7); // Set Oversample rate
    //Call with a rate from 0 to 7. See page 33 for table of ratios.
    //Sets the over sample rate. Datasheet calls for 128 but you can set it
    //from 1 to 128 samples. The higher the oversample rate the greater
    //the time between data samples.


    sensor.enableEventFlags(); //Necessary register calls to enble temp, baro and alt

}
//---------------------------------------------------------------
void loop()
{
      //Get readings from all sensors
      getWeather();

      // This math looks at the current time vs the last time a publish happened
      if(millis() - lastPrint > 5000) //Publishes every 5000 milliseconds, or 5 seconds
      {
        // Record when you published
        lastPrint = millis();

        // Use the printInfo() function to print data out to Serial
        printInfo();
      }
}
//---------------------------------------------------------------
void getWeather()
{
  // Measure Relative Humidity from the HTU21D or Si7021
  humidity = sensor.getRH();

  // Measure Temperature from the HTU21D or Si7021
  tempf = sensor.getTempF();
  
  tempfint = (int)tempf;

  tempc = (tempfint - 32) * 0.5556;
  
  // Temperature is measured every time RH is requested.
  // It is faster, therefore, to read it from previous RH
  // measurement with getTemp() instead with readTemp()

  //Measure the Barometer temperature in F from the MPL3115A2
  baroTemp = sensor.readBaroTempF();
  
    barofint = (int)baroTemp;

  baroc = (barofint - 32) * 0.5556;

  //Measure Pressure from the MPL3115A2
  pascals = sensor.readPressure();

  //If in altitude mode, you can get a reading in feet with this line:
  //float altf = sensor.readAltitudeFt();
}
//---------------------------------------------------------------
void printInfo()
{
//This function prints the weather data out to the default Serial Port

  Serial.print("Temp:");
  Serial.print(tempc);
  Serial.print("C ");

  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.print("%, ");

  Serial.print("Baro_Temp:");
  Serial.print(baroc);
  Serial.print("C, ");

  Serial.print("Pressure:");
  Serial.print(pascals/100);
  Serial.print("hPa, ");
  Serial.print((pascals/100) * 0.0295300);
  Serial.println("in.Hg");
  //The MPL3115A2 outputs the pressure in Pascals. However, most weather stations
  //report pressure in hectopascals or millibars. Divide by 100 to get a reading
  //more closely resembling what online weather reports may say in hPa or mb.
  //Another common unit for pressure is Inches of Mercury (in.Hg). To convert
  //from mb to in.Hg, use the following formula. P(inHg) = 0.0295300 * P(mb)
  //More info on conversion can be found here:
  //www.srh.noaa.gov/images/epz/wxcalc/pressureConversion.pdf

  //If in altitude mode, print with these lines
  //Serial.print("Altitude:");
  //Serial.print(altf);
  //Serial.println("ft.");

}
