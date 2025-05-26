#include <ESP8266WiFi.h>
#include <DHT11.h>

// WiFi 정보
const char* ssid = "NLD";
const char* password = "qwertyuiop";

// 핀 정의
int ledPin = D5;

int LMotorPin1 = D5;     // 좌측 모터 제어신호 1핀
int LMotorPin2 = D6;
int LMotorSpeedPin = D3;

int RMotorPin1 = D7;
int RMotorPin2 = D8;
int RMotorSpeedPin = D4;

int LMotorState = 0;
int RMotorState = 0;
int LMotorDirction = 0;
int RMotorDirction = 0;
int LMotorSpeed = 150;
int RMotorSpeed = 150;

// DHT11 센서 핀
#define DHT11_PIN D2
DHT11 dht11(DHT11_PIN);

// WIFI 설정
IPAddress local_IP(192, 168, 43, 150);
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

WiFiServer server(80);

void setup() {
    Serial.begin(115200);
    delay(10);

    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("Static IP configuration failed!");
    }

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    pinMode(LMotorPin1, OUTPUT);
    pinMode(LMotorPin2, OUTPUT);
    pinMode(LMotorSpeedPin, OUTPUT);

    pinMode(RMotorPin1, OUTPUT);
    pinMode(RMotorPin2, OUTPUT);
    pinMode(RMotorSpeedPin, OUTPUT);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    server.begin();
    Serial.println("Server started");
    Serial.print("Use this URL : http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void handleDHT11Request(WiFiClient& client) {
    int temperature = 0;
    int humidity = 0;
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();

    if (result == 0) {
        client.print("{\"temperature\":");
        client.print(temperature);
        client.print(",\"humidity\":");
        client.print(humidity);
        client.println("}");
    } else {
        // 에러 발생 시 에러코드를 문자열로 전송
        client.print("{\"error\":\"");
        client.print(DHT11::getErrorString(result));
        client.println("\"}");
    }
}

void loop() {
    WiFiClient client = server.available();
    if (!client) return;

    Serial.println("new client");
    while(!client.available()){
        delay(1);
    }

    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // =========================
    // 온습도 측정 API
    // =========================
    if (request.indexOf("/DHT11=READ") != -1) {
        handleDHT11Request(client);
        return;
    }

    // LED 제어 API
    if (request.indexOf("/LED=ON") != -1) digitalWrite(ledPin, HIGH);
    if (request.indexOf("/LED=OFF") != -1) digitalWrite(ledPin, LOW);

    // 모터 상태/방향/속도 설정
    if (request.indexOf("/LMotor=ON") != -1) LMotorState = 1;
    if (request.indexOf("/LMotor=OFF") != -1) LMotorState = 0;
    if (request.indexOf("/LMotor=FORWARD") != -1) LMotorDirction = 0;
    if (request.indexOf("/LMotor=BACKWARD") != -1) LMotorDirction = 1;

    if (request.indexOf("/RMotor=ON") != -1) RMotorState = 1;
    if (request.indexOf("/RMotor=OFF") != -1) RMotorState = 0;
    if (request.indexOf("/RMotor=FORWARD") != -1) RMotorDirction = 0;
    if (request.indexOf("/RMotor=BACKWARD") != -1) RMotorDirction = 1;

    if (request.indexOf("/Speed=LOW") != -1) {
        LMotorSpeed = 50; RMotorSpeed = 50;
    }
    if (request.indexOf("/Speed=MEDIUM") != -1) {
        LMotorSpeed = 150; RMotorSpeed = 150;
    }
    if (request.indexOf("/Speed=HIGH") != -1) {
        LMotorSpeed = 255; RMotorSpeed = 255;
    }

    // 좌측 모터 제어
    if (LMotorState == 1){
        if (LMotorDirction == 0) {
            digitalWrite(LMotorPin1, HIGH);
            digitalWrite(LMotorPin2, LOW);
        } else {
            digitalWrite(LMotorPin1, LOW);
            digitalWrite(LMotorPin2, HIGH);
        }
        analogWrite(LMotorSpeedPin, LMotorSpeed);
    } else {
        analogWrite(LMotorSpeedPin, 0);
    }

    // 우측 모터 제어
    if (RMotorState == 1){
        if (RMotorDirction == 0) {
            digitalWrite(RMotorPin1, HIGH);
            digitalWrite(RMotorPin2, LOW);
        } else {
            digitalWrite(RMotorPin1, LOW);
            digitalWrite(RMotorPin2, HIGH);
        }
        analogWrite(RMotorSpeedPin, RMotorSpeed);
    } else {
        analogWrite(RMotorSpeedPin, 0);
    }

    // 간단한 응답페이지
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE HTML><html><body>OK</body></html>");
}

