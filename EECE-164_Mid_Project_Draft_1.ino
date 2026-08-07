#include <Servo.h>
#include <DHT.h>

// ============================
// Pin Definitions
// ============================

const int DHT_PIN = 2;

const int BLUE_LED = 3;
const int RED_LED  = 4;

const int BUZZER = 8;
const int SERVO_PIN = 9;

const int MQ2_PIN = A0;

// ============================
// Thresholds
// ============================

const float TEMP_THRESHOLD = 35.0;
const int SMOKE_THRESHOLD = 650;
const int MQ2_ERROR = 1020;

// ============================
// Objects
// ============================

DHT dht(DHT_PIN, DHT22);
Servo doorServo;

// ============================

void setup()
{
    Serial.begin(9600);

    pinMode(BLUE_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    dht.begin();

    doorServo.attach(SERVO_PIN);

    // Initial State
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    doorServo.write(0);      // Door closed

    Serial.println("==================================");
    Serial.println(" Smart Fire Evacuation System");
    Serial.println("==================================");
}

void loop()
{
    //----------------------------
    // Read Sensors
    //----------------------------

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int smoke = analogRead(MQ2_PIN);

    //----------------------------
    // Sensor Error Check
    //----------------------------

    if (isnan(temperature) || isnan(humidity))
    {
        Serial.println("ERROR: DHT22 disconnected!");

        digitalWrite(BLUE_LED, LOW);
        digitalWrite(RED_LED, HIGH);

        digitalWrite(BUZZER, LOW);

        doorServo.write(0);

        delay(1000);
        return;
    }

    if (smoke >= MQ2_ERROR)
    {
        Serial.println("ERROR: MQ-2 sensor fault!");

        digitalWrite(BLUE_LED, LOW);
        digitalWrite(RED_LED, HIGH);

        digitalWrite(BUZZER, LOW);

        doorServo.write(0);

        delay(1000);
        return;
    }

    //----------------------------
    // Print Readings
    //----------------------------

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C");

    Serial.print("   Humidity: ");
    Serial.print(humidity);
    Serial.print("%");

    Serial.print("   Smoke: ");
    Serial.println(smoke);

    //----------------------------
    // Decision Logic
    //----------------------------

    bool fireDetected =
        (temperature >= TEMP_THRESHOLD) ||
        (smoke >= SMOKE_THRESHOLD);

    //----------------------------
    // Outputs
    //----------------------------

    if (fireDetected)
    {
        Serial.println("*** FIRE DETECTED ***");

        digitalWrite(BLUE_LED, LOW);
        digitalWrite(RED_LED, HIGH);

        digitalWrite(BUZZER, HIGH);

        doorServo.write(180);      // Open emergency exit
    }
    else
    {
        digitalWrite(BLUE_LED, HIGH);
        digitalWrite(RED_LED, LOW);

        digitalWrite(BUZZER, LOW);

        doorServo.write(0);        // Keep door closed
    }

    Serial.println();

    delay(500);
}