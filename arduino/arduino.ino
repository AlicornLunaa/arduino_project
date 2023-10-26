// Libs

// Variables
const uint8_t btnPin = 6;

// Functions
void setup() {
    Serial.begin(115200);
    pinMode(btnPin, INPUT_PULLUP);
}

void loop() {
    if(digitalRead(btnPin) == LOW){
        Serial.println("button1");
        delay(500);
    }
}