# wuyitongcasa0014
For the module assessment, the main function of this device is to measure the concentration of CO₂ in the surrounding air and indicate the level through different LED colors, which are then reflected on the Vespera light installation.
The basic design of the device includes a CO₂ sensor MQ135, an Arduino WiFi module MKR WIFI 1010, an 8-LED NeoPixel strip, and a control switch.
The schematic of the circuit is shown below (the schematic can also be found in the uploaded file named "vespera_project_schematics")：
<img width="1381" height="818" alt="image" src="https://github.com/user-attachments/assets/1f6d086e-05ef-461e-a6d4-88b4947c599b" />
The device works as following:
1. The Arduino MKR WiFi 1010 module connects to the Internet via WiFi and establishes an MQTT connection, preparing to transmit data in real time.
2. The CO₂ sensor measures the concentration of carbon dioxide in the air and outputs the value in ppm through analog pin A0.
3. Upon receiving the sensor data, the 8-LED NeoPixel strip displays a corresponding color based on the measured CO₂ level, with color transitions indicating air quality changes.
4. The Vespera light installation synchronizes its color with the NeoPixel strip, updating dynamically as the LEDs change.
5. The entire process can be interrupted at any time using the switch — it turns the system off, and restarts the operation.
All data and system status messages, such as current CO₂ concentration and WiFi/MQTT connection state, are displayed on the Serial Monitor for real-time observation and debugging.
