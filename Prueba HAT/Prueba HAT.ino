#include <SoftwareSerial.h>

// Define los pines RX y TX del Arduino para la comunicación con el módulo GSM
#define GSM_RX 0
#define GSM_TX 1

// Inicializa un objeto de tipo SoftwareSerial para la comunicación con el módulo GSM
SoftwareSerial gsmSerial(GSM_RX, GSM_TX);

void setup() {
  // Inicializar la comunicación serial con el módulo GSM
  Serial.begin(9600);
  gsmSerial.begin(9600);
}

void loop() {
  // Enviar el comando "AT" al módulo GSM
  gsmSerial.println("AT");
  delay(1000); // Esperar 1 segundo antes de enviar el siguiente comando

  // Leer la respuesta del módulo GSM y mostrarla en el puerto serie
  while (gsmSerial.available()) {
    char c = gsmSerial.read();
    Serial.write(c);
    Serial.println("Disponible");
  }
  Serial.println("H");
}

