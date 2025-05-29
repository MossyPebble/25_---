#include <WiFi.h>
#include <WebSocketsServer.h>
#include <driver/i2s.h>

#define I2S_SD 4
#define I2S_WS 15
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0

#define bufferLen 1024
int16_t sBuffer[bufferLen];

const char* ssid = "NLD";
const char* password = "qwertyuiop";

WebSocketsServer webSocket(81); // WebSocket 서버 포트

void setupMic() {
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = 0,
        .dma_buf_count = 10,
        .dma_buf_len = bufferLen,
        .use_apll = false
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD};

    i2s_set_pin(I2S_PORT, &pin_config);
}

void connectWiFi() {
    
    // 고정 IP 설정
    IPAddress local_IP(192, 168, 43, 157); // 원하는 고정 IP 주소
    IPAddress gateway(192, 168, 43, 1);    // 게이트웨이 주소 (일반적으로 라우터 IP)
    IPAddress subnet(255, 255, 255, 0);   // 서브넷 마스크
    IPAddress primaryDNS(8, 8, 8, 8);     // 기본 DNS 서버 (Google DNS)
    IPAddress secondaryDNS(8, 8, 4, 4);   // 보조 DNS 서버 (Google DNS)

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

void micTask(void* parameter) {
    setupMic();
    i2s_start(I2S_PORT);

    size_t bytesIn = 0;
    while (1) {
        esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
        if (result == ESP_OK) {
            webSocket.broadcastBIN((uint8_t*)sBuffer, bytesIn);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS); // 데이터 전송 주기 조정
    }
}

void setup() {
    Serial.begin(115200);
    connectWiFi();

    // WebSocket 서버 시작
    webSocket.begin();
    webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
        if (type == WStype_CONNECTED) {
            Serial.println("WebSocket client connected");
        } else if (type == WStype_DISCONNECTED) {
            Serial.println("WebSocket client disconnected");
        }
    });

    // I2S 태스크 실행
    xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
}

void loop() {
    webSocket.loop();
}