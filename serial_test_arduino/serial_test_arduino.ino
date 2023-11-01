// Functions
void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
}

void loop() {
    if(Serial.available() > 0){
        String msg = Serial.readString();
        Serial1.print(msg);
    }

    if(Serial1.available() > 0){
        String msg = Serial1.readString();
        Serial.println(msg);
    }
}