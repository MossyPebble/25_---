#include <ESP8266WiFi.h>
 
const char* ssid = "NLD";
const char* password = "qwertyuiop";

// pin 정의
int ledPin = D5;

int LMotorPin1 = D5;     // 좌측 모터 제어신호 1핀
int LMotorPin2 = D6;     // 좌측 모터 제어신호 2핀
int LMotorSpeedPin = D3;    // PWM 제어를 위한 핀

int RMotorPin1 = D7;     // 우측 모터 제어신호 1핀
int RMotorPin2 = D8;     // 우측 모터 제어신호 2핀
int RMotorSpeedPin = D4;    // PWM 제어를 위한 핀

// state 정의
int LMotorState = 0;   // 좌측 모터 상태, 0이면 OFF, 1이면 ON
int RMotorState = 0;   // 우측 모터 상태, 0이면 OFF, 1이면 ON

int LMotorDirction = 0;    // 좌측 모터 방향, 0이면 앞, 1이면 뒤
int RMotorDirction = 0;    // 우측 모터 방향, 0이면 앞, 1이면 뒤

int LMotorSpeed = 150;    // 좌측 모터 속도
int RMotorSpeed = 150;    // 우측 모터 속도

WiFiServer server(80);
 
void setup() {
	Serial.begin(9600);
	delay(10);
	
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);

    pinMode(LMotorPin1, OUTPUT);    // 제어 1번핀 출력모드 설정
    pinMode(LMotorPin2, OUTPUT);    // 제어 2번핀 출력모드 설정
    pinMode(LMotorSpeedPin, OUTPUT);   // PWM 제어핀 출력모드 설정

	pinMode(RMotorPin1, OUTPUT);    // 제어 1번핀 출력모드 설정
	pinMode(RMotorPin2, OUTPUT);    // 제어 2번핀 출력모드 설정
	pinMode(RMotorSpeedPin, OUTPUT);   // PWM 제어핀 출력모드 설정
	
	// Connect to WiFi network
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);
	
	WiFi.begin(ssid, password);
	
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	
	// Start the server
	server.begin();
	Serial.println("Server started");
	
	// Print the IP address
	Serial.print("Use this URL : ");
	Serial.print("http://");
	Serial.print(WiFi.localIP());
	Serial.println("/");
}
 
void loop() {

	// Check if a client has connected
	WiFiClient client = server.available();
	if (!client) {
		return;
	}
	
	// Wait until the client sends some data
	Serial.println("new client");
	while(!client.available()){
		delay(1);
	}
	
	// Read the first line of the request
	String request = client.readStringUntil('\r');
	Serial.println(request);
	client.flush();
	
	// 테스트용 LED API
	int value = LOW;
	if (request.indexOf("/LED=ON") != -1) {
		digitalWrite(ledPin, HIGH);
		value = HIGH;
	} 
	if (request.indexOf("/LED=OFF") != -1){
		digitalWrite(ledPin, LOW);
		value = LOW;
	}

	// 아래 코드들은 state만을 설정합니다.
	// 이후 모터 제어는 그 아래의 코드에서 state를 기반으로 직접적으로 제어합니다.
	// 좌측 모터 제어 API, /LMotor=ON, /LMotor=OFF, /LMotor=FORWARD, /LMotor=BACKWARD
    if (request.indexOf("/LMotor=ON") != -1) LMotorState = 1; // 좌측 모터 상태를 ON으로 설정
    if (request.indexOf("/LMotor=OFF") != -1) LMotorState = 0;   // 좌측 모터 상태를 OFF로 설정
	if (request.indexOf("/LMotor=FORWARD") != -1) LMotorDirction = 0;   // 좌측 모터 방향을 정방향으로 설정
	if (request.indexOf("/LMotor=BACKWARD") != -1) LMotorDirction = 1;   // 좌측 모터 방향을 역방향으로 설정

	// 우측 모터 제어 API, /RMotor=ON, /RMotor=OFF, /RMotor=FORWARD, /RMotor=BACKWARD
	if (request.indexOf("/RMotor=ON") != -1) RMotorState = 1; // 우측 모터 상태를 ON으로 설정
	if (request.indexOf("/RMotor=OFF") != -1) RMotorState = 0;   // 우측 모터 상태를 OFF로 설정
	if (request.indexOf("/RMotor=FORWARD") != -1) RMotorDirction = 0;   // 우측 모터 방향을 정방향으로 설정
	if (request.indexOf("/RMotor=BACKWARD") != -1) RMotorDirction = 1;   // 우측 모터 방향을 역방향으로 설정

	// 모터 속도 제어 API, /Speed=LOW, /Speed=HIGH
	if (request.indexOf("/Speed=LOW") != -1) {
		LMotorSpeed = 50;   // 좌측 모터 속도를 낮음으로 설정
		RMotorSpeed = 50;   // 우측 모터 속도를 낮음으로 설정
	}
	if (request.indexOf("/Speed=MEDIUM") != -1) {
		LMotorSpeed = 150;   // 좌측 모터 속도를 중간으로 설정
		RMotorSpeed = 150;   // 우측 모터 속도를 중간으로 설정
	}
	if (request.indexOf("/Speed=HIGH") != -1) {
		LMotorSpeed = 255;   // 좌측 모터 속도를 높음으로 설정
		RMotorSpeed = 255;   // 우측 모터 속도를 높음으로 설정
	}

	// 좌측 모터 제어
	if (LMotorState == 1){
		if (LMotorDirction == 0) {   // 좌측 모터가 정방향으로 회전
			digitalWrite(LMotorPin1, HIGH);  // 모터가 정방향으로 회전
			digitalWrite(LMotorPin2, LOW);
		} else {    // 좌측 모터가 역방향으로 회전
			digitalWrite(LMotorPin1, LOW);   // 모터가 역방향으로 회전
			digitalWrite(LMotorPin2, HIGH);
		}
		analogWrite(LMotorSpeedPin, LMotorSpeed);   // 모터 속도를 설정
	} else {
		analogWrite(LMotorSpeedPin, 0);   // 모터 속도를 0으로 설정
	}

	// 우측 모터 제어
	if (RMotorState == 1){
		if (RMotorDirction == 0) {   // 우측 모터가 정방향으로 회전
			digitalWrite(RMotorPin1, HIGH);  // 모터가 정방향으로 회전
			digitalWrite(RMotorPin2, LOW);
		} else {    // 우측 모터가 역방향으로 회전
			digitalWrite(RMotorPin1, LOW);   // 모터가 역방향으로 회전
			digitalWrite(RMotorPin2, HIGH);
		}
		analogWrite(RMotorSpeedPin, RMotorSpeed);   // 모터 속도를 설정
	} else {
		analogWrite(RMotorSpeedPin, 0);   // 모터 속도를 0으로 설정
	}
}