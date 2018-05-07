#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "example.firebaseio.com"
#define FIREBASE_AUTH "token_or_secret"
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"
#define FLOW_PIN 2

double flowRate, totalFlow;
volatile int count; // volatile to ensure it updates correctly during the interrupt process


void setup() {
    //setup flow sensor
    pinMode(FLOW_PIN, INPUT);
    attachInterrupt(0, flow, RISING); // configures interrupt 0 (pin 2 on the Arduino) to run the function "flow()"

    Serial.begin(9600);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.print("Connected: ");
    Serial.println(WiFi.localIP());

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

void loop() {
    count = 0; // reset to start counting from 0 again
    interrupts(); //Enable interrupt in Arduino
    delay(1000); //wait one second
    noInterrupts(); //disable interrupts

    /*
    quick maths
    -take counted pulses in last second and multiple by 2.25 mL
    -multiply that by 60 to convert seconds to minutes, giving us mL / min
    -divide by 1000 to convert mL to L, giving us L / min
    */
    totalFlow += count * 2.25;
    flowRate = (count * 2.25 * 60) / 1000; 
    
    Firebase.setFloat("flow_rate", flowRate);
    if (Firebase.failed()) {
        Serial.print("setting flow_rate failed:");
        Serial.println(Firebase.error());  
        return;
    }

    Firebase.setFloat("total_rate", flowRate);
    if (Firebase.failed()) {
        Serial.print("setting total_rate failed:");
        Serial.println(Firebase.error());  
        return;
    }
}

void flow() {
    count++;
}