/*  This example uses the camera to capture a JPEG image,
    and saves the image to SD Card.

 Example guide:
 https://www.amebaiot.com/en/amebapro2-arduino-video-jpeg-sdcard/
*/
#include "VideoStream.h"
#include "AmebaFatFS.h"

#define CHANNEL 0
#define BUTTON_PIN 2  // Cambia el número del pin según el pin al que conectes el botón
#define FILENAME "image.jpg"
#define FOLDER_NAME "imagenes_AMB82"  // Nombre de la carpeta en tu computadora

VideoSetting config(VIDEO_FHD, CAM_FPS, VIDEO_JPEG, 1);

int numero = 1;
uint32_t img_addr = 0;
uint32_t img_len = 0;
bool captureImage = false;

AmebaFatFS fs;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);  // Configura el pin del botón como entrada

  Camera.configVideoChannel(CHANNEL, config);
  Camera.videoInit();
  Camera.channelBegin(CHANNEL);

  fs.begin();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    
    Serial.println("Capturar Imagen");
    captureImage = true;
    delay(500);  // Debouncing, ajusta según sea necesario
  }

  if (captureImage) {

    FILENAME = "image_"+(String) numero+".jpg"
    File file = fs.open(String(fs.getRootPath()) + String(FILENAME));

    delay(1000);
    Camera.getImage(CHANNEL, &img_addr, &img_len);
    file.write((uint8_t*)img_addr, img_len);

    file.close();
    fs.end();

    numero++; 
    captureImage = false;
  }
}
