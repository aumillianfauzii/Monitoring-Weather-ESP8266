    #include <Blynk.h>
    #include <ESP8266WiFi.h>
    #include <Wire.h>
    #include <BlynkSimpleEsp8266.h>
    #include <Adafruit_Sensor.h>
    #include <Adafruit_BME280.h>
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
   // #include <WiFi.h>
   // #include <HTTPClient.h>
    //#include <WiFiClient.h>
    #define SEALEVELPRESSURE_HPA (1013.25)
    
    //Setup connection of the sensor
    Adafruit_BME280 bme; // I2C

    char auth[] = "nz8JR9UZtjDzgejQ74NcnxqEvkDuSBRa";
    char sid[] = "asd";
    char pass[] = "123456789";

   // const char* ssid = "asd";
   //const char* password = "123456789";
    const char* serverName = "http://sensordatas.000webhostapp.com/esp-post-data.php";
    
    // Keep this API Key value to be compatible with the PHP code provided in the project page.
    // If you change the apiKeyValue value, the PHP file /esp-post-data.php also needs to have the same key
    String apiKeyValue = "tPmAT5Ab3j7F9";
    String sensorName = "BME280";
    String sensorLocation = "Office";

    unsigned long lastTime = 0;
    unsigned long timerDelay = 5000;
    BlynkTimer timer;

    //Variables
    float pressure;     //To store the barometric pressure (Pa)
    float temperature;  //To store the temperature (oC)
    float humidity;     //To store the humidity (%) (you can also use it as a float variable)
    float altimeter;      //To store the humidity (%) (you can also use it as a float variable)

    void setup() {
      bme.begin(0x76);    //Begin the sensor`
      Serial.begin(115200); //Begin serial communication at 9600bps
      Serial.println("Adafruit BME280 test:");
      Serial.print("\t");
      Serial.println("Connecting");
      while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("."); }
      bool status = bme.begin(0x76);
      if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring or change I2C address!");
      while (1);}
  
      
      Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
      Blynk.begin(auth, sid,pass);
      timer.setInterval(5000L, ReadSensors);   // read sensor every 5s 
    }

    void ReadSensors(){
      //Read values from the sensor:
      pressure = bme.readPressure();
      temperature = bme.readTemperature();
      humidity = bme.readHumidity ();
      altimeter = bme.readAltitude (1029.00); 

      Blynk.virtualWrite(V1, pressure/100.0F);     // write pressure to V1 value display widget
      Blynk.virtualWrite(V2, temperature);  // write temperature to V2 value display widget
      Blynk.virtualWrite(V3, humidity);    // write altimeter to V3 value display widget
      Blynk.virtualWrite(V4, altimeter);    // write altimeter to V4 value display widget
      
      //Print values to serial monitor:
      Serial.print(F("Pressure: "));
      Serial.print(pressure);
      Serial.print(" Mb");
      Serial.print("\t");
      Serial.print(("Temp: "));
      //delay(2000); //Update every 5 sec  
      Serial.print(temperature);
      Serial.print(" °C");
      Serial.print("\t");
      Serial.print("Humidity: ");
      Serial.print(humidity); // this should be adjusted to your local forcase
      Serial.println(" %"); 
      /*Serial.print("\t");
      Serial.print("Altimeter: ");
      Serial.print(altimeter); // this should be adjusted to your local forcase
      Serial.println(" %");  */  
    }

    void loop() {
      Blynk.run();
      timer.run();
        //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Prepare your HTTP POST request data
      String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&location=" + sensorLocation + "&value1=" + String(bme.readTemperature()) + "&value2=" + String(bme.readHumidity()) + "&value3=" + String(bme.readPressure()/100.0F) + "";
      Serial.print("httpRequestData: ");
      Serial.println(httpRequestData);

      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);

      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
      
    }
    }
