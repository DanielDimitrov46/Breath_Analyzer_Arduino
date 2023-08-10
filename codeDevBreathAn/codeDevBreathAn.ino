//REAL CODE

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define BUTTON_PIN 4
#define BUZZER_PIN 8  // Change this to the actual pin connected to the passive buzzer


Adafruit_SH1106 display(OLED_RESET);

float sensor_volt;
float RS_gas;
float R0;
float ratio;
float BAC;
int R2 = 1.02;

const unsigned long MEASURING_DURATION = 5000;  // Measuring duration in milliseconds (5 seconds)
const unsigned long ONE_MINUTE = 60000;         // One minute in milliseconds
bool measuringInProgress = false;               // Flag to track if measuring is in progress
bool measurementCompleted = false;              // Flag to track if measurement is completed
unsigned long measuringStartTime;

// Function to initialize the OLED display
void setupDisplay() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.println("MQ3 warming up!");
  display.display();
  delay(2000);
  display.clearDisplay();
}

// Function to check if the button is pressed to start measuring
bool isButtonPressed() {
  return (digitalRead(BUTTON_PIN) == LOW);
}

// Function to perform the actual measurement and calculate BAC
float measureBAC() {
  int sensorValue;
  unsigned long elapsedTime;

  while ((elapsedTime = millis() - measuringStartTime) < MEASURING_DURATION) {
    tone(BUZZER_PIN, 3000);
    // delay(duration);

    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 32);
    display.print("Measuring...");
    display.print(elapsedTime);
    display.display();
  }
  noTone(BUZZER_PIN);

  // Calculate the BAC after 5 seconds of measuring
  sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  sensor_volt = (float)sensorValue / 1024 * 5.0;
  RS_gas = ((5.0 * R2) / sensor_volt) - R2;
  /*-Replace the value of R0 with the value of R0 in your test -*/
  // R0 = 73.63;
  R0 = 30.23;
  // R0 = 55.34;
  ratio = RS_gas / R0;  // ratio = RS/R0
  double x = 0.4 * ratio;
  BAC = pow(x, -1.431);  //BAC in mg/L
  Serial.println(BAC);

  // Display the BAC result
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.print("BAC = ");
  BAC = BAC * 0.0001;
  display.print(BAC);
  display.print(" g/DL\n\n");
  display.println(sensorValue);
  display.display();

  // Set the measurementCompleted flag to true
  measurementCompleted = true;
  return BAC;
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);  // Set the buzzer pin as an output
  tone(BUZZER_PIN, 1000, 2000);


  setupDisplay();
}

// void playBuzzerSound(unsigned int frequency, unsigned long duration) {
//   tone(BUZZER_PIN, frequency, duration);
//   delay(duration);
//   noTone(BUZZER_PIN);
// }

const float BAC_TOLERANCE = 0.01;
void loop() {
  // Display the initial message
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  display.println("Press the button to");
  display.println("start measuring!");
  display.display();

  if (isButtonPressed() && !measuringInProgress && !measurementCompleted) {
    // Set the measuring flag to true to start measuring
    measuringInProgress = true;
    measuringStartTime = millis();  // Record the start time of measuring
    delay(1000);                    // Wait for 1 second to avoid accidental double-clicks

    // Show "Measuring..." message for 5 seconds and perform the actual measurement
    //  playBuzzerSound(3000, MEASURING_DURATION);
    float forDelay;
    forDelay = measureBAC();
    Serial.println(forDelay);
    if (forDelay <= BAC_TOLERANCE) {
      Serial.println("yes");
      delay(2000);
    } else {
      Serial.println("no");
      delay(5000);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("For best result, you need to wait 3 minutes to measure again!");
      display.display();
      delay(180000);
    }
  }

  // Check if the button is pressed to allow another measurement
  if (isButtonPressed() && measurementCompleted) {
    // Reset the flags for a new measurement
    measuringInProgress = false;
    measurementCompleted = false;
  }
}



//FOR BUZZERS


// int buzzerPin = 8;

// void setup() {
//   pinMode(buzzerPin, OUTPUT);

//   tone(buzzerPin, 1000, 2000);
// }

// void loop() {
//   tone(buzzerPin, 3000); // A4
//   delay(5000);

// tone(buzzerPin, 494); // B4
// delay(1000);

// tone(buzzerPin, 523); // C4
// delay(1000);

// tone(buzzerPin, 587); // D4
// delay(1000);

// tone(buzzerPin, 659); // E4
// delay(1000);

// tone(buzzerPin, 698); // F4
// delay(1000);

// tone(buzzerPin, 784); // G4
// delay(1000);

// noTone(buzzerPin);
// delay(5000);
// }


// int buzzerPin = 8;
// int buttonPin = 4;

// void setup() {
//   pinMode(buzzerPin, OUTPUT);
//   pinMode(buttonPin, INPUT_PULLUP);
// }

// void loop() {
//   int buttonState = digitalRead(buttonPin);

//   if (buttonState == LOW) {
//     digitalWrite(buzzerPin, HIGH);
//   }

//   if (buttonState == HIGH) {
//     digitalWrite(buzzerPin, LOW);
//   }
// }