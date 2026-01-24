# PRD — Detector de Gritos de Bajo Consumo con ATtiny85
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
- Sensado periódico ultracorto (1× por segundo)
- Todo periférico apagado por defecto
- Activación solo cuando hay evento sonoro

## 3. Alcance (MVP)
- Sensor KY-038 analógico
- Umbral configurable con potenciómetro
- Volumen buzzer configurable con potenciómetro
- 3 LEDs indicadores solo durante evento
- Buzzer activo ~5 s si supera umbral
- Sin comunicaciones
- Interruptor físico general

## 4. Plataforma Hardware

### 4.1 Microcontrolador
- ATtiny85 @ 1 MHz interno
- Programación Arduino IDE

### 4.2 Sensado
- KY-038 AO → ADC
- Alimentación controlada por GPIO mediante **corte high-side**

### 4.3 Actuadores
- LEDs × 3 (evento leve/medio/fuerte)
- Buzzer piezo activo (via NPN)

### 4.4 Alimentación
- Li-ion 1 o 2 celdas en paralelo (10440/14500/18650)
- TP4056 para carga
- Interruptor entre batería y booster
- Step-up 3.7 → 5 V (MT3608 o similar)

### 4.5 Filtrado
- 100 µF bulk en 5V
- 100 nF cerámico cerca de VCC ATtiny

## 5. Distribución de Pines del ATtiny85

| Función | Pin físico | Nombre MCU | Tipo |
|--------|-------------|-------------|------|
Mic audio | 7 | PB2 / ADC1 | Entrada analógica |
Umbral pot | 3 o 2* | PB4 / ADC2 | Entrada analógica |
Corte alimentación mic | 6 | PB1 | Salida digital |
Buzzer (NPN) | 5 | PB0 | Salida digital |
LED 1 | 2 o 3* | PB3 | Salida digital |
LED 2 | 2/3/4* | PB3/PB4 | Salida digital |
LED 3 | 0/1/5* | PB0/PB? | Salida digital |

*PB5/RESET se mantiene como reset (no se usa).

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
9. Si evento:
   - LEDs ON (~5 s)
   - Buzzer ON (~5 s)
10. Dormir

### ADC & filtrado
- 32 muestras
- Descartar primera tras encender sensor
- 1 ms entre lecturas
- ADC clock ≤ 200 kHz
- Promedio ventana

## 8. Gestión Energética

### Estados normales
| Elemento | Estado |
|----------|--------|
Sensor | Apagado |
LEDs | Apagados |
Buzzer | Apagado |
MCU | Sleep |

### Wake periódico
- WDT cada ~1 s

## 9. Consumo y Autonomía

### Consumos típicos
| Componente | Modo | Consumo |
|------------|------|----------|
ATtiny sleep | Power-down | 6–15 µA |
KY-038 | Encendido | 4–6 mA |
LED | ON | 3–5 mA |
Buzzer | Evento | 20–40 mA |

### Promedio global típico
~30–50 µA (sin eventos frecuentes)

### Autonomía estimada
| Batería | Capacidad | Autonomía |
|---------|------------|------------|
10440 | 350–600 mAh | 1–3 semanas |
14500 | 700–1000 mAh | 1–2 meses |
18650 | 2000–3000 mAh | 2–6 meses |
2×18650 | 4000–6000 mAh | 6–12 meses |

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

## 13. Fuera de Alcance
- Pantallas
- Comunicación
- Registro de datos
- Sensores alternativos
