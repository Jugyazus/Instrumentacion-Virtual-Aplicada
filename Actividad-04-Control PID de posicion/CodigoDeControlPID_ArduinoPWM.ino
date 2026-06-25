const int pwmPin = 9;
const int in1 = 7;
const int in2 = 8;

char buffer[16];
byte index = 0;

void setup() {
  Serial.begin(115200);

  pinMode(pwmPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    // Ignorar retorno de carro (\r)
    if (c == '\r') continue;

    // Detectar fin de línea (\n)
    if (c == '\n') {
      buffer[index] = '\0';

      int value = atoi(buffer);  // convertir a entero

      // 🔁 ECO (regresa el valor recibido)
      Serial.println(value);

      // 🔒 Limitar rango por seguridad
      if (value > 255) value = 255;
      if (value < -255) value = -255;

      controlarMotor(value);

      index = 0; // reset buffer
    }
    else {
      if (index < sizeof(buffer) - 1) {
        buffer[index++] = c;
      }
    }
  }
}

void controlarMotor(int value) {

  // 🔥 Zona muerta (opcional, mejora estabilidad)
  if (abs(value) < 20) value = 0;

  if (value > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(pwmPin, value);
  }
  else if (value < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(pwmPin, -value);
  }
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(pwmPin, 0);
  }
}