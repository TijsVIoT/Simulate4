#include <ESP32Servo.h>
#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <DFRobot_HX711_I2C.h>

const char* ssid = "Galaxy S20 FE A601";
const char* password = "jnna0341";
const char* botToken = "6998324646:AAFoh6_mfoINcOzhpMPf9ZNMbT95-CY5YGA";
const String chatId = "5444660794";

#define HX711_I2C_ADDR 0x64 // Controleer of dit het juiste I2C-adres is
DFRobot_HX711_I2C MyScale(&Wire, HX711_I2C_ADDR);

float Weight = 0;
unsigned long previousMillis = 0;
const long interval = 1000; // Interval van 10 seconden (10 * 1000 milliseconden)

// Servo motor pin and object
const int servoPin = A4; // Change this to the correct pin
Servo myServo;

void sendTelegramMessage(const char* message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String("https://api.telegram.org/bot") + botToken + "/sendMessage?chat_id=" + chatId + "&text=" + message;

    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending GET: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Not connected to WiFi, can't send message");
  }
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int retryCount = 0;

  while (WiFi.status() != WL_CONNECTED && retryCount < 20) { // Max 20 pogingen
    delay(500); // Wacht 0,5 seconden per poging
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    sendTelegramMessage("Verbonden met het internet");
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi");
  }
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();

  if (!MyScale.begin()) {
    Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
    while (1) {
      delay(1000); // Blijf hier hangen als de sensor niet correct is geïnitialiseerd
    }
  }

  MyScale.setCalWeight(50);
  MyScale.setThreshold(30);

  Serial.print("the calibration value of the sensor is: ");
  Serial.println(MyScale.getCalibration());
  MyScale.setCalibration(MyScale.getCalibration());
  delay(1000);

  // Servo initialization
  myServo.attach(servoPin);
  myServo.write(0); // Ensure the servo is closed initially
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Weight = MyScale.readWeight();
    Serial.print("weight is: ");
    if (Weight > 1.5) {
      Serial.print(Weight, 1);
    } else {
      Serial.print(0, 1);
    }
    Serial.println(" g");

    if (Weight < 5.0) {
      sendTelegramMessage("De voederbak is bijna leeg!");

      // Open the servo motor for 10 seconds
      myServo.write(90); // Adjust this value to the correct open position
      delay(20000); // Wait for 10 seconds
      myServo.write(0); // Close the servo motor
    }

    if (Weight > 20.0) {
      sendTelegramMessage("De voederbak zit over zijn limiet!");
    }
  }

  delay(5000); // Voeg een korte vertraging toe om de loop te vertragen
}
