# Contador de Pulsos de Encoder Bidireccional

Este módulo desarrollado en LabVIEW realiza la lectura y el procesamiento de las señales provenientes de un encoder de cuadratura acoplado a un motor DC. El sistema es capaz de determinar la dirección del giro y llevar un conteo acumulado (suma y resta de pulsos) en tiempo real.

## 🚀 Funcionamiento del Código

El diagrama a bloques implementa una arquitectura basada en un ciclo `While Loop` con **Shift Registers (Registros de Desplazamiento)** para mantener el estado del conteo y las señales anteriores:

1. **Adquisición de Datos (DAQ Assistant):** Se utilizan dos instancias de DAQ Assistant para leer los canales físicos (Canal A y Canal B) del encoder incremental.
2. **Detección de Flancos y Cuadratura:** Mediante lógica booleana, el código compara el estado actual de los canales con su estado anterior para detectar los flancos de subida/bajada.
3. **Lógica de Conteo (Case Structure):**
   * Cuando se detecta un pulso válido, se activa la estructura de caso (`True`).
   * Un bloque de selección (`Select`) determina el sentido de giro: suma `+1` si el motor gira en sentido horario o resta `-1` si gira en sentido antihorario.
4. **Acumulación:** El valor resultante se suma al valor acumulado en el Shift Register numérico y se despliega en tiempo real en el indicador `Numeric`.

## 🛠️ Requisitos e Instrumentación

* **Software:** LabVIEW 2020 o superior.
* **Drivers:** NI-DAQmx instalados para el correcto funcionamiento de los bloques *DAQ Assistant*.
* **Hardware sugerido:** Tarjeta de adquisición de datos (DAQ) de National Instruments o sistema de desarrollo compatible.
