#include <WiFi.h>
#include <HTTPClient.h>
 
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
 
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;
   http.begin("https://mongo-express-vue-node.herokuapp.com/api/posts/");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "application/json");             //Specify content-type header
 
   int httpResponseCode = http.POST("{\"text\": \"Hello arduino day from DSTU\"}");   //Send the actual POST request
 
   if(httpResponseCode>0){
 
    Serial.println(httpResponseCode);   //Print return code
 
   }else{
 
    Serial.print("Error on sending request: ");
    Serial.println(httpResponseCode);
 
   }
 
   http.end();  //Free resources
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
 
  delay(5000);  //Send a request every 5 seconds
 
}
