# Nodo de Control de Posición en la Nube vía Firebase (IoT)

Este repositorio contiene el ecosistema de hardware y software desarrollado para el control de posición de una planta en lazo cerrado con conectividad inalámbrica a Internet. El sistema utiliza un microcontrolador **ESP32** para la adquisición y manipulación física de la planta, interactuando en tiempo real con una interfaz HMI supervisada en **LabVIEW** mediante transacciones en la nube empleando **Firebase Realtime Database**.

---

## 📐 Arquitectura de Comunicación General

El sistema elimina por completo las conexiones cableadas tradicionales a la computadora de supervisión, estructurando una red ciberfísica basada en el protocolo HTTP REST:
1. **Planta / ESP32:** Se conecta de forma inalámbrica a la red Wi-Fi local. Lee de forma continua la posición real y el indicador de actividad (*Heartbeat*), subiendo estos datos a Firebase mediante un método `PUT/POST`. De forma asíncrona, lee las ganancias de control y el *SetPoint* deseado.
2. **Firebase Realtime Database (Nube):** Actúa como el puente o base de datos en tiempo real donde residen dos nodos de datos estructurados en formato JSON: `Estado.json` y `Variables.json`.
3. **LabVIEW HMI (Supervisor):** Actúa como el centro de control maestro del operario, consumiendo y publicando datos de manera remota para modificar el comportamiento físico del motor.

---

## 💻 1. Descripción del Código de LabVIEW (image_218d20.png)

El diagrama a bloques ejecuta una arquitectura concurrente y temporizada (muestreo de 100 ms) diseñada para la gestión segura de variables a través de peticiones web:

### A. Canal de Escritura: Configuración de Parámetros (HTTP PUT)
* **Agrupación de Ganancias:** Consolida en un clúster las ganancias dinámicas del controlador (`Kp`, `Ki`, `Kd`) junto con la referencia espacial de destino (`SetPoint`) configuradas por el usuario en el panel frontal.
* **Serialización JSON:** El bloque `Flatten To JSON` convierte de manera inmediata el clúster en una cadena de texto plana con formato de objeto.
* **Inyección en la Nube:** A través de un cliente web, se realiza un disparo continuo de peticiones `HTTP PUT` dirigidas exclusivamente a la URL de control: `https://json-685a3-default-rtdb.firebaseio.com/Variables.json`. Esto sobreescribe los valores de sintonización en Firebase sin generar retardos ni duplicar datos.

### B. Canal de Lectura: Monitoreo Remoto (HTTP GET)
* **Extracción de Estado:** En cada iteración, el instrumento ejecuta un método `HTTP GET` apuntando a la URL: `https://json-685a3-default-rtdb.firebaseio.com/Estado.json` para descargar las variables reportadas por el ESP32.
* **Parseo Estructurado:** La cadena JSON recibida es inyectada en el bloque `Unflatten From JSON`, el cual extrae los valores crudos basándose en un clúster patrón que contiene las variables `Posicion` y `Heartbeat`.
* **Telemetría y Despliegue de Error:** El sistema despaqueta la posición real y calcula matemáticamente el error instantáneo del sistema mediante la resta respecto al *SetPoint* actual (`SetPoint - Posicion`), actualizando la **Gráfica del Error** en el panel frontal.

### C. Sistema de Seguridad: Detección de Falla (Fuera de Línea)
* **Monitoreo por Heartbeat:** El lazo principal cuenta con una lógica de seguridad por software conectada a un registro de desplazamiento (*Shift Register*) que incrementa secuencialmente un contador interno.
* **Evaluación de Pérdida de Conexión:** Compara el valor actual de la variable `Heartbeat` remota (enviada por el ESP32) contra el contador acumulado de LabVIEW. Si la diferencia aritmética supera un umbral crítico de tolerancia ($> 5$ iteraciones sin cambio), el sistema asume una pérdida de paquetes o desconexión física de la planta, encendiendo inmediatamente la alerta **Fuera de línea** en el panel frontal para advertir al operador.

---

## 🔌 2. Firmware del ESP32 (Estructura Base)

El código embebido cargado en el ESP32 gestiona las tareas críticas de bajo nivel para asegurar que el lazo se mantenga estable de manera autónoma:

* **Gestión de Red (`WiFi.h`):** Inicializa las credenciales de red inalámbrica y mantiene una rutina de reconexión activa en caso de caídas de señal.
* **Cliente HTTP y Firebase (`HTTPClient.h` / `FirebaseESP32.h`):**
  * **Rutina de Transmisión:** Cada ciclo de trabajo lee los canales del encoder para determinar la `Posicion`, incrementa una variable entera entera (`Heartbeat`) y envía un paquete JSON al nodo `/Estado.json` de Firebase.
  * **Rutina de Recepción:** Realiza una petición de lectura al nodo `/Variables.json`. Una vez descargado el objeto, utiliza una librería de parseo (como `ArduinoJson`) para extraer las variables dinámicas `Kp`, `Ki`, `Kd` y `SetPoint`.
* **Cómputo Local:** Aplica los nuevos parámetros directo en su algoritmo de control interno (ej. PID local o mapeo directo de potencia) para ajustar la velocidad y dirección del actuador físico en función de los requerimientos de la interfaz HMI.
