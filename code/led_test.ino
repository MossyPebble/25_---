// 보드에 있는 led를 깜빡이게 하는 test 코드

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
}
  
void loop() {
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);
}