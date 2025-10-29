# wuyitongcasa0014 · CO₂ Reactive Light System

![Arduino](https://img.shields.io/badge/Arduino-MKR1010-00979D?logo=arduino&logoColor=white)
![WiFiNINA](https://img.shields.io/badge/Library-WiFiNINA-blue)
![MQTT](https://img.shields.io/badge/Protocol-MQTT-purple)
![NeoPixel](https://img.shields.io/badge/LED-Adafruit_NeoPixel-orange)
![License](https://img.shields.io/badge/License-MIT-green)

---

## Project Overview
This project implements an **IoT-based CO₂ indicator system** that measures the concentration of CO₂ in the surrounding air using an **MQ135 sensor** and displays the air quality through a dynamic **NeoPixel LED strip**.  
The system synchronizes the color display with a **Vespera light installation** over **MQTT**, visualizing live environmental data in a responsive light pattern.

---

## Hardware Components (Circuit Wiring)

| Component | Description | Connection Pin |
|---|---|---|
| **CO₂ Sensor (MQ135)** | Analog output for CO₂ concentration | **A0** |
| **NeoPixel LED Strip (8-LED)** | RGB color feedback | **D6** |
| **Push Button Switch** | System ON/OFF (INPUT_PULLUP) | **D2** |
| **Power Supply** | System VCC | **5V** |
| **Ground** | Common reference | **GND** |

> Button wiring: one leg → **D2**, the other → **GND**; use `pinMode(D2, INPUT_PULLUP)`.

---

## Circuit Schematic
<img width="1381" height="818" alt="image" src="https://github.com/user-attachments/assets/1f6d086e-05ef-461e-a6d4-88b4947c599b" />

## Software Setup

1.  Required Libraries: `#include <WiFiNINA.h>`, `#include <PubSubClient.h>`, `#include <Adafruit_NeoPixel.h>`
    *   Install via Arduino Library Manager
2.  WiFi Configuration: `char ssid[] = "CT-Hub-Student"; char pass[] = "caca-co-gagarin-public-service";`
3.  MQTT Configuration: `const char* mqtt_server = "mqtt.onsolid.org"; const int mqtt_port = 1883; const char* mqtt_user = "student"; const char* mqtt_pass = "cn2021-moqit-forgen-whale"; String lightid = "20"; String mqtt_topic = "student/CASA0014/luminaire"; + lightid;`
4.  Upload & Run
    *   Board: Arduino MKR WiFi 1010
    *   Upload your sketch (e.g., `vespera.ino`)
    *   Open Serial Monitor @ 9600 baud to view CO2 readings & WiFi/MQTT status

## Device Workflow

*   MKR WiFi 1010 connects to WiFi and establishes an MQTT client.
*   MQ135 measures CO2 ppm via A0.
*   NeoPixel (8-LED) on D6 displays color mapped to ppm.
*   RGB payload is published to MQTT so Vespera mirrors the color.
*   Button on D2 toggles system ON/OFF (debounced in code).
*   Serial Monitor prints ppm, WiFi, and MQTT status messages.

## CO2 - Colour Mapping (from code)

*   The code maps ppm to a 9-step palette in 5-ppm increments (damped range).
*   ```c++
    // Clamp ppm, compute step, pick color
    const int COLORS[9] = { (0,255,0), (128,255,0), (255,255,0), (255,128,0), (255,0,0), (255,0,128), (128,0,255), (0,0,255), (0,255,255) };
    int step = constrain((int)(ppm - 40) / 5, 0, 8);
    int r = COLORS[step][0];
    int g = COLORS[step][1];
    int b = COLORS[step][2];
    ```

## Legend

*   CO2 (ppm) Color Meaning:
    *   < 40: Green (Fresh)
    *   40-50: Light Green (Normal)
    *   50-60: Yellow (Mild)
    *   60-70: Orange (Moderate)
    *   70-80: Red (Unhealthy)
    *   > 80: Pink/Cyan (Severe) (palette still)
*   MQTT Payload (`Vespera Sync`)
    *   Topic: `student/CASA0014/luminaire`
    *   Payload: `72 + RGB bytes` (for Vespera), mirrored from the local NeoPixel color.

## Notes/Tips

*   Power: MQ135 typically needs stable 5V; GND must be common with MKR1010 and NeoPixel.
*   Button: you may add a 100 nF cap from D2-GND for extra hardware debounce.
*   NeoPixel: keep wires short; set brightness prudently to avoid brown-outs.
*   Calibration: MQ135 ppm is approximate without calibration/burn-in.

# Author

*   Yitong Wu - MSc Electronic & Computer Engineering, HKUST October 2025
*   "Bridging data, color, and atmosphere - an IoT system that visualize the air we breathe."
