// Libs
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Definitions
// #define DEBUG_SERIAL
#define URL_START "http://"

// Variables
const char* wirelessSSID = "Clydesdale";
const char* wirelessPassword = "arduino_project";
const uint16_t serverPort = 3000;
String url = URL_START;

// Functions
void assignServerURL(IPAddress addr){
    // Take the current IP Address and point the URL target to the router.
    // The router will always hold the webserver, as this is a demo project
    // where I can guarantee environment conditions.
    url = URL_START;
    url += addr.toString();
    url += ":";
    url += String(serverPort);
}

bool sendRequest(WiFiClient client, String req){
    // Send request to server
    HTTPClient http;

    if(http.begin(client, url + req)) {
        // Get server's response
        int httpCode = http.GET();
        
        if(httpCode > 0)
            // Print data
            Serial.print(http.getString());

        // Cleanup connection
        http.end();
        return true;
    }

    return false;
}

void setup() {
    // User feedback
    Serial.begin(115200);

    #ifdef DEBUG_SERIAL
    Serial.print("Connecting");
    #else
    Serial.print("connect");
    #endif

    // Start connection to internet
    WiFi.begin(wirelessSSID, wirelessPassword);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);

        #ifdef DEBUG_SERIAL
        Serial.print(".");
        #endif
    }

    // Get server
    assignServerURL(WiFi.gatewayIP());

    // User feedback
    #ifdef DEBUG_SERIAL
    Serial.print(" Done!\n");
    Serial.print("Connected to ");
    Serial.print(wirelessSSID);
    Serial.print(" with IP address: ");
    Serial.println(WiFi.localIP());
    #else
    Serial.print("success");
    #endif
}

void loop() {
    // Sending data to server
    if(Serial.available() > 0){
        String str = Serial.readString();
        
        if(WiFi.status() == WL_CONNECTED) {
            // Start connection
            WiFiClient client;
            sendRequest(client, str);
        }
    }
}