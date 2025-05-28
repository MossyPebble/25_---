#include <Servo.h>
#include <WiFi.h>

// 두 개의 서보 객체 생성 (수평, 수직)
Servo servoH;  // 좌우(수평) 움직임
Servo servoV;  // 상하(수직) 움직임

const int servoHPin = 4; // 수평 서보 핀
const int servoVPin = 5; // 수직 서보 핀

volatile int posH = 90; // 초기 각도: 중앙 (0~180)
volatile int posV = 90; // 초기 각도: 중앙 (0~180)

WiFiServer server(80);

// 방향 제어 상태
volatile bool moveLeft = false;
volatile bool moveRight = false;
volatile bool moveUp = false;
volatile bool moveDown = false;

unsigned long lastMoveTime = 0;
const unsigned long moveInterval = 20; // ms, 각도 1도씩 조절 간격

void setup() {
  Serial.begin(115200);
  servoH.attach(servoHPin);
  servoV.attach(servoVPin);
  servoH.write(posH);
  servoV.write(posV);

  WiFi.mode(WIFI_STA);
  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    client.flush();

    // 좌우 제어 명령 파싱 (수평)
    if (req.indexOf("/LEFT_START") != -1) {
      moveLeft = true;
    }
    if (req.indexOf("/LEFT_STOP") != -1) {
      moveLeft = false;
    }
    if (req.indexOf("/RIGHT_START") != -1) {
      moveRight = true;
    }
    if (req.indexOf("/RIGHT_STOP") != -1) {
      moveRight = false;
    }

    // 상하 제어 명령 파싱 (수직)
    if (req.indexOf("/UP_START") != -1) {
      moveUp = true;
    }
    if (req.indexOf("/UP_STOP") != -1) {
      moveUp = false;
    }
    if (req.indexOf("/DOWN_START") != -1) {
      moveDown = true;
    }
    if (req.indexOf("/DOWN_STOP") != -1) {
      moveDown = false;
    }

    // 응답
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Dual-servo control OK");
  }

  // 주기적으로 각도 업데이트
  unsigned long now = millis();
  if (now - lastMoveTime > moveInterval) {
    lastMoveTime = now;
    // 좌우(수평)
    if (moveLeft && posH > 0) {
      posH -= 1;
      servoH.write(posH);
    }
    if (moveRight && posH < 180) {
      posH += 1;
      servoH.write(posH);
    }
    // 상하(수직)
    if (moveUp && posV < 180) {
      posV += 1;
      servoV.write(posV);
    }
    if (moveDown && posV > 0) {
      posV -= 1;
      servoV.write(posV);
    }
  }
}
