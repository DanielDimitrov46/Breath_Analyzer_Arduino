#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);


#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif

/////////////Change your data below/////////////
////////////////////////////////////////////////
const int MQ_PIN = A0;    // Analog pin connected to sensor
float RL_VALUE = 1.02;      // RL Resistance in Kilo ohms
// float  R0 = 0.018;        // R0 Resistance in kilo ohms (use other code to get it)
float  R0 = 36.34;        // R0 Resistance in kilo ohms (use other code to get it)



// Variables we need in the code
const int READ_SAMPLE_INTERVAL = 100;  //Time between samples
const int READ_SAMPLE_TIMES = 5;       //Amount of samples
// Add next points values according to your dataseheet Rs/R0 graph
const float X0 = 50;
const float Y0 = 0.18;
const float X1 = 500;
const float Y1 = 0.012;


// Calculate the curve points {X, Y}
const float punto0[] = { log10(X0), log10(Y0) };
const float punto1[] = { log10(X1), log10(Y1) };
// Calculate the values of the graph
const float scope = (punto1[1] - punto0[1]) / (punto1[0] - punto0[0]);
const float coord = punto0[1] - punto0[0] * scope;


void setup(){
  pinMode(MQ_PIN, INPUT);
  Serial.begin(9600);

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  delay(100);
}


void loop()
{
  float rs_med = readMQ(MQ_PIN);      // Get mesn RS resistance value 
  //Serial.println(rs_med,7);
  float concentration = getConcentration(rs_med/R0);   // Get concentration
  
  // Display the value

  float BAC = map(concentration, 20, 500, 7, 184); //20 to 500 scale is from the MQ-3 datasheet graph
  BAC = BAC/1000;  
  Serial.print("BAC: ");
  Serial.println(BAC,3);

  // display.clearDisplay();
  // display.setTextSize(2);
  // display.setTextColor(WHITE);
  // display.setCursor(0,25);
  // display.print("PPM ");
  // display.println(concentration,0);
  // display.display();
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,25);
  display.print("BEC ");
  display.println(BAC,4);
  display.display();
  
}



// Obtener la resistencia promedio en N muestras
float readMQ(int mq_pin)
{
   float rs = 0;
   for (int i = 0;i<READ_SAMPLE_TIMES;i++) {
      rs += getMQResistance(analogRead(mq_pin));
      delay(READ_SAMPLE_INTERVAL);
   }
   return rs / READ_SAMPLE_TIMES;
}

// Obtener resistencia a partir de la lectura analogica
float getMQResistance(int raw_adc)
{
   return (((float)RL_VALUE / 1000.0*(1023 - raw_adc) / raw_adc));
}


// Obtener concentracion 10^(coord + scope * log (rs/r0)
float getConcentration(float rs_ro_ratio)
{
   return pow(10, coord + scope * log(rs_ro_ratio));
}
