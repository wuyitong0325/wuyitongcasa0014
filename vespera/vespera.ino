#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

// -------------------- WiFi setting --------------------
char ssid[] = "CE-Hub-Student";        // WiFi name
char pass[] = "casa-ce-gagarin-public-service";   // WiFi password
int status = WL_IDLE_STATUS;

// -------------------- MQTT setting --------------------
const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;                        
const char* mqtt_user = "student";                 
const char* mqtt_pass = "ce2021-mqtt-forget-whale";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// everyone's individual topic ID
String lightId = "28"; 
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;

// -------------------- Vespera setting --------------------
const int num_leds = 72;
const int payload_size = num_leds * 3; 
byte RGBpayload[payload_size];

// -------------------- NeoPixel setting --------------------
#define NEOPIXEL_PIN 6
#define NUMPIXELS 8
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// -------------------- MQ135 setting --------------------
#define MQ135_PIN A0
#define RL 10.0
#define R0 76.63
#define CLEAN_AIR_RATIO 3.6

// -------------------- button setting --------------------
#define BUTTON_PIN 2
bool deviceOn = true;
int lastButtonState = HIGH;

// -------------------- WiFi connecting function --------------------
void startWifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// -------------------- MQTT reconnecting function --------------------
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("MKR1010_Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 3 seconds.");
      delay(3000);
    }
  }
}

// -------------------- get MQ135 reading --------------------
float getCO2ppm() {
  int adcValue = analogRead(MQ135_PIN);
  float voltage = adcValue * (3.3 / 1023.0);
  float RS = (3.3 - voltage) * RL / voltage;
  float ratio = RS / R0;

  float a = -0.42;
  float b = 1.92;
  float ppm = pow(10, (b + a * log10(ratio)));
  return ppm;
}

// -------------------- turn off all light --------------------
void turnOffLEDs() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.show();

  for (int i = 0; i < num_leds; i++) {
    RGBpayload[i * 3 + 0] = 0;
    RGBpayload[i * 3 + 1] = 0;
    RGBpayload[i * 3 + 2] = 0;
  }
  if (client.connected()) {
    client.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
  }
}

// -------------------- color mapping --------------------
int baseR = 0, baseG = 255, baseB = 0;

void setLEDColor(float ppm) {
  if (ppm < 40) ppm = 40;
  if (ppm > 100) ppm = 100;

  int step = int((ppm - 40) / 5);
  const int COLORS[][3] = {
    {0, 255, 0}, {128, 255, 0}, {255, 255, 0}, {255, 128, 0},
    {255, 0, 0}, {255, 0, 128}, {128, 0, 255}, {0, 128, 255}, {0, 255, 255}
  };
  const int NUM_COLORS = sizeof(COLORS) / sizeof(COLORS[0]);
  if (step < 0) step = 0;
  if (step >= NUM_COLORS) step = NUM_COLORS - 1;

  baseR = COLORS[step][0];
  baseG = COLORS[step][1];
  baseB = COLORS[step][2];

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(baseR, baseG, baseB));
  }
  pixels.show();

  for (int i = 0; i < num_leds; i++) {
    RGBpayload[i * 3 + 0] = (byte)baseR;
    RGBpayload[i * 3 + 1] = (byte)baseG;
    RGBpayload[i * 3 + 2] = (byte)baseB;
  }

  if (client.connected()) {
    client.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
  }

  Serial.print("CO2: ");
  Serial.print(ppm, 1);
  Serial.print(" ppm | RGB=(");
  Serial.print(baseR);
  Serial.print(",");
  Serial.print(baseG);
  Serial.print(",");
  Serial.print(baseB);
  Serial.println(")");
}

// -------------------- Setup --------------------
void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not detected!");
    while (true);
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  startWifi();
  client.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();

  pixels.begin();
  pixels.setBrightness(50);
  pixels.show();

  Serial.println("System ready: MQ135 + WiFi + Vespera online.");
}

// -------------------- Main loop (Synced Breathing) --------------------
void loop() {
  // ===== button check =====
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      deviceOn = !deviceOn;
      if (!deviceOn) {
        Serial.println("device turned off");
        turnOffLEDs();
      } else {
        Serial.println("device turned on");
      }
    }
  }
  lastButtonState = buttonState;

  if (!deviceOn) return;

  if (WiFi.status() != WL_CONNECTED) startWifi();
  if (!client.connected()) reconnectMQTT();
  client.loop();

  // ===== CO₂ sensing + color update =====
  static unsigned long lastUpdate = 0;
  static float lastPPM = 0;
  if (millis() - lastUpdate > 2000) {
    lastPPM = getCO2ppm();
    setLEDColor(lastPPM);
    lastUpdate = millis();
  }

  // ===== breathing effect (local + MQTT sync) =====
  static int brightness = 10;
  static int step = 3;
  brightness += step;
  if (brightness >= 200 || brightness <= 10) step = -step;


  int minDelay = 10;
  int maxDelay = 70;
  int breathDelay = map((int)lastPPM, 40, 100, maxDelay, minDelay);
  breathDelay = constrain(breathDelay, minDelay, maxDelay);

  // update light
  pixels.setBrightness(brightness);
  pixels.show();

  // sychronize to Vespera
  float scale = brightness / 200.0; 
  for (int i = 0; i < num_leds; i++) {
    RGBpayload[i * 3 + 0] = (byte)(baseR * scale);
    RGBpayload[i * 3 + 1] = (byte)(baseG * scale);
    RGBpayload[i * 3 + 2] = (byte)(baseB * scale);
  }
  if (client.connected()) {
    client.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
  }

  delay(breathDelay);
}
