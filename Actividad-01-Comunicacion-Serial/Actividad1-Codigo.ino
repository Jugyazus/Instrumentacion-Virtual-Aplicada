// ===============================
//      VARIABLES GLOBALES
// ===============================

volatile long Np = 0;      // Contador del encoder (sube o baja)
int lm35Pin = A0;          // Pin analógico LM35

// ===============================
//           SETUP
// ===============================

void setup() {

  Serial.begin(115200);     // Velocidad para LabVIEW

  pinMode(lm35Pin, INPUT);

  // Encoder en pines 2 y 3 (Arduino UNO interrupciones externas)
  attachInterrupt(digitalPinToInterrupt(2), CH_A, RISING);
  attachInterrupt(digitalPinToInterrupt(3), CH_B, RISING);
}

// ===============================
//            LOOP
// ===============================

void loop() {

  // ---- Leer LM35 ----
  int adc = analogRead(lm35Pin);

  // Convertir ADC a voltaje (suponiendo referencia 5V)
  float voltaje = adc * (5.0 / 1023.0);

  // ---- Enviar datos en formato <voltaje,encoder> ----
  Serial.print("<");
  Serial.print(voltaje, 2);   // 2 decimales
  Serial.print(",");
  Serial.print(Np);
  Serial.println(">");

  delay(200);   // 20 Hz de envío (suficiente para LabVIEW)
}

// ===============================
//      INTERRUPCIONES ENCODER
// ===============================

void CH_A() {
  if (digitalRead(3) == LOW)
    Np++;
  else
    Np--;
}

void CH_B() {
  if (digitalRead(2) == HIGH)
    Np++;
  else
    Np--;
}
