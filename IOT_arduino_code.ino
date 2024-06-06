#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns, 2 rows

const int trigPin = 11;
const int echoPin = 12;
const int buzzerPin = 2;
const int ledPin = 7;
#define sensor_pin A0  
const int turbidityPin = A0;  // Analog pin for turbidity sensor

int read_ADC;
int ntu;

// DS18B20 temperature sensor setup
OneWire oneWire(3);  // Connect DS18B20 data pin to digital pin 9
DallasTemperature sensors(&oneWire);

// Bluetooth module setup
SoftwareSerial bluetooth(6, 5); // RX, TX pins for Bluetooth (connect TX of HC-05 to pin 5, RX to pin 6)

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(sensor_pin, INPUT);

  // DS18B20 temperature sensor setup
  sensors.begin();

  // Bluetooth module setup
  bluetooth.begin(9600);
  Serial.println("Bluetooth connected");

  // LCD display setup
  lcd.init();       // Initialize the LCD
  lcd.backlight();   // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Project");
  lcd.setCursor(1, 0);
  lcd.print("Group I");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the echo duration
  long duration = pulseIn(echoPin, HIGH);

  // Calculate distance in centimeters
  int distance = duration * 0.034 / 2;

  // Print the distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Check the distance and perform actions accordingly
  if (distance < 10) {
    // If the distance is less than 10 cm, close proximity
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    bluetooth.print("Close,");
  } else if (distance >= 10 && distance < 30) {
    // If the distance is between 10 and 30 cm, medium proximity
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    bluetooth.print("Medium,");
  } else {
    // If the distance is 30 cm or more, far proximity
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, HIGH);
    bluetooth.print("Far,");
  }

  // Read temperature from DS18B20 sensor
  sensors.requestTemperatures();
  float temperatureCelsius = sensors.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.print(temperatureCelsius);
  Serial.println(" °C");

  // Send temperature data over Bluetooth
  bluetooth.print(temperatureCelsius);
  bluetooth.print(",");

  // Read turbidity from sensor
  int read_ADC = analogRead(turbidityPin);
  if (read_ADC > 208) read_ADC = 208;
  int ntu = map(read_ADC, 0, 208, 300, 0);

  // Print turbidity to Serial Monitor
  Serial.print("Turbidity: ");
  Serial.println(ntu);

  lcd.setCursor(0, 0);
  lcd.print("Turbidity: ");
  lcd.print(ntu);
  lcd.print("     l");
  lcd.setCursor(0, 1);//set cursor (column by row) indexing from 0

  if (ntu < 10)
    lcd.print("Water Very Clean");
  else if (ntu >= 10 && ntu < 80)
    lcd.print("Water Norm Clean");
  else
    lcd.print("Water Very Dirty");

  // Activate the buzzer when turbidity is maximum
  if (ntu >= 80) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    

  } else {
    digitalWrite(buzzerPin, LOW);
  }

  // Display turbidity on LCD for 2 seconds
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print(" cm");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperatureCelsius);
  lcd.print(" C");
  
  // Send turbidity data over Bluetooth
  bluetooth.print(ntu);
  bluetooth.print(";");

  // Delay before the next measurement
  delay(2000);
}
