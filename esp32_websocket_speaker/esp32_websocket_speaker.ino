#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <driver/i2s.h>

// WiFi 설정
const char *ssid = "NLD";
const char *password = "qwertyuiop";

int connected = false;

// WebSocket 서버 포트
WebSocketsServer webSocket(81);

// I²S 설정
void setupSpeaker()
{
    const i2s_config_t i2sConfig = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 16000,                        
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false};

    const i2s_pin_config_t pinConfig = {
        .bck_io_num = GPIO_NUM_27,
        .ws_io_num = GPIO_NUM_14,
        .data_out_num = GPIO_NUM_26,
        .data_in_num = -1};

    // I²S 초기화
    i2s_driver_install(I2S_NUM_1, &i2sConfig, 0, NULL);
    i2s_set_pin(I2S_NUM_1, &pinConfig);
}

void connectWiFi()
{
    // 고정 IP 설정
    IPAddress local_IP(192, 168, 43, 158); // 원하는 고정 IP 주소
    IPAddress gateway(192, 168, 43, 1);    // 게이트웨이 주소 (일반적으로 라우터 IP)
    IPAddress subnet(255, 255, 255, 0);    // 서브넷 마스크
    IPAddress primaryDNS(8, 8, 8, 8);      // 기본 DNS 서버 (Google DNS)
    IPAddress secondaryDNS(8, 8, 4, 4);    // 보조 DNS 서버 (Google DNS)

    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
    {
        Serial.println("Static IP configuration failed!");
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting up");

    // I²S 출력 초기화
    Serial.println("Starting I2S Output");
    setupSpeaker();

    // WiFi 연결
    connectWiFi();

    // WebSocket 서버 시작
    webSocket.begin();
    webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            connected = true;
            Serial.printf("Client %u connected from %s\n", num, webSocket.remoteIP(num).toString().c_str());
            break;
        case WStype_DISCONNECTED:{
            connected = false;
            Serial.printf("Client %u disconnected\n", num);

            // 무음 버퍼 전송 (예: 1024 샘플 = 약 64ms @16kHz)
            uint8_t silence[2048] = {0}; // 1024 * 2바이트 (16bit mono)
            size_t written;
            for (int i = 0; i < 100; i++) {
                i2s_write(I2S_NUM_1, silence, sizeof(silence), &written, portMAX_DELAY);
            }
            break;}
        case WStype_BIN:{
            static unsigned long lastTime = 0;
            unsigned long now = millis();
            Serial.printf("[%lu] Client %u sent %u bytes\n", now, num, length);
            lastTime = now;

            // PCM 처리
            if (connected){
              size_t written;
              i2s_write(I2S_NUM_1, payload, length, &written, portMAX_DELAY);
            }
            break;
            }
    }
});

}

void loop()
{
    webSocket.loop();
}