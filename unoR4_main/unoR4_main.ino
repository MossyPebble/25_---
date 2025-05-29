#include <WiFiS3.h>
#include <DHT11.h>
#include <Servo.h>

// WiFi 설정
char ssid[] = "NLD";
char pass[] = "qwertyuiop";

WiFiServer server(80);
int status = WL_IDLE_STATUS;

// 핀 정의
int ledPin = 5;
int LMotorPin1 = 3;
int LMotorPin2 = 4;
int LMotorSpeedPin = 5;

int RMotorPin1 = 7;
int RMotorPin2 = 8;
int RMotorSpeedPin = 6;

int upperServoPin = 9;
int lowerServoPin = 10;

#define DHT11_PIN 2
DHT11 dht11(DHT11_PIN);

Servo upperServo;
Servo lowerServo;

int LMotorState = 0, RMotorState = 0;
int LMotorDir = 0, RMotorDir = 0;
int LMotorSpeed = 150, RMotorSpeed = 150;
int upperServoAngle = 90, lowerServoAngle = 90;

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(LMotorPin1, OUTPUT);
  pinMode(LMotorPin2, OUTPUT);
  pinMode(LMotorSpeedPin, OUTPUT);
  pinMode(RMotorPin1, OUTPUT);
  pinMode(RMotorPin2, OUTPUT);
  pinMode(RMotorSpeedPin, OUTPUT);

  upperServo.attach(upperServoPin);
  lowerServo.attach(lowerServoPin);

  WiFi.config(IPAddress(192, 168, 43, 150));
  Serial.println("Starting Access Point...");

  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("AP Start Failed");
    while (true);
  }

  delay(5000);
  server.begin();
  printWiFiStatus();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String currentLine = "", request = "";

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      request += c;
      if (c == '\n' && currentLine.length() == 0) break;
      if (c == '\n') currentLine = "";
      else if (c != '\r') currentLine += c;
    }
  }

  int temp = 0, hum = 0;
  dht11.readTemperatureHumidity(temp, hum);

  Serial.print("Request: ");
  Serial.println(request);


  if (request.indexOf("/LED=ON") != -1) digitalWrite(ledPin, HIGH);
  if (request.indexOf("/LED=OFF") != -1) digitalWrite(ledPin, LOW);

  if (request.indexOf("/angle=") != -1) {
    int angleStart = request.indexOf("/angle=") + 7;
    int angleEnd = request.indexOf(" ", angleStart);
    if (angleEnd == -1) angleEnd = request.length();
    int angle = request.substring(angleStart, angleEnd).toInt();
    if (angle >= 0 && angle <= 180) {
      upperServo.write(angle);
      lowerServo.write(angle);
    }

    Serial.println("angle turned");
  }

  if (request.indexOf("/LMotor=ON") != -1) LMotorState = 1;
  if (request.indexOf("/LMotor=OFF") != -1) LMotorState = 0;
  if (request.indexOf("/LMotor=FORWARD") != -1) LMotorDir = 0;
  if (request.indexOf("/LMotor=BACKWARD") != -1) LMotorDir = 1;

  if (request.indexOf("/RMotor=ON") != -1) RMotorState = 1;
  if (request.indexOf("/RMotor=OFF") != -1) RMotorState = 0;
  if (request.indexOf("/RMotor=FORWARD") != -1) RMotorDir = 0;
  if (request.indexOf("/RMotor=BACKWARD") != -1) RMotorDir = 1;

  if (request.indexOf("/Speed=LOW") != -1) LMotorSpeed = RMotorSpeed = 50;
  if (request.indexOf("/Speed=MEDIUM") != -1) LMotorSpeed = RMotorSpeed = 150;
  if (request.indexOf("/Speed=HIGH") != -1) LMotorSpeed = RMotorSpeed = 255;

  if (LMotorState) {
    digitalWrite(LMotorPin1, LMotorDir == 0 ? HIGH : LOW);
    digitalWrite(LMotorPin2, LMotorDir == 0 ? LOW : HIGH);
    analogWrite(LMotorSpeedPin, LMotorSpeed);
  } else analogWrite(LMotorSpeedPin, 0);

  if (RMotorState) {
    digitalWrite(RMotorPin1, RMotorDir == 0 ? HIGH : LOW);
    digitalWrite(RMotorPin2, RMotorDir == 0 ? LOW : HIGH);
    analogWrite(RMotorSpeedPin, RMotorSpeed);
  } else analogWrite(RMotorSpeedPin, 0);

  client.stop();
  Serial.println("Client disconnected");
}

void printWiFiStatus() {
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("IP: "); Serial.println(WiFi.localIP());
  Serial.println("Connect to the above IP in your browser.");
}
