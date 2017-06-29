//https://www.hackster.io/gusgonnet/storing-data-in-google-714dde

#include "PietteTech_DHT/PietteTech_DHT.h"

#define PUSHBULLET_NOTIF "pushbullet"

// system defines
#define DHTTYPE  DHT22              // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   4         	    // Digital pin for communications
#define DHT_SAMPLE_INTERVAL   5000

//declaration
void dht_wrapper(); // must be declared before the lib initialization

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);

// globals
unsigned int DHTnextSampleTime;	    // Next time we want to start sample
bool bDHTstarted;		    // flag to indicate we started acquisition
int n;                              // counter

char resultstr[64]; //String to store the sensor temp
char humiditystr[64]; //String to store the sensor humidity

char VERSION[64] = "0.11";
bool dryer_on = false;
String dryer_stat = "";
int humidity_samples_below_10 = 0;
int humidity_samples_below_5 = 0;

void setup()
{

 DHTnextSampleTime = 0;  // Start the first sample immediately
 Particle.variable("temp", resultstr, STRING);
 Particle.variable("humidity", humiditystr, STRING);
 Particle.variable("dryer_stat", dryer_stat);

 Particle.publish("DHT22 - firmware version", VERSION, 60, PRIVATE);

 bool success = Particle.function("resetDryer", resetDryer);
 if (not success) {
    Particle.publish("ERROR", "Failed to register function resetDryer", 60, PRIVATE);
 }
 
}


// This wrapper is in charge of calling
// must be defined like this for the lib work
void dht_wrapper() {
    DHT.isrCallback();
}

void loop()
{

  // Check if we need to start the next sample
  if (millis() > DHTnextSampleTime) {
      
	if (!bDHTstarted) {		// start the sample
	    DHT.acquire();
	    bDHTstarted = true;
	}

	if (!DHT.acquiring()) {		// has sample completed?

 float temp = (float)DHT.getCelsius();
 int temp1 = (temp - (int)temp) * 100;

 char tempInChar[32];
 sprintf(tempInChar,"%0d.%d", (int)temp, temp1);

 //google docs will get this variable - if you are storing the value in a google sheet
 sprintf(resultstr, "{\"t\":%s}", tempInChar);

 char humiInChar[32];
 float humid = (float)DHT.getHumidity();
 int humid1 = (humid - (int)humid) * 100;

 sprintf(humiInChar,"%0d.%d", (int)humid, humid1);

 //google docs will get this variable - if you are storing the value in a google sheet
 sprintf(humiditystr, "{\"t\":%s}", humiInChar);


  n++;  // increment counter
  bDHTstarted = false;  // reset the sample flag so we can take another
  DHTnextSampleTime = millis() + DHT_SAMPLE_INTERVAL;  // set the time for next sample

  //if humidity goes above 50% then we believe the dryer has just started a cycle
  if ( ( not dryer_on ) and ( humid > 50 ) and ( temp > 30 ) ) {
    dryer_on = true;
    humidity_samples_below_10 = 0;
    humidity_samples_below_5 = 0;
    Particle.publish(PUSHBULLET_NOTIF, "Your dryer started a cycle", 60, PRIVATE);
    
  }

  if ( dryer_on and ( humid < 10) and ( temp > 50 ) ) {
    humidity_samples_below_10 = humidity_samples_below_10 +1;
  }

  //if there are 5 samples below 10% then we are sure it's almost done
  if ( dryer_on and (humidity_samples_below_10 >= 5) ) {
        Particle.publish(PUSHBULLET_NOTIF, "Your clothes are dry", 60, PRIVATE);
        dryer_on = false;
  }

  if( dryer_on ) {
      dryer_stat = "dryer_on";
  } else {
        dryer_stat = "dryer_off";
  }

 }
 }
 
}

//call this function when the algorithm gets stuck (you aborted the dryer cycle, etc)
int resetDryer(String args)


//Google Code

"""function collectDatadht22() {
  var  sheet = SpreadsheetApp.getActiveSheet();

  var response = UrlFetchApp.fetch("https://api.spark.io/v1/devices/__YOUR_PARTICLE_DEVICE_ID__/temp?access_token=__YOUR_PARTICLE_ACCESS_TOKEN__");
  var response2 = UrlFetchApp.fetch("https://api.spark.io/v1/devices/__YOUR_PARTICLE_DEVICE_ID__/humidity?access_token=__YOUR_PARTICLE_ACCESS_TOKEN__");

  try {
    var response = JSON.parse(response.getContentText()); // parse the JSON the Core API created
    var result = unescape(response.result); // you'll need to unescape before your parse as JSON

    var response2 = JSON.parse(response2.getContentText()); // parse the JSON the Core API created
    var result2 = unescape(response2.result); // you'll need to unescape before your parse as JSON

    try {
      var p = JSON.parse(result); // parse the JSON you created
      var p2 = JSON.parse(result2); // parse the JSON you created
      var d = new Date(); // time stamps are always good when taking readings
      sheet.appendRow([d, p.t, p2.t]); // append the date and data to the sheet
      sheet.getRange(2,4).setValue(d);
      sheet.getRange(2,5).setValue(p.t);
      sheet.getRange(2,6).setValue(p2.t);
      } catch(e)
    {
      Logger.log("Unable to do second parse");
    }
  } catch(e)
  {
    Logger.log("Unable to return JSON");
  }
  
}"""
