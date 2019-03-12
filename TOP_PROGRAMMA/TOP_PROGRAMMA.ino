#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "ASUS_X00PD";
const char* password =  "Froiji22";

void setup() {
  Serial.begin(115200);
  delay(4000);   
 
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(500);
    Serial.println("Connecting..");
  }
 
  Serial.print("Connected to the WiFi network with IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    http.begin("https://arduino-esp.herokuapp.com/api/posts/esp/"); //HTTP
    int httpCode = http.GET();
    if(httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if(httpCode == HTTP_CODE_OK) {
        DynamicJsonDocument doc(1024); //Создаем JSON
        String payload = http.getString();
        payload.replace("[","");
        payload.replace("]","");
        Serial.println(payload);
        deserializeJson(doc, payload);
        JsonObject obj = doc.as<JsonObject>();
        Serial.println(obj[String("_id")].as<String>());
      }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  delay(10000);
}
