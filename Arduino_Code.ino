#define NOP __asm__("nop")
// LED pattern
unsigned char pattern = 0b00000000; // Initial state: all RED
int sensorCount = 4;
// Time constants
const long baseGreenDelay = 50; // Base time for green light in milliseconds
const long redDelay = 50;   // Time for red light in milliseconds
// Sensor Pins
int sensorPin[4] = {A0, A1, A2, A3};
// SHR control
int SER = 2;     //pin-14 in 595 
int RST = 3;    //pin-10 in 595
int inClk = 4;    //pin-11 in 595
int latchpin = 5;   //pin-12 in 595

//*************************************************************************
void init_Port() {
    for (int i = 0; i < sensorCount; i++) {
        pinMode(sensorPin[i], INPUT);
    }
}

//*************************************************************************
void init_Light() {
    pinMode(SER, OUTPUT);
    pinMode(RST, OUTPUT);
    pinMode(inClk, OUTPUT);
    pinMode(latchpin, OUTPUT);
    digitalWrite(SER, LOW);
    digitalWrite(RST, LOW);
    delay(500);
    digitalWrite(RST, HIGH);
}

//*************************************************************************
void write_LED(unsigned char val) {
    digitalWrite(latchpin, LOW);
    NOP;

   // digitalWrite(inClk, LOW);
    NOP;

    shiftOut(SER, inClk, MSBFIRST, val);
    NOP;

    digitalWrite(latchpin, HIGH);
    NOP;
    NOP;
    NOP;

    //digitalWrite(latchpin, LOW);
    NOP;

    //digitalWrite(inClk, LOW);
    NOP; 
}

// Function to display the current LED signal
void displaySignal() {
    write_LED(pattern);
}

//*************************************************************************
void updatePattern(int state) {
    // Reset the pattern to RED for all
    pattern = 0b00000000;

    // Update pattern based on the current state
    switch (state) {
        case 0: // North Green
            pattern |= 0b01010110; // Set North to Green
            break;
        case 1: // East Green
            pattern |= 0b01011001; // Set East to Green
            break;
        case 2: // South Green
            pattern |= 0b01100101; // Set South to Green
            break;
        case 3: // West Green
            pattern |= 0b10010101; // Set West to Green
            break;
    }
}

// Function to read the vehicle density from sensors
int readDensity(int sensorId) {
    return analogRead(sensorPin[sensorId]); // Adjust according to your sensor type
}

//*************************************************************************
void setup() {
    Serial.begin(9600);
    Serial.println(" ------- Automatic Traffic Light Control based on Density --------");

    // SHR Init
    init_Light();
    init_Port();
}

//*************************************************************************
void loop() {
    for (int state = 0; state < sensorCount; state++) {
        // Read the density for the current state
        int density = readDensity(state);
        Serial.print("density: ");
        Serial.println(density);
        // Calculate green light duration based on density
        long greenDelay = baseGreenDelay;
        if (density > 800) {
            greenDelay += 4000; // High traffic: increase green duration
        }
        else if(density > 400){
            greenDelay += 2000;
        } 
        updatePattern(state);
        Serial.print("state: ");
        Serial.println(state);
        Serial.print("p: ");
        Serial.println(pattern);
        displaySignal();
        delay(greenDelay); // Dynamic green light duration
        displaySignal();
        delay(redDelay); // Fixed time for red light
    }
}
