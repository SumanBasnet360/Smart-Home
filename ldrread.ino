
#define LDR 34
#define LED_PIN           2  // ESP32 pin GIOP22 connected to LED
#define ANALOG_THRESHOLD  100

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  int LDR_val = analogRead(4);
  int mapped_val = map(LDR_val, 0, 4095, 0, 255);
  Serial.println(mapped_val);
  if (mapped_val < ANALOG_THRESHOLD)
    digitalWrite(LED_PIN, HIGH); // turn on LED
  else
    digitalWrite(LED_PIN, LOW);  // turn off LED
  delay(100);

}
