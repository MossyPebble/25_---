// serial 통신 테스트

void setup() {
    Serial.begin(9600);
    Serial.println("Serial test start");
    Serial.println("Serial test end");
}
  
void loop() {
    Serial.println("Serial test loop");
    delay(1000);
    Serial.println("Serial test loop end");
    delay(2000);
}