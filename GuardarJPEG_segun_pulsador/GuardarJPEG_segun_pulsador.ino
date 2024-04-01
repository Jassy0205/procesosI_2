#include "VideoStream.h"
#include "AmebaFatFS.h"

#define CHANNEL 0
#define FILENAME "image"
#define buttonPin 2

// Use a pre-defined resolution, or choose to configure your own resolution
VideoSetting config(VIDEO_FHD, CAM_FPS, VIDEO_JPEG, 1);

uint32_t img_addr = 0;
uint32_t img_len = 0;
uint32_t count = 0;

int ledState=LOW;
int buttonState;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;   

AmebaFatFS fs;

void setup() {
    pinMode(buttonPin, INPUT);
    Serial.begin(115200);

    Camera.configVideoChannel(CHANNEL, config);
    
    Camera.videoInit();
    Camera.channelBegin(CHANNEL);

    fs.begin();
}

void loop() {
    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState) {
      // reinicia el temporizador de rebote
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
    // Si han pasado más de debounceDelay milisegundos desde el último cambio de botón
    // significa que el cambio es debido a una presión real y no por rebote
    if (reading != buttonState) {
      buttonState = reading;

      // Si el botón fue presionado, cambia el estado del LED
      if (buttonState == HIGH) {
        File file = fs.open(String(fs.getRootPath()) + String(FILENAME) + String(count) + String(".jpg"));
        Camera.getImage(CHANNEL, &img_addr, &img_len);
        file.write((uint8_t*)img_addr, img_len);
        printf("Saved %s\r\n", file.name());
        file.close();
        count++;
      }
    }
  }

  // Guarda el estado actual como el último estado, para la próxima iteración
  lastButtonState = reading;
    
    
}
