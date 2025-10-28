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
const int mqtt_port = 1884;                        // using WebSocket MQTT port
const char* mqtt_user = "student";                 // username
const char* mqtt_pass = "ce2021-mqtt-forget-whale";// password

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// everyone's individual topic ID
String lightId = "28"; 
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;

// -------------------- Vespera setting --------------------
const int num_leds = 72;
const int payload_size = num_leds * 3; // every led 3bit RGB
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

  // turn off the vespera
  for (int i = 0; i < num_leds; i++) {
    RGBpayload[i * 3 + 0] = 0;
    RGBpayload[i * 3 + 1] = 0;
    RGBpayload[i * 3 + 2] = 0;
  }
  if (client.connected()) {
    client.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
  }
}

// -------------------- set led colour according to the air quality --------------------
void setLEDColor(float ppm) {
  int r, g, b;

  if (ppm < 40) ppm = 50;//CO2 detecting range
  if (ppm > 80) ppm = 90;

  int step = int((ppm - 40) / 5);// every colour covers 5ppm

  const int COLORS[][3] = {
    {0, 255, 0}, {128, 255, 0}, {255, 255, 0}, {255, 128, 0},
    {255, 0, 0}, {255, 0, 128}, {128, 0, 255}, {0, 128, 255}, {0, 255, 255}//set different colour
  };
  const int NUM_COLORS = sizeof(COLORS) / sizeof(COLORS[0]);

  if (step < 0) step = 0;
  if (step >= NUM_COLORS) step = NUM_COLORS - 1;

  r = COLORS[step][0];
  g = COLORS[step][1];
  b = COLORS[step][2];

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();

  for (int i = 0; i < num_leds; i++) {
    RGBpayload[i * 3 + 0] = (byte)r;
    RGBpayload[i * 3 + 1] = (byte)g;
    RGBpayload[i * 3 + 2] = (byte)b;
  }

  if (client.connected()) {
    if (client.publish(mqtt_topic.c_str(), RGBpayload, payload_size)) {
      Serial.println("→ MQTT publish success");
    } else {
      Serial.println("→ MQTT publish failed!");
    }
  }

  Serial.print("CO2: ");
  Serial.print(ppm, 1);
  Serial.print(" ppm | Step=");
  Serial.print(step);
  Serial.print(" | RGB=(");
  Serial.print(r);
  Serial.print(",");
  Serial.print(g);
  Serial.print(",");
  Serial.print(b);
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

  pinMode(BUTTON_PIN, INPUT_PULLUP);  // use the internal pullup ressistor

  startWifi();
  client.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();

  pixels.begin();
  pixels.setBrightness(50);
  pixels.show();

  for (int i = 0; i < payload_size; i++) RGBpayload[i] = 0;

  Serial.println("System ready: MQ135 + WiFi + Vespera online.");
}

// -------------------- main loop --------------------
void loop() {
  // ===== checking the button logic（not interfering Serial output） =====
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

  // if turn off, skip the main loop
  if (!deviceOn) return;
  // ============================================

  if (WiFi.status() != WL_CONNECTED) startWifi();
  if (!client.connected()) reconnectMQTT();
  client.loop();

  float ppm = getCO2ppm();
  setLEDColor(ppm);

  delay(1000); // upgrate every sec
}
