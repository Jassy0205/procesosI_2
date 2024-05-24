#include "VideoStream.h"
#include "AmebaFatFS.h"
#include <WiFi.h>

#define CHANNEL 0
#define FILENAME "Rosa_Perez"
#define buttonPin 2

// Configuración de la red WiFi
char ssid[] = "L.J.J";
char pass[] = "jj@?78martes";
int status = WL_IDLE_STATUS;

uint32_t img_addr = 0;  // Variable para almacenar la dirección de la imagen capturada
uint32_t img_len = 0; // Variable para almacenar la longitud de la imagen capturada
uint32_t count = 0;   // Contador para el número de imágenes guardadas

int ledState=LOW;
int buttonState;
int lastButtonState = LOW;

// Configuración del servidor
const char* serverName = "192.168.43.236";
const int serverPort = 5000;
const char* uploadPath = "/upload";

// Use a pre-defined resolution, or choose to configure your own resolution
VideoSetting config(VIDEO_FHD, CAM_FPS, VIDEO_JPEG, 1);

unsigned long lastDebounceTime = 0;  // Tiempo de la última lectura del botón
unsigned long debounceDelay = 50;   // Retardo para el rebote del botón

AmebaFatFS fs;   // Objeto del sistema de archivos
WiFiClient client;  // Objeto del cliente WiFi

void setup() {
    pinMode(buttonPin, INPUT);
    Serial.begin(115200);

    while (status != WL_CONNECTED) {
        status = WiFi.begin(ssid, pass);
        delay(5000);
    }

    Serial.println(" Conectado a la red WiFi");

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

    if ((millis() - lastDebounceTime) > debounceDelay) { // Verifica si ha pasado el tiempo de rebote
    // Si han pasado más de debounceDelay milisegundos desde el último cambio de botón
    // significa que el cambio es debido a una presión real y no por rebote
    if (reading != buttonState) {
      buttonState = reading;

      // Si el botón fue presionado, cambia el estado del LED
      if (buttonState == HIGH) // Si el botón fue presionado
      {
        String filePath = String(fs.getRootPath()) + String(FILENAME) + String(count) + String(".jpg");
        File file = fs.open(filePath.c_str());
        Camera.getImage(CHANNEL, &img_addr, &img_len);
        file.write((uint8_t*)img_addr, img_len);  // Escribe la imagen en el archivo
        printf("Saved %s\r\n", file.name());

        // Enviar la imagen al servidor
        if ((WiFi.status() == WL_CONNECTED)) {
          if (client.connect(serverName, serverPort)) {
            // Construir la solicitud HTTP POST
            String head = "--boundary\r\nContent-Disposition: form-data; name=\"file\"; filename=\"" + String(FILENAME) + String(count) + ".jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
            String tail = "\r\n--boundary--\r\n";

            // Calcular la longitud del contenido
            int contentLength = head.length() + img_len + tail.length();

            // Enviar las cabeceras de la solicitud HTTP POST
            client.print(String("POST ") + uploadPath + " HTTP/1.1\r\n");
            client.print(String("Host: ") + serverName + "\r\n");
            client.print("Content-Type: multipart/form-data; boundary=boundary\r\n");
            client.print("Content-Length: " + String(contentLength) + "\r\n");
            client.print("\r\n");

            // Enviar el contenido de la imagen
            client.print(head);
            client.write((uint8_t*)img_addr, img_len);
            client.print(tail);

            // Leer la respuesta del servidor
            while (client.connected()) {
                String line = client.readStringUntil('\n');
                if (line == "\r") {
                    break;
                }
            }

            // Imprimir la respuesta del servidor
            String response = client.readString();
            Serial.println("Respuesta del servidor:");
            Serial.println(response);

            // Cerrar la conexión
            client.stop();
          } else {
            Serial.println("Conexión al servidor fallida");
          }
        } else {
          Serial.println("Error de conexión WiFi");
        }

        file.close();
        count++;
        Serial.println("Final");
      }
    }
  }

  // Guarda el estado actual como el último estado, para la próxima iteración
  lastButtonState = reading;  
}