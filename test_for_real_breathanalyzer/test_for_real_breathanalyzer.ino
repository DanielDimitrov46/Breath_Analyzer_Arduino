// float sensor_volt;
// float RS_gas;
// float R0;
// int R2 = 1000;

// void setup() {
//  Serial.begin(9600);
// }

// void loop() {
//   int sensorValue = analogRead(A0);
//   sensor_volt=(float)sensorValue/1024*5.0;
//   RS_gas = ((5.0 * R2)/sensor_volt) - R2;
//   R0 = RS_gas / 60;
//   Serial.print("R0: ");
//   Serial.println(R0);
// }

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define BUTTON_PIN 4
Adafruit_SH1106 display(OLED_RESET);


float sensor_volt;
float RS_gas;
float R0;
float ratio;
float BAC;
int R2 = 1.02;

// set sensor to warm up and show it on display - remember to change the delay to higher
void setup() {
  Serial.begin(9600);
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 32);
  Serial.println("MQ3 warming up!");
  display.println("MQ3 warming up!");
  display.display();
  delay(2000);  // allow the MQ3 to warm up
  display.clearDisplay();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

// code for start meausoring
byte buttonState;
bool realTesting() {
buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    Serial.println("Button is pressed");
    return true;
  } else {
    Serial.println("Button is not pressed");
    return false;
  }
  delay(100);
}

// function for showing the message while measure.

void loop() {
   display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 32);
    // display.print("PPM ");
    display.print("Press the button to start measuring! ");
    display.display();
  if (realTesting()) {
      display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 32);
    // display.print("PPM ");
    display.print("Measuring...");
    display.display();
    delay(3000);
    while(true){
      //
      //
      display.clearDisplay();


    int sensorValue = analogRead(A0);
    Serial.println(sensorValue);
    sensor_volt = (float)sensorValue / 1024 * 5.0;
    RS_gas = ((5.0 * R2) / sensor_volt) - R2;
    /*-Replace the value of R0 with the value of R0 in your test -*/
    // R0 = 73.63;
    R0 = 30.23;
    ratio = RS_gas / R0;  // ratio = RS/R0
    double x = 0.4 * ratio;
    BAC = pow(x, -1.431);  //BAC in mg/L
    Serial.print("BAC = ");
    Serial.print(BAC * 0.0001);  //convert to g/dL
    Serial.print(" g/DL\n\n");
    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 32);
    // display.print("PPM ");
    display.print("BAC = ");
    display.print(BAC * 0.0001);
    display.print(" g/DL\n\n");
    display.display();
    delay(1000);
    }
    

  } else {
    Serial.print("Nothing");
  }
}
