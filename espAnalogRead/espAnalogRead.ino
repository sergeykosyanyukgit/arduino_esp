void setup() {
  Serial.begin(115200);
  pinMode(23, OUTPUT);
}

void loop() {
  int p = analogRead(34);
  p = map(p, 0, 4096, 100, 0);
  Serial.println(p);
  delay(20);
  digitalWrite(23, HIGH);
  delay(200);
  digitalWrite(23, LOW);
  delay(200);
}
