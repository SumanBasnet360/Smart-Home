#include <WiFi.h>
#include <WebServer.h>
#include <AHT10.h>
#include <ESP32_Servo.h>

const char* ssid = "ESP32";
const char* password = "12345678";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

#define LED1pin 12
bool LED1status = LOW;

#define LED2pin 14
bool LED2status = LOW;

#define LED3pin 33
bool LED3status = LOW;

#define FANpin 27
bool FANstatus = LOW;

#define LED4pin 32
bool LED4status = LOW;

#define LED5pin 15
bool LED5status = LOW;

AHT10 aht;
float temperature = 0.0;
float humidity = 0.0;

Servo doorServo;
#define doorPin 25
const int doorOpenAngle = 0;
const int doorClosedAngle = 180;
bool isDoorOpen = false;

#define lightPin 26

#define ldrPin 34
#define irPin 13

bool streetLightStatus = false;
bool doorAutomaticMode = true;
bool lightAutomaticMode = true;
const int ldrThreshold = 100;

void setup() {
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);
  pinMode(LED3pin, OUTPUT);
  pinMode(LED4pin, OUTPUT);
  pinMode(LED5pin, OUTPUT);
  pinMode(FANpin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(irPin, INPUT);

  doorServo.attach(doorPin);

  aht.begin();

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.on("/led3on", handle_led3on);
  server.on("/led3off", handle_led3off);
  server.on("/led4on", handle_led4on);
  server.on("/led4off", handle_led4off);
  server.on("/led5on", handle_led5on);
  server.on("/led5off", handle_led5off);
  server.on("/fanon", handle_fanon);
  server.on("/fanoff", handle_fanoff);
  server.on("/opendoor", handle_opendoor);
  server.on("/closedoor", handle_closedoor);
  server.on("/lighton", handle_lighton);
  server.on("/lightoff", handle_lightoff);
  server.on("/doorautomodeon", handle_doorautomodeon);
  server.on("/doorautomodeoff", handle_doorautomodeoff);
  server.on("/lightautomodeon", handle_lightautomodeon);
  server.on("/lightautomodeoff", handle_lightautomodeoff);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  
}

void loop() {
  server.handleClient();
  
  temperature = aht.readTemperature();
  humidity = aht.readHumidity();

  digitalWrite(LED1pin, LED1status ? LOW : HIGH);
  digitalWrite(LED2pin, LED2status ? LOW : HIGH);
  digitalWrite(LED3pin, LED3status ? HIGH : LOW);
  digitalWrite(LED4pin, LED4status ? HIGH : LOW);
  digitalWrite(LED5pin, LED5status ? HIGH : LOW);
  digitalWrite(FANpin, FANstatus ? LOW : HIGH);

  if (lightAutomaticMode) {
    int ldrValue = analogRead(ldrPin);
    int mapped_val = map(ldrValue, 0, 4095, 0, 255);
    if (mapped_val < ldrThreshold) {
      streetLightStatus = true;
    } else {
      streetLightStatus = false;
    }
    digitalWrite(lightPin, streetLightStatus ? LOW : HIGH);
  }

  if (doorAutomaticMode) {
    int irValue = digitalRead(irPin);
    if (irValue == HIGH) {
      if (!isDoorOpen) {
        delay(3000);
        isDoorOpen = true;
        for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    doorServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
        
      }
    } else {
      if (isDoorOpen) {
        isDoorOpen = false;
        for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    doorServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);
  }
      }
    }
  }
}

void handle_OnConnect() {
  String htmlPage = "<html><head><style>";
  htmlPage += "button {";
  htmlPage += "background-color: #3498db;";
  htmlPage += "border: none;";
  htmlPage += "color: white;";
  htmlPage += "padding: 15px 32px;";
  htmlPage += "text-align: center;";
  htmlPage += "text-decoration: none;";
  htmlPage += "display: inline-block;";
  htmlPage += "font-size: 30px;";
  htmlPage += "margin: 4px 2px;";
  htmlPage += "cursor: pointer;";
  htmlPage += "}";
  htmlPage += "</style></head><body><h1>ESP32 Web Server</h1>";
  htmlPage += "<p>Temperature: " + String(temperature) + " &deg;C</p>";
  htmlPage += "<p>Humidity: " + String(humidity) + " %</p>";
  htmlPage += "<p>Light Sensor Value: " + String(map(analogRead(ldrPin), 0, 4095, 0, 255)) + " </p>";
  
  htmlPage += "<h2>Controls</h2>";
  htmlPage += "<p>Sitting Room Lights: ";
  if(LED1status)
    htmlPage += "<span style=\"color: green;\">ON</span></p><button onclick=\"location.href='/led1off'\">Turn Lights OFF</button>";
  else
    htmlPage += "<span style=\"color: red;\">OFF</span></p><button onclick=\"location.href='/led1on'\">Turn Lights ON</button>";

  htmlPage += "<p>Kitchen Lights: ";
  if(LED2status)
    htmlPage += "<span style=\"color: green;\">ON</span></p><button onclick=\"location.href='/led2off'\">Turn Lights OFF</button>";
  else
    htmlPage += "<span style=\"color: red;\">OFF</span></p><button onclick=\"location.href='/led2on'\">Turn Lights ON</button>";

  htmlPage += "<p>Bedroom Lights: ";
  if(LED3status)
    htmlPage += "<span style=\"color: green;\">ON</span></p><button onclick=\"location.href='/led3off'\">Turn Lights OFF</button>";
  else
    htmlPage += "<span style=\"color: red;\">OFF</span></p><button onclick=\"location.href='/led3on'\">Turn Lights ON</button>";
    
    htmlPage += "<p>Bathroom Lights: ";
  if(LED4status)
    htmlPage += "<span style=\"color: green;\">ON</span></p><button onclick=\"location.href='/led4off'\">Turn Lights OFF</button>";
  else
    htmlPage += "<span style=\"color: red;\">OFF</span></p><button onclick=\"location.href='/led4on'\">Turn Lights ON</button>";

    htmlPage += "<p>Garage Lights: ";
  if(LED5status)
    htmlPage += "<span style=\"color: green;\">ON</span></p><button onclick=\"location.href='/led5off'\">Turn Lights OFF</button>";
  else
    htmlPage += "<span style=\"color: red;\">OFF</span></p><button onclick=\"location.href='/led5on'\">Turn Lights ON</button>";

  htmlPage += "<p>Fan: ";
  if(FANstatus)
    htmlPage += "<span style=\"color: green;\">ON</span></p><button onclick=\"location.href='/fanoff'\">Turn Fan OFF</button>";
  else
    htmlPage += "<span style=\"color: red;\">OFF</span></p><button onclick=\"location.href='/fanon'\">Turn Fan ON</button>";

  htmlPage += "<h2>Door Control</h2>";
  htmlPage += "<p>Door Status: ";
  if (isDoorOpen)
    htmlPage += "<span style=\"color: red;\">CLOSED</span></p><button onclick=\"location.href='/closedoor'\">Open Door</button>";
  else
    htmlPage += "<span style=\"color: green;\">OPEN</span></p><button onclick=\"location.href='/opendoor'\">Close Door</button>";

  htmlPage += "<h2>Street Light Control</h2>";
  htmlPage += "<p>Street Light Status: ";
  if (streetLightStatus)
    htmlPage += "<span style=\"color: green;\">ON</span></p><button onclick=\"location.href='/lightoff'\">Turn Street Light OFF</button>";
  else
    htmlPage += "<span style=\"color: red;\">OFF</span></p><button onclick=\"location.href='/lighton'\">Turn Street Light ON</button>";

  htmlPage += "<h2>Automatic Mode Control</h2>";
  htmlPage += "<p>Door Automatic Mode: ";
  if (doorAutomaticMode)
    htmlPage += "<span style=\"color: green;\">ON</span></p><button onclick=\"location.href='/doorautomodeoff'\">Turn Automatic Mode OFF</button>";
  else
    htmlPage += "<span style=\"color: red;\">OFF</span></p><button onclick=\"location.href='/doorautomodeon'\">Turn Automatic Mode ON</button>";

  htmlPage += "<p>Street Light Automatic Mode: ";
  if (lightAutomaticMode)
    htmlPage += "<span style=\"color: green;\">ON</span></p><button onclick=\"location.href='/lightautomodeoff'\">Turn Automatic Mode OFF</button>";
  else
    htmlPage += "<span style=\"color: red;\">OFF</span></p><button onclick=\"location.href='/lightautomodeon'\">Turn Automatic Mode ON</button>";

  htmlPage += "</body></html>";
  server.send(200, "text/html", htmlPage);
}


void handle_led1on() {
  LED1status = HIGH;
  handle_OnConnect();
}

void handle_led1off() {
  LED1status = LOW;
  handle_OnConnect();
}

void handle_led2on() {
  LED2status = HIGH;
  handle_OnConnect();
}

void handle_led2off() {
  LED2status = LOW;
  handle_OnConnect();
}

void handle_led3on() {
  LED3status = HIGH;
  handle_OnConnect();
}

void handle_led3off() {
  LED3status = LOW;
  handle_OnConnect();
}

void handle_led4on() {
  LED4status = HIGH;
  handle_OnConnect();
}

void handle_led4off() {
  LED4status = LOW;
  handle_OnConnect();
}

void handle_led5on() {
  LED5status = HIGH;
  handle_OnConnect();
}

void handle_led5off() {
  LED5status = LOW;
  handle_OnConnect();
}

void handle_fanon() {
  FANstatus = HIGH;
  handle_OnConnect();
}

void handle_fanoff() {
  FANstatus = LOW;
  handle_OnConnect();
}

void handle_opendoor() {
  doorAutomaticMode = false;
  isDoorOpen = true;
  for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    doorServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
  handle_OnConnect();
}

void handle_closedoor() {
  doorAutomaticMode = false;
  isDoorOpen = false;
  for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    doorServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);
  }
  delay(1000);
  handle_OnConnect();
}

void handle_lighton() {
  lightAutomaticMode = false;
  streetLightStatus = true;
  digitalWrite(lightPin, LOW);
  handle_OnConnect();
}

void handle_lightoff() {
  lightAutomaticMode = false;
  streetLightStatus = false;
  digitalWrite(lightPin, HIGH);
  handle_OnConnect();
}

void handle_doorautomodeon() {
  doorAutomaticMode = true;
  handle_OnConnect();
}

void handle_doorautomodeoff() {
  doorAutomaticMode = false;
  handle_OnConnect();
}

void handle_lightautomodeon() {
  lightAutomaticMode = true;
  handle_OnConnect();
}

void handle_lightautomodeoff() {
  lightAutomaticMode = false;
  handle_OnConnect();
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
