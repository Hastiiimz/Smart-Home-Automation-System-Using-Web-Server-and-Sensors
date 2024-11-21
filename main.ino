#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>  // Include the ESP32Servo library

// Network settings
const char *ssid = "IoT-Group1-AP";
const char *password = "12345678";

// IP settings
IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Define pins for SRF04 sensor, LEDs, and servo motor
const int trigPin = 5;
const int echoPin = 18;
const int led1 = 19;
const int led2 = 21;
const int led3 = 22;
const int servoPin = 23;

// Create a WebServer object on port 80
WebServer server(80);

// Create a Servo object
Servo myServo;
int servoAngle = 0; // Initial servo angle

void setup() {
  Serial.begin(115200);

  // Set sensor pins as input and output
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set LED pins as output
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  // Attach the servo motor to the pin
  myServo.attach(servoPin);
  myServo.write(servoAngle);

  // Start AP mode with IP settings
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("AP Config Failed");
  }
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("AP Failed");
    return;
  }
  Serial.println("Access Point started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Set up the routes and handlers for the web server
  server.on("/", handleRoot);
  server.on("/setServo", handleSetServo);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
  long distance = readSRF04();

  // Control LEDs based on distance
  digitalWrite(led1, distance < 10 ? HIGH : LOW);
  digitalWrite(led2, distance < 20 ? HIGH : LOW);
  digitalWrite(led3, distance < 30 ? HIGH : LOW);
}

// Function to read data from SRF04 sensor
long readSRF04() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;

  return distance;
}

// Handler for the root route
void handleRoot() {
  long distance = readSRF04();

  // Control LEDs based on distance
  digitalWrite(led1, distance < 10 ? HIGH : LOW);
  digitalWrite(led2, distance < 20 ? HIGH : LOW);
  digitalWrite(led3, distance < 30 ? HIGH : LOW);

  String message = "<html><head><title>SRF04 Sensor Data</title></head>";
  message += "<style>";
  message += ".circle {width: 50px; height: 50px; border-radius: 50%; display: inline-block; margin: 10px;}";
  message += ".on {background-color: green;}";
  message += ".off {background-color: black;}";
  message += "</style></head><body>";
  message += "<h1> Hojjat Yazdan 99242155 , Amirhossein Dibaj 99242059 , Artin Elhamirad 99242017</h1>";
  message += "<h2>SRF04 Sensor Data</h2>";
  message += "<p>Distance: " + String(distance) + " cm</p>";
  message += "<div class='circle " + String(digitalRead(led1) == HIGH ? "on" : "off") + "'></div>";
  message += "<div class='circle " + String(digitalRead(led2) == HIGH ? "on" : "off") + "'></div>";
  message += "<div class='circle " + String(digitalRead(led3) == HIGH ? "on" : "off") + "'></div>";
  message += "<h2>Servo Control</h2>";
  message += "<input type='range' min='0' max='180' value='" + String(servoAngle) + "' id='servoSlider' onchange='updateServo(this.value)'>";
  message += "<p>Angle: <span id='servoAngle'>" + String(servoAngle) + "</span> degrees</p>";
  message += "<script>";
  message += "function updateServo(val) {";
  message += "  var xhr = new XMLHttpRequest();";
  message += "  xhr.open('GET', '/setServo?angle=' + val, true);";
  message += "  xhr.send();";
  message += "  document.getElementById('servoAngle').innerText = val;";
  message += "}";
  message += "</script>";
  message += "</body></html>";
  server.send(200, "text/html", message);
}

// Handler to set the servo angle
void handleSetServo() {
  if (server.hasArg("angle")) {
    servoAngle = server.arg("angle").toInt();
    myServo.write(servoAngle);
  }
  server.send(200, "text/plain", "OK");
}