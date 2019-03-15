//Подключаем библиотеки
#include <WiFi.h> //Wifi
#include <HTTPClient.h> //Библиотека создания запросов к серверу
#include <ArduinoJson.h> //Библиотека приобразования строк в json объект и обратно
#include <Tone32.h> //Для пищалки
const char* ssid = "ASUS_X00PD"; //Логин и пароль от точки доступа
const char* password =  "Froiji22";

void setup() {
  Serial.begin(115200);//Скорость передачи данных на пк
  delay(4000);   
 
  WiFi.begin(ssid, password); //Инициализируем wifi
 
  while (WiFi.status() != WL_CONNECTED) { //Ожидаем подключения к точке доступа
    delay(500);
    Serial.println("Connecting..");
  }
 
  Serial.print("Connected to the WiFi network with IP: ");
  Serial.println(WiFi.localIP()); //Выводим ip адрес
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){//Если еще подключены к wifi
    HTTPClient http;//Создаем новый http клиент
    http.begin("https://arduino-esp.herokuapp.com/api/posts/esp/"); //Устанавливаем адрес сервера
    int httpCode = http.GET(); //Производим GET запрос к серверу и код ответа сохраняем в переменную
    if(httpCode > 0) { //Если вообще что то получили
      Serial.printf("[HTTP] GET... code: %d\n", httpCode); //Печатаем код ответа (200 или 201 - значит все норм, 404 - недоступен сервер)
      if(httpCode == HTTP_CODE_OK) { //Если пришел нормальный ответ
        String payload = http.getString(); //Получаем из ответа сервера строку (на самом деле это массив Json - объектов)
        payload.replace("[",""); //Чтобы нормально парсить его, удаляем ненужные символы из строки
        payload.replace("]","");
        Serial.println(payload); //Печатаем то что пришло с сервера полностью
        //Поиск дополнительных строк
        int k = payload.indexOf("},{");//Ищем дополнительные карточки в ответе
        int g = -1;
        String payload_1;
        String payload_2;
        if(k > 0) { //если есть вторая доп карточка
          payload_1 = payload.substring(k);//Вытаскиваем ее
          payload_1.remove(0, 2);//Удаляем ненужный мусор из строки
          g = payload_1.indexOf("},{");//ищем 3ю карточку
          if(g > 0){//если она есть
            payload_2 = payload_1.substring(g); //Вытаскиваем ее
            payload_2.remove(0, 2);//Удаляем ненужный мусор из строки
          }
        }
        
        
        DynamicJsonDocument doc(1024); //Создаем JSON
        deserializeJson(doc, payload);
        JsonObject obj = doc.as<JsonObject>();

        //Буферизация нужных нам элементов первой карточки
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
        
        //Подключаемся, формируем ответ серверу (датчики)
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
        //ВЫВОДИМ ИТОГОВЫЙ ОТВЕТ СЕРВЕРУ
        Serial.println(final_output);


        //Вдруг серв сгорел \.(-_-)./
        int httpResponseCodeRes = httpRes.POST(final_output); //Делаем Post запрос, передавая данные с сенсоров
        if(httpResponseCodeRes>0){
          Serial.println(httpResponseCodeRes);   //если вообще передали
         }else{
          Serial.print("Error on sending request: ");
          Serial.println(httpResponseCodeRes);
         }       
         httpRes.end();  //Все, отключаемся
      }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();//Вырубаем http 
  }
  delay(500);
}
