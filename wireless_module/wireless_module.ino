// Libs
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Definitions
#define DEBUG_SERIAL
#define URL_START "http://"

// Variables
const char* wirelessSSID = "Clydesdale";
const char* wirelessPassword = "unicorn!";
const uint16_t serverPort = 3000;

String url = URL_START;
String data = "/test";

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

void setup() {
    // User feedback
    #ifdef DEBUG_SERIAL
    Serial.begin(115200);
    Serial.print("Connecting");
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
    #endif
}

void loop() {
    // Sending data to server
    if(WiFi.status() == WL_CONNECTED) {
        // Start connection
        WiFiClient client;
        HTTPClient http;

        #ifdef DEBUG_SERIAL
        Serial.println("============= Start Response =============");
        Serial.println("Requesting: " + url + data);
        #endif

        // Send request to server
        if(http.begin(client, url + data)) {
            // Get server's response
            int httpCode = http.GET();
            
            #ifdef DEBUG_SERIAL
            Serial.println("Response code: " + String(httpCode));

            if(httpCode > 0)
                // Print data
                Serial.println(http.getString());
            #endif

            // Cleanup connection
            http.end();
        } else {
            // Error reporting
            #ifdef DEBUG_SERIAL
            Serial.printf("[HTTP] Unable to connect\n");
            #endif
        }

        #ifdef DEBUG_SERIAL
            Serial.println("============== End Response ==============\n");
        #endif
    }

    // Dont overload the server
    delay(5000);
}