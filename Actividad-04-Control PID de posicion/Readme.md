# Sistema Corporativo de Control PID de Posición: Integración LabVIEW - Arduino

Este repositorio contiene la arquitectura completa de hardware y software para el control de posición en lazo cerrado de un motor de corriente directa (DC). El sistema combina la potencia de cálculo y visualización de **LabVIEW** para el procesamiento del algoritmo PID y la adquisición de pulsos, junto con un microcontrolador **Arduino** operando como una etapa de potencia y driver esclavo mediante comunicación serial.

## 📐 Arquitectura General del Sistema

El sistema opera bajo un esquema descentralizado:
1. **LabVIEW (Maestro):** Se encarga de la lectura en cuadratura del encoder, calcula la posición actual en grados, computa el error respecto al *SetPoint*, ejecuta el lazo de control PID e inyecta la acción de control calculada (rango de `-255` a `255`) a través del puerto COM.
2. **Arduino (Esclavo):** Actúa puramente como receptor de comandos de velocidad y dirección (PWM), gestionando el puente H e implementando un retorno de datos (eco) para validar la integridad de la comunicación serial.

---

## 💻 1. Descripción del Código de LabVIEW (Diagrama de Bloques)

El instrumento virtual (`.vi`) está dividido en dos grandes secciones secuenciales y concurrentes:

### A. Lazo Principal de Control (Ciclo de Adquisición y PID)
* **Contador de Pulsos en Cuadratura:** Reutiliza la lógica de lectura digital por flancos mediante registros de desplazamiento (*Shift Registers*) para acumular los pulsos del encoder de manera bidireccional.
* **Conversión de Unidades:** El bloque toma el conteo entero de pulsos y aplica una constante de escalamiento para transformar la lectura a variables físicas del mundo real (`Grados Actuales`).
* **Algoritmo PID:** Compara la posición actual frente al control del panel frontal (*SetPoint en grados*). Utiliza las ganancias dinámicas `Kp`, `Ki` y `Kd` con un tiempo de muestreo fijo de `0.02 s` para mitigar el error exponencialmente.
* **Saturación y Formateo de String:** La salida del PID es multiplicada y mapeada mediante un selector lógico para restringir la señal de control en el rango industrial de PWM (`-255` a `255`). El valor entero se convierte a una cadena de texto (String) finalizada con un salto de línea (`\n`) listo para transmisión.

### B. Lazo de Comunicación Serial (VISA Loop)
* Inicializa el recurso serie configurado a una tasa de **115200 baudios**.
* En cada iteración, el bloque `VISA Write` envía el buffer de texto formateado con la acción del PWM.
* De manera asíncrona, el bloque `VISA Read` lee el canal serial para capturar el retorno (`read buffer`) que reenvía el Arduino, sirviendo como un indicador de telemetría y diagnóstico en tiempo real.

---

## 🔌 2. Descripción del Firmware de Arduino

El código cargado en la tarjeta de desarrollo procesa las cadenas recibidas e interactúa directamente con el hardware del motor.

### Características Clave Implementadas:
* **Comunicación por Buffer Asíncrona:** Lee caracter por caracter la trama de datos, ignorando los retornos de carro (`\r`) y procesando únicamente al encontrar un fin de línea (`\n`) para evitar retrasos que afecten el control dinámico.
* **Seguridad y Acotamiento de Rango:** Utiliza funciones de conversión de texto a entero (`atoi`) y aplica un candado de seguridad estricto que satura el software a un rango absoluto de $\pm255$ para salvaguardar la integridad de los transistores del puente H.
* **Tratamiento de Zona Muerta (Deadband):** Para prevenir oscilaciones de alta frecuencia provocadas por el ruido o la inercia del motor en estado de reposo, se define un umbral crítico de $\pm20$. Cualquier acción dentro de esta ventana se trunca a cero, optimizando el consumo energético y la vida útil mecánica.
* **Lógica del Puente H (`controlarMotor`):**
  * **Acción Positiva ($> 0$):** Energiza `IN1` en `HIGH` e `IN2` en `LOW` para giro horario, inyectando el valor absoluto al pin PWM.
  * **Acción Negativa ($< 0$):** Invierte los estados lógicos (`IN1` en `LOW`, `IN2` en `HIGH`) para invertir el sentido del torque.
  * **Freno Activo ($= 0$):** Apaga ambos canales y deshabilita el pin de PWM, deteniendo la planta de inmediato.

---

## 🛠️ Especificaciones de Hardware y Pines

* **Pin 9 (PWM):** Salida de modulación por ancho de pulso hacia la entrada de habilitación (Enable) del puente H.
* **Pines 7 y 8 (Dirección):** Señales digitales de control hacia las entradas lógicas `IN1` e `IN2` del driver de potencia (L298N o similar).
* **Configuración Serial:** 115200 bps, 8 bits de datos, sin paridad, 1 bit de parada.
