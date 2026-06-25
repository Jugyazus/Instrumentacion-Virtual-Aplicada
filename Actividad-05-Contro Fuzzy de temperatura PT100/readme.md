# Sistema de Control Difuso (Fuzzy Logic) para Temperatura de Lámpara Incandescente

Este módulo implementa un lazo cerrado de control difuso (`SISO`) en LabVIEW para regular la temperatura de una lámpara incandescente de alta potencia (360 W). El sistema utiliza una arquitectura modular dividida en adquisición de datos, inferencia difusa, modulación por ancho de pulso (PWM) por software acoplada a un relevador de estado sólido (SSR), y un subsistema de registro de datos (*Datalogger*) en formato CSV.

---

## 📐 Arquitectura del Hardware y Etapa de Potencia

1. **Planta:** Lámpara incandescente de 360 W.
2. **Sensor de Retroalimentación:** Sensor de temperatura RTD PT100, acondicionado y leído mediante una tarjeta de adquisición de datos (DAQ).
3. **Etapa de Acoplamiento y Disparo:** Una salida digital de la DAQ se conecta a la base de un transistor (etapa de potencia de baja señal) que conmuta la corriente necesaria para disparar un Relevador de Estado Sólido **SSR-40 DA**.
4. **Actuador:** El SSR-40 DA realiza la conmutación de la línea de corriente alterna que alimenta la lámpara.

---

## 💻 1. Algoritmo de Control Difuso e Inferencia (Lazo Principal)

El ciclo principal adquiere la variable física y calcula la acción de control necesaria:
* **Adquisición de Datos (DAQ Assistant):** Captura el voltaje analógico del sensor PT100. Mediante escalamiento matemático (bloques de multiplicación y resta), convierte el valor crudo en la temperatura real de la planta (`Temp`).
* **Cálculo del Error:** Compara el valor actual contra la referencia del usuario (`Temp Deseada`) para inyectar el error del sistema al bloque difuso.
* **Controlador Fuzzy (SISO):** Evalúa el error a través de funciones de membresía preconfiguradas y entrega un ciclo de trabajo (`Duty`) normalizado en un rango de 0% a 100%.
* **Conversión a Tiempos de Conmutación:** Suponiendo un periodo base total de muestreo de $16.66\text{ ms}$ (equivalente a un ciclo de la frecuencia de línea de 60 Hz), el sistema calcula matemáticamente de forma proporcional el tiempo que la señal debe permanecer en alto (`High Time`) y en bajo (`Low Time`).

---

## 🔌 2. Modulación por Software y Driver DAQmx

Dado que se requiere un control síncrono del SSR, el segundo lazo se encarga de estructurar el tren de pulsos digital empleando retardos dinámicos:
* **Inicialización:** Configura y arranca una tarea de salida digital de un solo canal (`Digital Output - 1Chan 1Samp`) mediante la API de bajo nivel de **NI-DAQmx**.
* **Máquina de Conmutación Alternada:** Evaluando si el número de iteraciones del ciclo es par o impar (`i % 2`), una estructura de casos determina si escribe un estado lógico alto (`True`) o bajo (`False`) hacia el pin de control.
* **Retardo por Variables Locales:** El tiempo que duerme el ciclo en cada estado se asigna dinámicamente extrayendo los valores de `High Time` y `Low Time` calculados en el lazo PID/Fuzzy a través de variables locales, garantizando el ancho de pulso exacto en el transistor de disparo.

---

## 📊 3. Subsistema Datalogger (Histórico en Excel/CSV)

Para permitir el análisis posterior de las curvas de estabilidad y la respuesta transitoria del controlador difuso, se ejecuta un lazo dedicado a la persistencia de datos:
* **Recolección de Variables:** Mediante variables locales, el bloque consolida las 4 variables críticas del sistema en un arreglo de clústeres: `Temp`, `Temp Deseada`, `Error` y `Duty`.
* **Indexación Temporal:** Añade una estampa de tiempo/iteración a la fila de datos para llevar la secuencia cronológica.
* **Escritura en Archivo (`Write Delimited Spreadsheet`):** Configurado con un temporizador estricto de una muestra por segundo ($1000\text{ ms}$), el bloque añade filas de manera incremental (*Append*) en una ruta local fija:  
  `D:\Trabajos UPIIZ\OCTAVO SEMESTRE\Instrumentacion Virtual Aplicada\Temperatura PT100.csv`.
