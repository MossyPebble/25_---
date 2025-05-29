#include <ESP8266WiFi.h>
#include <DHT11.h>
#include <Servo.h>

Servo upperServo;   // 상단 서보 모터 객체
Servo lowerServo;   // 하단 서보 모터 객체

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

int upperServoPin = D9;   // 상단 서보 모터 핀
int lowerServoPin = D10;   // 하단 서보 모터 핀

// state 정의
int LMotorState = 0;   // 좌측 모터 상태, 0이면 OFF, 1이면 ON
int RMotorState = 0;   // 우측 모터 상태, 0이면 OFF, 1이면 ON

int LMotorDirction = 0;    // 좌측 모터 방향, 0이면 앞, 1이면 뒤
int RMotorDirction = 0;    // 우측 모터 방향, 0이면 앞, 1이면 뒤

int LMotorSpeed = 150;    // 좌측 모터 속도
int RMotorSpeed = 150;    // 우측 모터 속도

// servo 모터 state 정의
int upperServoAngle = 0;
int lowerServoAngle = 0;

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

	// 서보 모터 초기화
	upperServo.attach(upperServoPin);   // 상단 서보 모터 핀 연결
	lowerServo.attach(lowerServoPin);   // 하단 서보 모터 핀 연결

	// 고정 IP 설정
	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
		Serial.println("Static IP configuration failed!");
	}
	
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

	int temperature = 0;
	int humidity = 0;
	int result = dht11.readTemperatureHumidity(temperature, humidity);

	// 온도, 습도 측정 API
	if (request.indexOf("/temperature") != -1) client.println(temperature);
	if (request.indexOf("/humidity") != -1) client.println(humidity);

	// servo 모터 제어 API
	if (request.indexOf("/angle=") != -1)
	{
		int angleStart = request.indexOf("/angle=") + 7; // "/angle=" 뒤의 숫자 시작 위치
		int angleEnd = request.indexOf(" ", angleStart); // 공백 또는 요청 끝까지 숫자 추출
		if (angleEnd == -1)
			angleEnd = request.length(); // 공백이 없으면 요청 끝까지

		String angleStr = request.substring(angleStart, angleEnd); // 숫자 부분 추출
		int angle = angleStr.toInt();							   // 문자열을 정수로 변환

		각도 유효성 검사 (0~180도 범위)
		if (angle >= 0 && angle <= 180)
		{
			upperServoAngle = angle; // 상단 서보 모터 각도 설정
			lowerServoAngle = angle; // 하단 서보 모터 각도 설정 (필요 시 분리 가능)
			Serial.print("Servo angle set to: ");
			Serial.println(angle);

			// 서보 모터 제어 코드 추가
			upperServo.write(upperServoAngle); // 상단 서보 모터 회전
			lowerServo.write(lowerServoAngle); // 하단 서보 모터 회전
		}
		else
		{
			Serial.println("Invalid angle value");
		}
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