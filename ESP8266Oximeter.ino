#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define BLYNK_PRINT Serial
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
 
#include "Wire.h"

#define REPORTING_PERIOD_MS 1000

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
 
char auth[] = "v60fhfJerBZAZzmYbwS2uSzs2Qvy6sjK";             // You should get Auth Token in the Blynk App.
char ssid[] = "TP-LINK_9950";                                     // Your WiFi credentials.
char pass[] = "8348502146";
 
// Connections : SCL PIN - D1 , SDA PIN - D2 , INT PIN - D0
PulseOximeter pox;
 
float BPM, SpO2;
uint32_t tsLastReport = 0;
 
// Callback (registered below) fired when a pulse is detected
 
void onBeatDetected()
{
    Serial.println("Beat!");
}
 
void setup()
{
    Serial.begin(115200);
    
    pinMode(16, OUTPUT);
    Blynk.begin(auth, ssid, pass);
 
    Serial.print("Initializing Pulse Oximeter..");
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
 
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
 
}
 
void loop()
{
    pox.update();
    Blynk.run();
 
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.print(" bpm / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");
 
        Blynk.virtualWrite(V7, BPM);
        Blynk.virtualWrite(V8, SpO2);
 
        tsLastReport = millis();
    }
}
