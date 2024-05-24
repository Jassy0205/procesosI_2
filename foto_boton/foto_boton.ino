#include <WiFi.h>
#include "VideoStream.h"

#define CHANNEL 0
#define BUTTON_PIN 2  // Cambia el número del pin según el pin al que conectes el botón

// Use a pre-defined resolution, or choose to configure your own resolution
VideoSetting config(VIDEO_FHD, CAM_FPS, VIDEO_JPEG, 1);
// VideoSetting config(1072, 1072, CAM_FPS, VIDEO_JPEG, 1);  // Puedes comentar la línea anterior y descomentar esta para una resolución personalizada

char ssid[] = "L.J.J";   // Tu SSID de red
char pass[] = "jj@?78martes";       // Tu contraseña de red
int status = WL_IDLE_STATUS;
WiFiServer server(80);

uint32_t img_addr = 0;
uint32_t img_len = 0;
bool captureImage = false;

void sendImage(WiFiClient& client, uint8_t* buf, uint32_t len) {
    uint8_t header_buf[128] = {0};
    uint8_t header_len = snprintf((char*)header_buf, 128, "HTTP/1.1 200 OK\r\nContent-type: image/jpeg\r\nContent-Length: %lu\r\n\r\n", len);
    client.write(header_buf, header_len);
    client.write(buf, len);
    client.print("\r\n");
}

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT);  // Configura el pin del botón como entrada

    while (status != WL_CONNECTED) {
        status = WiFi.begin(ssid, pass);
        delay(5000);
    }
    Camera.configVideoChannel(CHANNEL, config);
    Camera.videoInit();
    Camera.channelBegin(CHANNEL);

    server.begin();
}

void loop() {
    if (digitalRead(BUTTON_PIN) == HIGH) {
        Serial.println("Capturar Imagen");
        captureImage = true;
        delay(500);  // Debouncing, ajusta según sea necesario
    }

    if (captureImage) {

        Camera.getImage(CHANNEL, &img_addr, &img_len);

        WiFiClient client = server.available();
        if (client) {
          while(client.connected()){
            Serial.println("new client connected");
            sendImage(client, (uint8_t*)img_addr, img_len);
            break;
          }
            client.stop();
            Serial.println("client disconnected");
        }

        captureImage = false;
    }

    //Serial.println("waiting for button press");
    //delay(1000);
}
