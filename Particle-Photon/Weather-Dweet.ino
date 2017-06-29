// This #include statement was automatically added by the Particle IDE.
#include <HttpClient.h>

// This #include statement was automatically added by the Particle IDE.
#include <SparkFun_Photon_Weather_Shield_Library.h>

Weather sensor;

float t = 0;    //actual temperature
float h = 0;    //humidity
int WDIR = A0;
int RAIN = D2;
int WSPEED = D3; //march2017
int c = 0;
HttpClient http;
http_request_t request;
http_response_t response;

http_header_t headers[] = {
    //  { "Content-Type", "application/json" },
    //  { "Accept" , "/" },
    { "Accept" , "*/*"},
    { NULL, NULL } 
};

void setup() {
    sensor.begin();
    sensor.setOversampleRate(7);
    sensor.setModeBarometer();
    pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
    pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor march 2017
    pinMode(WDIR, INPUT_PULLUP);

    delay(1000);
}

void loop() {
    getClimateData();
    publishData();
    publishDweet();
    delay(60000);
    //System.sleep(30);
    //delay(1000);
}

void publishDweet() {
    
    char json[255];
    
    request.hostname = "dweet.io";
    request.port = 80;
    request.path = "/dweet/for/weather-dob_91?";
	
	//json 
	// request.body = "{\"humidity\": h,\"temp\": t}"; //"{\"key\":\"value\"}";
	snprintf(json, sizeof(json), "{ \"t\": %.1f, \"h\": %.1f, \"c\": %i }", t, h, c);
	request.body = json;
	
	
	Particle.publish("DEBUG-1",request.body);
	
    // Get request
    http.post(request, response, headers);
    
    Particle.publish("DEBUG-2",response.status);

}


void getClimateData() {
    t = sensor.getTempF();
    t = (t - 32) * 5/9;
    h = sensor.getRH();
    c = get_wind_direction();
}

//Read the wind direction sensor, return heading in degrees
int get_wind_direction()
{
  unsigned int adc;

  adc = analogRead(WDIR); // get the current reading from the sensor

  // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
  // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
  // Note that these are not in compass degree order! See Weather Meters datasheet for more information.

  //Wind Vains may vary in the values they return. To get exact wind direction,
  //it is recomended that you AnalogRead the Wind Vain to make sure the values
  //your wind vain output fall within the values listed below.
  if(adc > 2270 && adc < 2290) return (0);//North
  if(adc > 3220 && adc < 3299) return (1);//NE
  if(adc > 3890 && adc < 3999) return (2);//East
  if(adc > 3780 && adc < 3850) return (3);//SE

  if(adc > 3570 && adc < 3650) return (4);//South
  if(adc > 2790 && adc < 2850) return (5);//SW
  if(adc > 1580 && adc < 1610) return (6);//West
  if(adc > 1930 && adc < 1950) return (7);//NW

  //return (-1); // error, disconnected?
  return(adc);
}

void publishData() {
    char payload[255]; 

    //  {
    //      "tC": ##.#
    //      "h": ##.#
    //  }
    snprintf(payload, sizeof(payload), "{ \"t\": %.1f, \"h\": %.1f, \"c\": %i }", t, h, c);
    Particle.publish("climate", payload, 60);
}
