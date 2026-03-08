#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>
#include <WebSocketsServer.h>

const char* ssid = "SSID";
const char* password = "password";

MPU6050 mpu;
WebSocketsServer webSocket = WebSocketsServer(81);

// Raw accelerometer values
int16_t ax, ay, az;

// Calculated angles
float roll, pitch;

void setup() {

  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Start I2C
  Wire.begin();

  // Initialize MPU6050
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 connected");
  } else {
    Serial.println("MPU6050 connection failed");
  }

  // Start WebSocket server
  webSocket.begin();
}

void loop() {

  webSocket.loop();

  // Read accelerometer values
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert to float
  float axf = ax;
  float ayf = ay;
  float azf = az;

  // Calculate roll and pitch
  roll  = atan2(ayf, azf) * 180.0 / PI;
  pitch = atan2(-axf, sqrt(ayf * ayf + azf * azf)) * 180.0 / PI;

  // Send data to browser
  String data = String(roll) + "," + String(pitch);

  webSocket.broadcastTXT(data);

  // Print for debugging
  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print("  Pitch: ");
  Serial.println(pitch);

  delay(50);
}
