# Intercambio de Datos y Telemetría IoT mediante Firebase y JSON

Este proyecto implementa una arquitectura de comunicación web de tipo cliente-servidor en LabVIEW. Utiliza solicitudes HTTP orientadas al servicio REST API de **Firebase Realtime Database** para realizar la lectura (GET) y escritura (PUT) de objetos de datos estructurados en formato **JSON**.

---

## 📡 1. Módulo de Lectura: Receptor JSON (HTTP GET)

El primer instrumento virtual actúa como un suscriptor o monitor que extrae variables de la base de datos en tiempo real:

* **Petición HTTP GET:** Realiza una consulta directa a la URL del servidor (`https://databaseiva0262-default-rtdb.firebaseio.com/data.json`) cada 1000 ms.
* **Cuerpo de la Respuesta (Body):** El servidor de Firebase responde con una cadena de texto plana estructurada en formato JSON.
* **Decodificación (`Unflatten From JSON`):** El bloque especializado toma el string JSON proveniente de la web y lo parsea o convierte en un tipo de dato nativo de LabVIEW utilizando un **Cluster** molde como referencia.
* **Estructura de Datos Despaquetada:** Mediante un bloque *Unbundle*, el clúster se divide en sus componentes físicos para encender indicadores en el panel frontal:
  * `Hora` y `Fecha` (Cadenas de texto).
  * `Accionamiento` (Variable numérica de control).
  * `Rele` (Indicador booleano/LED).

---

## 🚀 2. Módulo de Escritura: Transmisor JSON (HTTP PUT)

El segundo instrumento virtual recolecta los estados locales de la interfaz y actualiza el estado del servidor en la nube:

* **Empaquetado de Datos (Bundle By Name):** Se agrupan las variables del sistema en un clúster estructurado que contiene la hora actual, la fecha del sistema, la posición o valor numérico del control `Accionamiento` y el estado lógico del interruptor `Rele`.
* **Codificación (`Flatten To JSON`):** Transforma el clúster binario de LabVIEW en un formato de texto estándar industrial **JSON Buffer**, entendible por cualquier plataforma web.
* **Petición HTTP PUT:** Envía el paquete JSON a través del VI `PUT.vi` a un nodo específico de la base de datos (`.../DiegoPerez.json`). A diferencia de un método POST que genera rutas aleatorias, el método **PUT** sobrescribe los datos del nodo seleccionado en tiempo real, manteniendo la base de datos limpia y actualizada.
