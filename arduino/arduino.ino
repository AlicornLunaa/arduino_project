// Libs

// Variables
const uint8_t btnPin = 6;
const uint8_t ledPin = 13;

bool serialEstablished = false;
char serialBuf[1000];

// Functions
bool checkBufferToStr(const char* str, const char* buffer, const size_t len){
    for(int i = 2; i < len; i++){
        if(str[i - 2] != buffer[i]){
            return false;
        }
    }
    
    return true;
}

void setup() {
    Serial.begin(115200);

    pinMode(ledPin, OUTPUT);
    pinMode(btnPin, INPUT_PULLUP);

    digitalWrite(ledPin, HIGH);
}

void loop() {
    if(!serialEstablished){
        // Start initializing a serial connection, with this method the devices will return success or bad
        if(Serial.available() > 0){
            size_t len = Serial.readBytes(serialBuf, 1000);

            if(checkBufferToStr("success", serialBuf, len)){
                serialEstablished = true;
                digitalWrite(ledPin, LOW);
            }
        }

        // Prevent other logic from running until its established
        return;
    } 

    // Wait for button presses
    if(digitalRead(btnPin) == LOW){
        Serial.print("button1");
        delay(500);
    }
}