#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <driver/i2s.h>

#define I2S_SD 4
#define I2S_WS 15
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0

#define bufferLen 1024
int16_t sBuffer[bufferLen];

const char *ssid = "NLD";
const char *password = "qwertyuiop";

WebSocketsServer webSocket(81); // WebSocket 서버 포트

int usingMicOrSpeaker = 0; // 0: 마이크, 1: 스피커

// 스피커 설치 및 핀 설정
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

// 마이크 설치 및 핀 설정
void setupMic()
{
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = 0,
        .dma_buf_count = 10,
        .dma_buf_len = bufferLen,
        .use_apll = false};

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD};

    i2s_set_pin(I2S_PORT, &pin_config);
}

void connectWiFi()
{

    // 고정 IP 설정
    IPAddress local_IP(192, 168, 43, 210); // 원하는 고정 IP 주소
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

void micTask(void *parameter)
{
    setupMic();
    i2s_start(I2S_PORT);

    size_t bytesIn = 0;
    while (1)
    {
        esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
        if (result == ESP_OK && usingMicOrSpeaker == 0)
        {
            webSocket.broadcastBIN((uint8_t *)sBuffer, bytesIn);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS); // 데이터 전송 주기 조정
    }
}

void setup()
{
    // 시리얼 통신 초기화
    Serial.begin(115200);
    Serial.println("Starting up");

    // 마이크 설치 및 핀 설정
    Serial.println("Starting I2S Output");
    setupSpeaker();

    // WiFi 연결
    connectWiFi();

    // WebSocket 서버 시작
    webSocket.begin();
    webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        static unsigned long lastTime = 0;
        unsigned long now;

        switch (type) {
            case WStype_CONNECTED:
                Serial.printf("Client %u connected from %s\n", num, webSocket.remoteIP(num).toString().c_str());
                break;
            case WStype_DISCONNECTED:
                Serial.printf("Client %u disconnected\n", num);
                break;
            case WStype_BIN:
                if (usingMicOrSpeaker == 0)
                {

                    // 마이크가 활성화된 경우 데이터 수신 무시
                    Serial.printf("Client %u sent %u bytes, but mic is active\n", num, length);
                    return;
                }
                
                now = millis();
                Serial.printf("[%lu] Client %u sent %u bytes\n", now, num, length);
                lastTime = now;

                // PCM 처리
                size_t written;
                i2s_write(I2S_NUM_1, payload, length, &written, portMAX_DELAY);
                break;
            case WStype_TEXT:

                // WebSocket 텍스트 메시지 처리
                String message = String((char *)payload);
                Serial.printf("Received message: %s\n", message.c_str());

                if (message == "USE_MIC")
                {

                    // 마이크 활성화
                    setupMic();
                    usingMicOrSpeaker = 0; 
                    Serial.println("Switched to MIC mode");
                }
                else if (message == "USE_SPEAKER")
                {

                    // 스피커 활성화
                    setupSpeaker();
                    usingMicOrSpeaker = 1; 
                    Serial.println("Switched to SPEAKER mode");
                }
                break;
            } });

    // 마이크 task 실행
    xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
}

void loop()
{
    webSocket.loop();
}