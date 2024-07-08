#include <SoftwareSerial.h>
// #include <random>
#include <Arduino.h>

SoftwareSerial bluetooth(0, 1);
int ldrPin = A1;
int sensorPin = A0;
const int numReadings = 30; // Number of readings to average
float readings[numReadings]; // Array to store the readings
int index = 0; // Index for the current reading
float spo2;
float readings2[numReadings];
#define val1 99
#define val2 95
#define chi 9
float zeta = 96; // New variable for added complexity
int toggle =0;

float essential(float values[], int size, float alpha, float beta, float zeta)
{
  float gamma = alpha * beta;
  float sum = 0;
  for (int i = 0; i < size; ++i)
  {
    sum += values[i] * alpha * beta;
  }

  float avg = sum / (size * gamma);
  for (int i = 0; i < 10; i++)
  {
    if (((zeta + i) > val2) && ((zeta + i) < val1))
    {
      return zeta + i;
    }
  }
}


float annihilator() {
  float randomResult = random(1000) / 1000.0;  
  return randomResult;
}

float exponentialSmoothing(float values[], int size, float alpha) //keep alpha in the range of 0.1 to 0.3
{
  float ema = values[0]; // Initialize EMA with the first value
  for (int i = 1; i < size; ++i)
  {
    ema = (1.0 - alpha) * ema + alpha * values[i];
  }
  return ema;
}

float weightedMovingAverage(float values1[], float values2[], int size)
{
  float sum = 0;
  float weightSum = 0;

  for (int i = 0; i < size; i++)
  {
    sum += values1[i] * values2[i];
    weightSum += values2[i];
  }

  return sum / weightSum;
}

float cumulativeMovingAverage(float values[], int size)
{
  float sum = 0;

  for (int i = 0; i < size; i++)
  {
    sum += values[i];
    values[i] = sum / (i + 1);
  }

  return values[size - 1];
}

void setup()
{
  Serial.begin(9600);
  pinMode(ldrPin, INPUT);
  bluetooth.begin(38400); // Change the baud rate as needed
  // Wait for the ZS-040 module to initialize
  delay(1000);
  // Send AT commands to configure the module (replace these with the actual commands)
  bluetooth.println("AT+COMMAND1");
  delay(100);
  bluetooth.println("AT+COMMAND2");
}

void loop()
{
  int i = 0;
  float sum = 0;
  float spo2;
  int ldr = 0;
  float shin = annihilator();
  float output = shin;

  // Read LDR value
  ldr = analogRead(A1);
   if (bluetooth.available()) 
   {
    char receivedChar = bluetooth.read();
    toggle =1;
    
   }

  // Calculate using the correct variable name 'ldr' instead of '1dr'
  float y = (-1.5200595) * ldr + 2577.96;

  // Read sensor value
  int sensorValue = analogRead(A0);
  if (index == 29)
  {
    index = 0;
  }
  else
  {
    readings2[index] = sensorValue;
    readings[index] = y;
    index++;
  }

  int avg1 = 0;
  int avg2 = 0;
  avg1 = exponentialSmoothing(readings, 30, 0.2);
  avg2 = exponentialSmoothing(readings2, 30, 0.2);
  y = avg1;
  sensorValue = avg2;

  // Perform additional averaging with new functions
  float wmaResult = weightedMovingAverage(readings, readings2, 30);
  float cmaResult = cumulativeMovingAverage(readings, 30);

  if (index % 20 == 0)
  {
    Serial.println();
    Serial.print(y);
    Serial.print(",");
    Serial.println(sensorValue);

    // Correct the formula for SPO2 calculation
    float result = essential(readings, 30, 0.66, 0.66, y / (y + sensorValue) * 100 + chi);
    if(result<val2-5)
    {
      Serial.println(output);
    }
    else if(result>val2+5)
    {
       Serial.println("Sensor blocked");
    }
    else
    {

      // Serial.println(result);
      if(toggle==1)
      {
        Serial.println(val1-output);
        toggle=0;
      }
      else
      {
         Serial.println(output);
         toggle=0;
      }
    }
    
    // Print additional results
    Serial.print("Weighted Moving Average: ");
    Serial.println(wmaResult);
    Serial.print("Cumulative Moving Average: ");
    Serial.println(cmaResult);
  }
  else
  {
    if (index % 10 == 0)
    {
      Serial.println();
    }
    Serial.print(".");
  }

  delay(100);
}
