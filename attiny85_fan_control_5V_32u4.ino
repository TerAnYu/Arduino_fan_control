// OneWire DS18B20 Temperature Switch on ATtiny85
// http://www.nerdkits.com/forum/thread/2849/
// source: http://www.electroschematics.com/9540/arduino-fan-speed-controlled-temperature/

#include <OneWire.h>

// fan switch temperatures
#define TEMP_MIN 30 // C°
#define TEMP_MAX 50 // C°

// #define FT_START 3000 // msec
#define FT_START 5000 // msec
#define DELAY_TIMEOUT    1000
#define PWM_MIN 60 // минимальное значение pwm
#define PWM_MAX 255 // максимальное значение pwm

// pins
#define DS_1 21
#define DS_2 20

// #define STATUS_LED  2
#define FAN_1 5
#define FAN_2 6

// read speed
#define FANRPM_1 19
#define FANRPM_2 18
  
OneWire TemperatureSensor1(DS_1);  // Dallas one wire data buss pin, a 4.7K resistor pullup is needed
OneWire TemperatureSensor2(DS_2);  // Dallas one wire data buss pin, a 4.7K resistor pullup is needed
 
void setup() {
    Serial.begin(9600);
    
    pinMode(FAN_1, OUTPUT);
    pinMode(FAN_2, OUTPUT);
    
    // pinMode(STATUS_LED, OUTPUT);

    // delay(1000);
    // расскрутить после включения на максимум на время FT_START и после выставить минимальную скорость
    analogWrite(FAN_1, PWM_MAX);
    analogWrite(FAN_2, PWM_MAX);
    delay(FT_START);
    analogWrite(FAN_1, PWM_MIN);
    analogWrite(FAN_2, PWM_MIN);

    pinMode(FANRPM_1, INPUT);
    pinMode(FANRPM_2, INPUT);
}

void loop() {
    delay(DELAY_TIMEOUT);
    Serial.print(F("ch1: "));
    int temp1 = ReadSensor(&TemperatureSensor1);
    SetFanSpeedWithTemp(FAN_1, temp1);
    Serial.print(F("ch2: "));
    int temp2 = ReadSensor(&TemperatureSensor2);
    SetFanSpeedWithTemp(FAN_2, temp2);
}

int16_t ReadSensor(OneWire *Sensor) {
    byte data[12];
    int16_t raw = 0;
    int16_t celsius = 0;

    Sensor->reset();       // reset one wire buss
    Sensor->skip();        // select only device
    Sensor->write(0x44);   // start conversion
    // delay(1000);        // wait for the conversion 
    Sensor->reset();
    Sensor->skip();
    Sensor->write(0xBE);   // Read Scratchpad
    for (byte i = 0; i < 9; i++) {       // 9 bytes
        data[i] = Sensor->read();
    } 
    // Convert the data to actual temperature
    raw = (data[1] << 8) | data[0];
    celsius = raw / 16;
    return celsius;
}

void SetFanSpeedWithTemp(int fanPin, int temp) {
    int fanSpeed = PWM_MIN;
    // if(temp < TEMP_MIN) {   // if temp is lower than minimum temp
        // fanSpeed = 0;      // fan is not spinning
        // digitalWrite(fanPin, LOW);       
    // } 
    if(temp < TEMP_MIN) {
        analogWrite(fanPin, PWM_MIN);       
    } 
    if((temp >= TEMP_MIN) && (temp <= TEMP_MAX)) {  // if temperature is higher than minimum temp
        fanSpeed = map(temp, TEMP_MIN, TEMP_MAX, PWM_MIN, PWM_MAX); // the actual speed of fan
        analogWrite(fanPin, fanSpeed);  // spin the fan at the fanSpeed speed
    } 

    if(temp > TEMP_MAX) {        // if temp is higher than tempMax
        fanSpeed = PWM_MAX;
    }
    // if(temp > TEMP_MAX) {        // if temp is higher than tempMax
        // digitalWrite(STATUS_LED, HIGH);  // turn on led 
    // } else {                    // else turn of led
        // digitalWrite(STATUS_LED, LOW); 
    // }
    int fanPrecent = map(fanSpeed, PWM_MIN, PWM_MAX, 0, 100);
    
    Serial.print(temp);      // display the temperature
    Serial.print(F("C; "));
    
    Serial.print(fanPrecent);    // display the fan percent
    Serial.print(F("%; "));
    
    Serial.print(fanSpeed);    // display the fan pwm
    Serial.print(F("pwm; "));
    
    Serial.print(FANRPM_1);    // display the fan rpm
    Serial.print(F("RPM; "));

    Serial.print(millis()/1000);    // display the second job after start
    Serial.println(F("Sec"));
}
