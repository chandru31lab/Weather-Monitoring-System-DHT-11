#define BLYNK_TEMPLATE_ID "XXXXXXXXXXXXXXX"
#define BLYNK_TEMPLATE_NAME "xxxxxxxxxxxxx"
#define BLYNK_AUTH_TOKEN "xxxxxxxxxxxxxxx"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Wi-Fi Credentials
char ssid[] = "Your_SSID";      // Replace with your Wi-Fi SSID
char pass[] = "Password";  // Replace with your Wi-Fi password

// DHT11 Configuration
#define DHTPIN D4  // GPIO2 (D4 pin)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// I2C LCD Configuration (use a compatible library)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27, 16 columns, 2 rows

BlynkTimer timer;

bool isBlynkConnected = false;

void setup() {
  // Start Serial for debugging
  Serial.begin(115200);

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Connect to WiFi in a non-blocking way
  WiFi.begin(ssid, pass);
  Serial.println("Connecting to WiFi...");

  // Attempt to connect to Blynk
  connectToBlynk();

  // Set up a function to read the sensor every 2 seconds
  timer.setInterval(1000L, sendSensorData);
}

void connectToBlynk() {
  Blynk.config(BLYNK_AUTH_TOKEN); // Configure Blynk (non-blocking)
}

void sendSensorData() {
  // Read temperature and humidity from DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();  // Celsius
  
  // Check if any reads failed and try again
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT Read Error");
    return;
  }

  // Print values to Serial
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Display the values on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print(" %");

  // Send values to Blynk if connected
  if (isBlynkConnected) {
    Blynk.virtualWrite(V0, temperature); // V5 for temperature
    Blynk.virtualWrite(V1, humidity);    // V6 for humidity
  }
}

void loop() {
  // Check if Wi-Fi is connected
  if (WiFi.status() == WL_CONNECTED) {
    // Attempt to reconnect to Blynk if not already connected
    if (!Blynk.connected()) {
      Serial.println("Attempting to connect to Blynk...");
      if (Blynk.connect()) {
        Serial.println("Connected to Blynk!");
        isBlynkConnected = true;
      } else {
        Serial.println("Failed to connect to Blynk");
        isBlynkConnected = false;
      }
    }
  } else {
    Serial.println("WiFi not connected");
    isBlynkConnected = false;
  }

  // Run Blynk in the background if connected
  if (isBlynkConnected) {
    Blynk.run();
  }

  // Run the timer to call the sensor function
  timer.run();
}
