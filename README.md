🌈 wuyitongcasa0014 · CO₂ Reactive Light System










📝 Project Overview

This project implements an IoT-based CO₂ indicator system that measures the concentration of CO₂ in the surrounding air using an MQ135 sensor and displays the air quality through a dynamic NeoPixel LED strip.
The system synchronizes the color display with a Vespera light installation over MQTT, visualizing live environmental data in a responsive light pattern.

⚙️ Hardware Components
Component	Description	Connection Pin
🧠 CO₂ Sensor (MQ135)	Analog output for CO₂ concentration	A0
💡 NeoPixel LED Strip (8-LED)	RGB light strip showing color feedback	D6
🔘 Push Button Switch	Toggles system ON/OFF	D2
🔋 Power Supply	System VCC	5V
⚫ Ground	Common reference	GND

Design of the device shown in the schematic below: ( can also be found in the uploaded file)
<img width="1381" height="818" alt="image" src="https://github.com/user-attachments/assets/1f6d086e-05ef-461e-a6d4-88b4947c599b" />

🧰 Software Setup
1. Required Libraries
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>


Install these via Arduino Library Manager.

2. WiFi Configuration
char ssid[] = "CE-Hub-Student";
char pass[] = "casa-ce-gagarin-public-service";

3. MQTT Configuration
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;
const char* mqtt_user = "student";
const char* mqtt_pass = "ce2021-mqtt-forget-whale";
String lightId = "28";
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;

4. Upload and Run

Select board: Arduino MKR WiFi 1010

Upload the sketch vespera.ino

Open Serial Monitor (9600 baud) to view CO₂ readings and WiFi/MQTT status

💡 Light Mode Logic
CO₂ Level (ppm)	LED Color	Meaning
< 40	🟢 Green	Fresh air
40–50	🟩 Light Green	Normal
50–60	🟨 Yellow	Mildly polluted
60–70	🟧 Orange	Moderate
70–80	🔴 Red	Unhealthy
80+	💗 Pink / 🔵 Cyan	Severe

Color steps change every 5 ppm, creating smooth transitions across the NeoPixel strip and the remote Vespera installation.

🚀 Device Workflow

The Arduino MKR WiFi 1010 connects to WiFi and establishes an MQTT connection.

The MQ135 sensor measures CO₂ concentration and sends analog values via A0.

The NeoPixel LED strip displays the corresponding color based on CO₂ level.

The system publishes the same RGB data to Vespera through MQTT for color synchronization.

The push button (D2) can turn the system on or off at any time.

All readings (CO₂ ppm, WiFi status, MQTT connection) are displayed in the Serial Monitor.

🎨 Colour Mapping Function
void setLEDColor(float ppm) {
  const int COLORS[][3] = {
    {0, 255, 0}, {128, 255, 0}, {255, 255, 0}, {255, 128, 0},
    {255, 0, 0}, {255, 0, 128}, {128, 0, 255}, {0, 128, 255}, {0, 255, 255}
  };
  int step = constrain((int)((ppm - 40) / 5), 0, 8);
  int r = COLORS[step][0], g = COLORS[step][1], b = COLORS[step][2];
  // Apply color to all NeoPixels and publish via MQTT
}

🧩 System Logic Diagram
[MQ135 Sensor] → [Analog A0] → [CO₂ Value in ppm]
           ↓
  [Color Mapping Function]
           ↓
[NeoPixel LED Strip D6] → [Vespera MQTT Topic]
           ↑
      [Push Button D2] → [ON/OFF Control]


📸 Physical Design & Output

Enclosure: small housing box

Display: Vespera lights mirror NeoPixel colors in real time

Sensor Placement: MQ135 positioned inside enclosure with a small window open for accurate air sampling


🧠 Author

Yitong Wu
MSc Electronic & Computer Engineering, HKUST
📅 October 2025

💬 “Bridging data, color, and atmosphere — an IoT system that visualizes the air we breathe.”
