#include <Homie.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

//Settings
#define DHTTYPE DHT22
#define PIN_SENSOR D4

unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 20000;              // interval at which to read sensor

DHT dht(PIN_SENSOR, DHTTYPE, 11); // 11 works fine for ESP8266
float humidity, temp_f;  // Values read from sensor

HomieNode temperatureNode("temperature", "temperature");
HomieNode humidityNode("humidity", "humidity");

void loopHandler() {

    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      humidity = dht.readHumidity();          // Read humidity (percent)
      temp_f = dht.readTemperature();     // Read temperature as Fahrenheit
      // Check if any reads failed and exit early (to try again).
      if (isnan(humidity) || isnan(temp_f)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }

      Homie.getLogger() << "Temperature " << temp_f << " and Humidity " << humidity << endl;
      temperatureNode.setProperty("temperature").send(String(temp_f));
      humidityNode.setProperty("humidity").send(String(humidity));
    }
}


void setup() {
//Homie.disableLogging(); // before Homie.setup()

  Serial.begin(115200);
  Serial << endl << endl;

  Homie_setFirmware("climate-sensor", "1.0.0");
  Homie.setLoopFunction(loopHandler);
  Homie.setResetTrigger(1, LOW, 2000); // before Homie.setup()

  temperatureNode.advertise("temperature");
  humidityNode.advertise("humidity");

  dht.begin();

  Homie.setup();
  
}

void loop() {
  Homie.loop();
}
