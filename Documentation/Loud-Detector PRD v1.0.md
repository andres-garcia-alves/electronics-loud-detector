# PRD - Detector de Griterío de bajo consumo con ATtiny85
Versión: 1.0  
Autor: Andrés Garcia Alves

## 1. Objetivo del Producto

Construir un detector autónomo de sonido fuerte basado en ATtiny85 que:
- Detecte eventos de volumen alto
- Active buzzer y LEDs solo cuando corresponda
- Minimize consumo (modo sleep dominante)
- Permanezca encendido semanas o meses
- Incluye interruptor físico ON/OFF entre batería y booster


## 2. Filosofía de Diseño

- "Dormido" el 99% del tiempo
- Sensado periódico ultracorto (2 veces por segundo)
- Periféricos apagado por defecto
- Activación solo cuando hay evento sonoro


## 3. Alcance (MVP)

- Sensor KY-038 analógico
- Sensibilidad de activación configurable con potenciómetro
- Volumen buzzer configurable con potenciómetro
- 2 niveles/umbrales según intensidad detectada
- Buzzer activo ~5/~10 segs si supera uno o ambos umbrales
- 2 LEDs indicadores solo durante evento
- Interruptor físico general


## 4. Plataforma Hardware

### 4.1 Microcontrolador

- ATtiny85 @ 1 MHz interno
- Programación Arduino IDE

### 4.2 Sensado

- KY-038 AO → ADC
- Alimentación controlada por GPIO mediante **corte high-side**

### 4.3 Actuadores

- Buzzer piezo pasivo (via NPN)
- LEDs × 2 (evento medio/fuerte)

### 4.4 Alimentación

- Li-ion 1 o 2 celdas en paralelo (10440/14500/18650)
- TP4056 para carga
- Interruptor entre batería y booster
- DC/DC converter step-up 3.7 → 5 V (MT3608 o similar)

### 4.5 Filtrado

- 100 µF bulk en 5V
- 2 x 100 nF cerámico cerca de VCC ATtiny y sensor KY-037


## 5. Distribución de Pines del ATtiny85

| Función      | Nombre MCU    | Pin físico | Tipo              |
|--------------|---------------|------------|-------------------|
| Mic ON/OFF   | PB0           | 5          | Salida digital    |
| BUZZ+LED1    | PB1           | 6          | Salida digital    |
| LED 2        | PB2           | 7          | Salida digital    |
| Sensibilidad | A3/ADC3 (PB3) | 2          | Entrada analógica |
| Mic OUT      | A2/ADC2 (PB4) | 3          | Entrada analógica |

Notas:
- PB5/RESET se mantiene como reset (descartado, porque usarlo requiere quemar fuses)
- El volumen se maneja de forma 100% analógica (RV2)


## 6. Potenciómetros

### 6.1 Umbral sensibilidad (ADC)

Conexión:
5V — pot — GND  
Cursora → ADC  
Firmware compara mic vs pot

### 6.2 Volumen buzzer

5V → pot → buzzer → NPN → GND
Pote 5k–20k lineal
Resistencia mínima a GND 100–330Ω


## 7. Software: Requisitos Funcionales

### Ciclo operativo

1. Dormir Power-down
2. WDT wake cada ~1 s
3. Encender sensor (GPIO + high-side)
4. Esperar 10 ms
5. Leer ADC (32 muestras)
6. Leer pot umbral
7. Evaluar evento
8. Apagar sensor
9. Si evento: LEDs ON + Buzzer ON (~5 s)
10. Dormir

### ADC & filtrado

- 32 muestras
- Descartar primera tras encender sensor
- 1 ms entre lecturas
- ADC clock ≤ 200 kHz
- Promedio ventana


## 8. Gestión Energética

### Estados normales

| Elemento | Estado   |
|----------|----------|
| Sensor   | Apagado  |
| LEDs     | Apagados |
| Buzzer   | Apagado  |
| MCU      | Sleep    |

### Wake periódico

- WDT cada ~0.5 a ~1 s.


## 9. Consumo y Autonomía

### Consumos típicos

| Componente   | Modo       | Consumo  |
|--------------|------------|----------|
| ATtiny sleep | Power-down | 6–15 µA  |
| KY-038       | Encendido  | 4–6 mA   |
| LED          | ON         | 3–5 mA   |
| Buzzer       | Evento     | 20–40 mA |

### Promedio global esperado

~30–50 µA (sin eventos frecuentes)

### Autonomía estimada

| Batería | Capacidad     | Autonomía   |
|---------|---------------|-------------|
| 10440   | 350–600 mAh   | 1–3 semanas |
| 14500   | 700–1000 mAh  | 1–2 meses   |
| 18650   | 2000–3000 mAh | 2–6 meses   |
| 2×18650 | 4000–6000 mAh | 6–12 meses  |


## 10. Validación

- Pruebas con niveles reales (habla / aplausos / música / grito)
- Medición de consumo en reposo y evento
- Ajuste de pot sensibilidad


## 11. Entregables

- Diseño KiCad
- BOM
- Firmware .ino
- Pruebas de autonomía


## 12. Confirmado explícitamente

✔ Corte high-side  
✔ Sleep WDT 1 s  
✔ 2 potenciómetros: umbral + volumen  
✔ 3 LEDs solo en evento  
✔ Autonomía de semanas/meses  
✔ Interruptor ON/OFF físico  
✔ Mantener KY-038  
✔ Sin pantallas ni extras  


## 13. Para Versión 2.0

- Aumentar autonomía con:
  - Mantener lógica de 2 umbrales de audio (5-10 segundos del buzzer), pero eliminar el LED2
  - Utilizar el pin liberado para controlar la alimentación del umbral de sensibilidad (RV1)
  - Actualmente RV1 (10K) es un consumo **permanente** de 500uA
  - Desactivar el BOD del ATtiny85
  - Desactivar Timer1 del ATtiny85 (revisar que no afecte al Buzzer)


## 14. Fuera de Alcance

- Pantallas
- Comunicación
- Registro de datos
- Sensores alternativos
