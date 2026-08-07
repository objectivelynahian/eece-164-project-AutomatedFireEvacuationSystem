#include <Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================
// Pin Definitions
// ============================

const int DHT_PIN = 2;

const int BLUE_LED = 3;
const int RED_LED  = 5;

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
// OLED Definitions
// ============================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool displayConnected = false;

// ============================
// Objects
// ============================

DHT dht(DHT_PIN, DHT22);
Servo doorServo;

// ============================
// Function Prototypes
// ============================

void showBootScreen();
void showDhtError();
void showMq2Error();
void showStatusScreen(float temperature, float humidity, int smoke, bool fireDetected);

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

    // OLED init
    displayConnected = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    if (displayConnected)
    {
        showBootScreen();
    }
    else
    {
        Serial.println("OLED not detected. Continuing without display...");
    }

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

        showDhtError();

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

        showMq2Error();

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

    showStatusScreen(temperature, humidity, smoke, fireDetected);

    Serial.println();

    delay(500);
}

// ============================
// OLED Helper Functions
// ============================

void showBootScreen()
{
    if (!displayConnected) return;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("SMART");

    display.setCursor(0, 20);
    display.println("FIRE");

    display.setCursor(0, 40);
    display.println("SYSTEM");

    display.display();

    delay(2000);

    display.clearDisplay();
    display.display();
}

void showDhtError()
{
    if (!displayConnected) return;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("ERROR");

    display.setTextSize(1);
    display.println();
    display.println("DHT22 Failure");

    display.display();
}

void showMq2Error()
{
    if (!displayConnected) return;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("ERROR");

    display.setTextSize(1);
    display.println();
    display.println("MQ-2 Failure");

    display.display();
}

void showStatusScreen(float temperature, float humidity, int smoke, bool fireDetected)
{
    if (!displayConnected) return;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.print("Temp: ");
    display.print(temperature, 1);
    display.println(" C");

    display.print("Hum : ");
    display.print(humidity, 0);
    display.println("%");

    display.print("Smoke: ");
    display.println(smoke);

    display.println();

    if (fireDetected)
    {
        display.print("Status: FIRE!");
    }
    else
    {
        display.print("Status: NORMAL");
    }

    display.display();
}