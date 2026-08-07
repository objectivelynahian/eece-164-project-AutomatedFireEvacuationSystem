#include <Servo.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22

Servo doorServo;
DHT dht(DHTPIN, DHTTYPE);

const int blue = 3;
const int buzz = 8;
const int servo = 9;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buzz, OUTPUT);
  doorServo.attach(servo);
  // DHT22
  Serial.begin(9600);
  dht.begin();
}

void loop() {

  digitalWrite(LED_BUILTIN, LOW);

  digitalWrite(blue, HIGH);
  //digitalWrite(buzz, HIGH);
  //doorServo.write(0);

  float temperature = dht.readTemperature();

  if (temperature >= 35.0) {
    Serial.println("WARNING: High temperature!");
  } else {
    Serial.println("Temperature is normal.\n");
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  delay(500);

  digitalWrite(blue, LOW);
  digitalWrite(buzz, LOW);
  //doorServo.write(180);
  delay(500);
}