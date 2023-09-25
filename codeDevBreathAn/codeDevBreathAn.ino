#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

//define ports and display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define BUTTON_PIN 4
#define BUTTON_PIN2 2
#define BUZZER_PIN 8

Adafruit_SH1106 display(OLED_RESET);
// SoftwareSerial nodemcu(5, 6);
float sensor_volt;
float RS_gas;
float R0;
float ratio;
float BAC;
float R2 = 1.02;

// int R2 = 1.02;
const unsigned long MEASURING_DURATION = 5000;  // Measuring duration in milliseconds (5 seconds)
// bool measuringInProgress = false;               // Flag to track if measuring is in progress
// bool measurementCompleted = false;              // Flag to track if measurement is completed
// unsigned long measuringStartTime;

//kod bateriq
const int batteryPin = A0;  // Аналогов пин за измерване на напрежението на батерията

// Спецификации на батерията
const float chargingLimitVoltage = 4.2;  // Максимално напрежение при зареждане
const float ratedVoltage = 3.7;          // Номинално напрежение


// Function to initialize the OLED display
void setupDisplay() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setCursor(0, 32);
  display.setTextColor(WHITE);
  display.println("MQ3 warming up!");
  display.display();
  delay(6000);
  display.clearDisplay();
}

void batteryLevel() {
  int rawValue = analogRead(batteryPin);
  float voltage = (rawValue / 1023.0) * 5.0;

  // Пресмятане на процентите на батерията
  float batteryPercentage = ((voltage - ratedVoltage) / (chargingLimitVoltage - ratedVoltage)) * 100.0;

  if (batteryPercentage < 0.00) {
    batteryPercentage = 0.00;
  }
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setCursor(0, 32);
  display.println("Battery:");
  display.print(batteryPercentage);
  display.println("%");
  display.println(voltage);
  display.display();
  // delay(2000);
  display.clearDisplay();
  delay(5000);
}

// Function to check if the button is pressed to start measuring
bool isButtonPressed1() {
  return (digitalRead(BUTTON_PIN) == LOW);
}
bool isButtonPressed2() {
  return (digitalRead(BUTTON_PIN2) == LOW);
}

// Function to perform the actual measurement
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


  sensorValue = analogRead(A1);
  Serial.println(sensorValue);
  sensor_volt = (float)sensorValue / 1024 * 5.0;
  RS_gas = ((5.0 * R2) / sensor_volt) - R2;
  // R0 = 0.15;
  R0 = 38.23;
  // R0 = 96.87;
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
  // measurementCompleted = true;
  return BAC;
}

void setup() {
  Serial.begin(9600);
  // nodemcu.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  tone(BUZZER_PIN, 1000, 2000);


  setupDisplay();
}

const float BAC_TOLERANCE = 0.01;
bool displayingBattery = false;
void loop() {
  //For UART communication
  // sensorValue = analogRead(A1);
  // StaticJsonBuffer<1000> jsonBuffer;
  // JsonObject& data = jsonBuffer.createObject();
  // data["sensorValue"] = sensorValue;

  // Display the home message
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setCursor(0, 32);
  display.setTextColor(WHITE);
  display.println("Press the right");
  display.println("button to");
  display.println("start measuring!");
  display.display();

  // if (isButtonPressed1() && !measuringInProgress && !measurementCompleted) {
  if (isButtonPressed1()) {
    // Set the measuring flag to true to start measuring
    // measuringInProgress = true;
    measuringStartTime = millis();  // Record the start time of measuring
    delay(1000);                    // Wait for 1 second to avoid accidental double-clicks

    // Show "Measuring..." message for 5 seconds and perform the actual measurement
    float resultBAC;  // opravi promenlivata
    resultBAC = measureBAC();
    Serial.println(resultBAC);
    if (resultBAC <= BAC_TOLERANCE) {  // tuk da se napravi oshte edna proverka za da ne e nushno da se chaka tolkova
      Serial.println("yes");
      delay(2000);  // вариант едно да се пусне по-дълго време
      //вариант 2 да се направи с do while - трябва да се пробва
      do {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.print(resultBAC);
        display.display();
        if(isButtonPressed2()){
          batteryLevel();
          break;
        }
      } while (!isButtonPressed1());
    } else { // tuk da obsudim dali da e kato gore ili da si e taka no s poveche vreme
      Serial.println("no");
      delay(5000);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      if(resultBAC<=0.05){
        display.print("For best result, you need to wait 1.5 minutes to measure again!");
        display.display();d
        elay(90000);
      }else{
        display.print("For best result, you need to wait 3 minutes to measure again!");
        display.display();
        delay(180000);
      }
      
    }
  // } else if (isButtonPressed2() && !displayingBattery) {
  } else if (isButtonPressed2()) {
    // Display battery voltage for 5 seconds
    batteryLevel();
  }
//naj veroqtno za triene
  // Check if the button is pressed to allow another measurement
  // if (isButtonPressed1() && measurementCompleted) {
  //   // Reset the flags for a new measurement
  //   measuringInProgress = false;
  //   measurementCompleted = false;
  // }
  //Send data to NodeMCU
  // data.printTo(nodemcu);
  // jsonBuffer.clear();
}



// // for R0
// float sensor_volt;
// float RS_gas;
// float R0;
// // int R2 = 1000;
// float R2 = 1.02;  //не е проверено

// void setup() {
//   Serial.begin(9600);
// }

// void loop() {
//   int sensorValue = analogRead(A1);
//   sensor_volt = (float)sensorValue / 1024 * 5.0;
//   RS_gas = ((5.0 * R2) / sensor_volt) - R2;
//   R0 = RS_gas / 60;
//   Serial.print("R0: ");
//   Serial.println(R0);
// }





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