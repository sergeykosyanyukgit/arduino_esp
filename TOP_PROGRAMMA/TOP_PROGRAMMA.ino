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
        Serial.println(obj[String("sensor")].as<int>());


        String id = obj[String("_id")].as<String>();
        String poliv = obj[String("poliv")].as<String>();
        
        pinMode(obj[String("motor")].as<int>(), OUTPUT);
        if(poliv == "poliv") {
          digitalWrite(obj[String("motor")].as<int>(), HIGH);
        } else {
          digitalWrite(obj[String("motor")].as<int>(), LOW);
        }
        int p = analogRead(obj[String("sensor")].as<int>());
        p = map(p, 0, 4096, 100, 0);
        

        HTTPClient httpRes;
        httpRes.begin("https://arduino-esp.herokuapp.com/api/posts/esp-reload-hum/");
        httpRes.addHeader("Content-Type", "application/json");
        
        String output;
        StaticJsonDocument<200> jsonBuffer;
        JsonObject object = jsonBuffer.to<JsonObject>();
        object["sensorValue"] = p;
        object["id"] = id;
        serializeJson(object, output);
        Serial.println(output);
        
        int httpResponseCodeRes = httpRes.POST(output);
        if(httpResponseCodeRes>0){
          Serial.println(httpResponseCodeRes);   //Print return code
         }else{
          Serial.print("Error on sending request: ");
          Serial.println(httpResponseCodeRes);
         }
         httpRes.end();  //Free resources
      }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  delay(10000);
}
