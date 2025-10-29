# wuyitongcasa0014

## Project Description
For the module assessment, the main function of this device is to measure the concentration of **CO₂** in the surrounding air and indicate the level through different LED colors, which are then reflected on the **Vespera light installation**.  
The basic design of the device includes a **CO₂ sensor (MQ135)**, an **Arduino WiFi module (MKR WiFi 1010)**, an **8-LED NeoPixel strip**, and a **push-button switch**.

---

## Circuit Schematic
<img width="1381" height="818" alt="image" src="https://github.com/user-attachments/assets/1f6d086e-05ef-461e-a6d4-88b4947c599b" />

---

## Circuit Wiring

| Component | Description | Connection Pin |
|------------|--------------|----------------|
| CO₂ Sensor (MQ135) | Analog output for CO₂ concentration | A0 |
| NeoPixel LED Strip (8-LED) | RGB output control | D6 |
| Push Button Switch | Device ON/OFF control | D2 |
| Power Supply | System VCC | 5V |
| Ground | Common reference | GND |

---

## Device Workflow

1. The **Arduino MKR WiFi 1010** module connects to the Internet via WiFi and establishes an **MQTT** connection, preparing to transmit data in real time.  
2. The **CO₂ sensor (MQ135)** measures the concentration of carbon dioxide in the air and outputs the value in **ppm** through analog pin **A0**.  
3. Upon receiving the sensor data, the **8-LED NeoPixel strip** displays a corresponding color based on the measured CO₂ level, with color transitions indicating air-quality changes.  
4. The **Vespera light installation** synchronizes its color with the NeoPixel strip, updating dynamically as the LEDs change.  
5. The entire process can be interrupted at any time using the **push-button switch** — pressing it turns the system off, and pressing again restarts the operation.  
6. All data and system status messages, such as current **CO₂ concentration** and **WiFi/MQTT connection state**, are displayed on the **Serial Monitor** for real-time observation and debugging.

---
