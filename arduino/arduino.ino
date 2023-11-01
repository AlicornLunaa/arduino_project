// Libs

// Variables
const uint8_t btnPin = 6;
const uint8_t ledPin = 13;

bool serialEstablished = false;
bool serial1Established = false;
char serialBuf[1000];

// Functions
bool checkBufferToStr(const char* str, const char* buffer, const size_t len){
    int offset = 0;

    for(int i = 0; i < len; i++){
        if((uint8_t)buffer[i] == 240){
            offset++;
            continue;
        }

        if(str[i - offset] != buffer[i]){
            return false;
        }
    }
    
    return true;
}

void serialWriteStr(const char* str){
    if(serial1Established){
        Serial1.print(str);
    } else if(serialEstablished){
        Serial.print(str);
    }
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);

    pinMode(ledPin, OUTPUT);
    pinMode(btnPin, INPUT_PULLUP);

    digitalWrite(ledPin, HIGH);
}

void loop() {
    if(!serialEstablished && !serial1Established){
        // Start initializing a serial connection, with this method the devices will return success or bad
        if(Serial.available() > 0){
            size_t len = Serial.readBytes(serialBuf, 1000);

            if(checkBufferToStr("success", serialBuf, len)){
                serialEstablished = true;
                digitalWrite(ledPin, LOW);
            }
        } else if(Serial1.available() > 0){
            size_t len = Serial1.readBytes(serialBuf, 1000);

            if(checkBufferToStr("success", serialBuf, len)){
                serial1Established = true;
                digitalWrite(ledPin, LOW);
            }
        }

        // Prevent other logic from running until its established
        return;
    } 

    // Wait for button presses
    if(digitalRead(btnPin) == LOW){
        serialWriteStr("button1");
        delay(500);
    }
}