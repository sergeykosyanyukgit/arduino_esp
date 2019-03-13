#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Tone32.h>
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
        String payload = http.getString();
        payload.replace("[","");
        payload.replace("]","");
        Serial.println(payload);
        //Поиск дополнительных строк
        int k = payload.indexOf("},{");
        int g = -1;
        String payload_1;
        String payload_2;
        if(k > 0) {
          payload_1 = payload.substring(k);
          payload_1.remove(0, 2);
          g = payload_1.indexOf("},{");
          if(g > 0){
            payload_2 = payload_1.substring(g);
            payload_2.remove(0, 2);
          }
        }
        
        
        DynamicJsonDocument doc(1024); //Создаем JSON
        deserializeJson(doc, payload);
        JsonObject obj = doc.as<JsonObject>();

        //Буферизация
        String id = obj[String("_id")].as<String>();
        String poliv = obj[String("poliv")].as<String>();
        int motorPort = obj[String("motor")].as<int>();
        int sensorPort = obj[String("sensor")].as<int>();
        
        //Работа с мотором и датчиком
        pinMode(motorPort, OUTPUT);
        if(poliv == "poliv") {
          digitalWrite(motorPort, HIGH);
        } else {
          digitalWrite(motorPort, LOW);
        }
        int p = analogRead(sensorPort);
        p = map(p, 0, 4096, 100, 0);
        
        //Подключаемся, формируем ответ серверу
        HTTPClient httpRes;
        httpRes.begin("https://arduino-esp.herokuapp.com/api/posts/esp-reload-hum/");
        httpRes.addHeader("Content-Type", "application/json");

        //Формируем json, для ответа серверу
        String output;
        StaticJsonDocument<200> jsonBuffer;
        JsonObject object = jsonBuffer.to<JsonObject>();
        object["sensorValue"] = p;
        object["id"] = id;
        serializeJson(object, output);

        String final_output;
        
        //Вторая карточка
        String output_1;
        if(k > 0){   
          DynamicJsonDocument doc_1(1024); 
          deserializeJson(doc_1, payload_1);
          JsonObject obj_1 = doc_1.as<JsonObject>();
          
          String id_1 = obj_1[String("_id")].as<String>();
          String poliv_1 = obj_1[String("poliv")].as<String>();
          int motorPort_1 = obj_1[String("motor")].as<int>();
          int sensorPort_1 = obj_1[String("sensor")].as<int>();
          
          pinMode(motorPort_1, OUTPUT);
          if(poliv_1 == "poliv") {
            digitalWrite(motorPort_1, HIGH);
          } else {
            digitalWrite(motorPort_1, LOW);
          }
          int p_1 = analogRead(sensorPort_1);
          p_1 = map(p_1, 0, 4096, 100, 0);
          
          StaticJsonDocument<200> jsonBuffer_1;
          JsonObject object_1 = jsonBuffer_1.to<JsonObject>();
          object_1["sensorValue"] = p_1;
          object_1["id"] = id_1;
          serializeJson(object_1, output_1);
          //Конец второй карточки
          final_output = "[" + output + "," + output_1;
        } else {
          final_output = "[" + output;
        }


        //Третья карточка
        String output_2;
        if(g > 0){   
          DynamicJsonDocument doc_1(1024); 
          deserializeJson(doc_1, payload_2);
          JsonObject obj_1 = doc_1.as<JsonObject>();
          
          String id_1 = obj_1[String("_id")].as<String>();
          String poliv_1 = obj_1[String("poliv")].as<String>();
          int motorPort_1 = obj_1[String("motor")].as<int>();
          int sensorPort_1 = obj_1[String("sensor")].as<int>();
          
          pinMode(motorPort_1, OUTPUT);
          if(poliv_1 == "poliv") {
           // digitalWrite(motorPort_1, HIGH);
           tone(motorPort_1, 8000, 200);
          } else {
            //digitalWrite(motorPort_1, LOW);
          }
          int p_1 = analogRead(sensorPort_1);
          p_1 = map(p_1, 0, 4096, 100, 0);
          
          StaticJsonDocument<200> jsonBuffer_1;
          JsonObject object_1 = jsonBuffer_1.to<JsonObject>();
          object_1["sensorValue"] = p_1;
          object_1["id"] = id_1;
          serializeJson(object_1, output_2);
          //Конец второй карточки
          final_output = final_output + "," + output_2 + "]";
        } else {
          final_output = final_output + "]";
        }

        Serial.println(final_output);


        //Вдруг серв сгорел \.(-_-)./
        int httpResponseCodeRes = httpRes.POST(final_output);
        if(httpResponseCodeRes>0){
          Serial.println(httpResponseCodeRes);   //Print return code
         }else{
          Serial.print("Error on sending request: ");
          Serial.println(httpResponseCodeRes);
         }       
         httpRes.end();  //Все, отключаемся
      }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  delay(500);
}
