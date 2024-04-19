#include <Wire.h>
#define I2C_SDA 8
#define I2C_SCL 9
#define JOYSTICK_VRX 0
#define JOYSTICK_VRY 1
#define JOYSTICK_SW 3
#define BUTTON_1 21
#define BUTTON_2 20
#define ENCODER_GAIN_FACTOR 10
#define ENCODER_MIN_VALUE 0
#define ENCODER_MAX_VALUE 1023
#define BT_KEYBOARD_DEFAULT_DELAY 8
#define BT_KEYBOARD_DELAY 100
#define SETUP_DELAY 1000
#define LOOP_DELAY 100
#define JOYSTICK_X_MIN_LIMIT_NEUTRAL 3600
#define JOYSTICK_X_MAX_LIMIT_NEUTRAL 4000
#define X_INVERTED -1
#define JOYSTICK_Y_MIN_LIMIT_NEUTRAL 3600
#define JOYSTICK_Y_MAX_LIMIT_NEUTRAL 4000
#define Y_INVERTED 1
#define BT_KBD_NAME "Super Duper KBD"
#define BT_KBD_MANUFACTURER "Gienek Labs"
#define ENCODER_I2C_ADDR 0x54
#define COMM_SPD 115200
#define DEBUG_STATUSES 1
#define DEBUG_EVENTS 0

// KEY_ESC
//#define BUTTON_1_CODE 0xB1
// KEY_BACKSPACE
#define BUTTON_1_CODE 0xB2
// KEY_MENU
#define BUTTON_2_CODE 0xED
// KEY_RETURN
#define JOYSTICK_SW_CODE 0xB0
// ARROW UP
#define JOYSTICK_UP 0xDA
// ARROW DOWN
#define JOYSTICK_DOWN 0xD9
// ARROW LEFT
#define JOYSTICK_LEFT 0xD8
// ARROW RIGHT
#define JOYSTICK_RIGHT 0xD7
// VOLUME UP
#define ENCODER_VOLUME_UP KEY_MEDIA_VOLUME_UP
// VOLUME DOWN
#define ENCODER_VOLUME_DOWN KEY_MEDIA_VOLUME_DOWN

/**
 * Instantiate an object to drive our sensor;
 * Set address according to encoder DIP switch CH1 and CH2:
 * | 1 | 2 | ADDR |
 * |---|---|------|
 * | 0 | 0 | 0x54 |
 * | 0 | 1 | 0x55 |
 * | 1 | 0 | 0x56 |
 * | 1 | 1 | 0x57 |
 */

#include <DFRobot_VisualRotaryEncoder.h>

#include <BleKeyboard.h>

DFRobot_VisualRotaryEncoder_I2C sensor(/*i2cAddr = */ENCODER_I2C_ADDR, /*i2cBus = */&Wire);

BleKeyboard bleKeyboard(BT_KBD_NAME, BT_KBD_MANUFACTURER, 100);
// The third parameter is the initial battery level of your device. To adjust the battery level later on you can simply call e.g. bleKeyboard.setBatteryLevel(50) (set battery level to 50%).

int lastVolumeLevel = 0;

void setup() {
  Serial.begin(COMM_SPD);
  bool var = Wire.begin(I2C_SDA, I2C_SCL);
  if(var) {
    Serial.println("I2C connected");
    // initialize sensor
    while( NO_ERR != sensor.begin() ){
      Serial.println("Communication with device SEN0502 failed, please check connection. Pause for 3 sec.");
      delay(3000);
    }
    Serial.println("SEN0502 Sensor Begin ok!");

    /**
    * Retrieve basic information from the sensor and buffer it into basicInfo, the structure that stores information
    * Members of basicInfo structure: PID, VID, version, i2cAddr
    */
    sensor.refreshBasicInfo();

    /* Module PID, default value 0x01F6 (the highest two of the 16-bits data are used to judge SKU type: 00: SEN, 01: DFR, 10: TEL; The next 14 numbers are used as num)(SEN0502) */
    Serial.print("PID: 0x0");
    Serial.println(sensor.basicInfo.PID, HEX);

    /* Module VID, default value 0x3343(for manufacturer DFRobot) */
    Serial.print("VID: 0x");
    Serial.println(sensor.basicInfo.VID, HEX);

    /* Firmware version number: 0x0100 represents V0.1.0.0 */
    Serial.print("versions: 0x0");
    Serial.println(sensor.basicInfo.version, HEX);

    /* Module communication address, default value 0x54, module device address (0x54~0x57) */
    Serial.print("communication address:  0x");
    Serial.println(sensor.basicInfo.i2cAddr, HEX);

    sensor.setGainCoefficient(ENCODER_GAIN_FACTOR);
    /**
    * Get the encoder current gain factor, and the numerical accuracy for turning one step
    * Accuracy range:1~51, the minimum is 1 (light up one LED about every 2.5 turns), the maximum is 51 (light up one LED every one step rotation)
    * Return value range: 1-51
    */
    uint8_t gainCoefficient = sensor.getGainCoefficient();
    Serial.print("Encoder current gain coefficient: ");
    Serial.println(gainCoefficient);

    Serial.println();

    uint16_t encoderValue = sensor.getEncoderValue();
    int volumeLevel = map(encoderValue, ENCODER_MIN_VALUE, ENCODER_MAX_VALUE, 0, 100);
    lastVolumeLevel = volumeLevel;

    // Now init joystick

    //pinMode(JOYSTICK_SW, INPUT);
    //digitalWrite(JOYSTICK_SW, HIGH) ;
    pinMode(JOYSTICK_SW, INPUT_PULLUP); 

    // Now init buttons

    pinMode(BUTTON_1, INPUT_PULLUP); 
    pinMode(BUTTON_2, INPUT_PULLUP); 

    // Now init bluetooth

    Serial.println("Starting BLE work!");
    bleKeyboard.setDelay(BT_KEYBOARD_DEFAULT_DELAY);
    bleKeyboard.begin();
    delay(SETUP_DELAY);
  }
  else  {
    Serial.println("I2C not connected");
    vTaskDelete(NULL);
  }
}

void loop() {
  /**
   * Detect if the button is pressed
   * return true when the button pressed，otherwise, return false
   */
  if(sensor.detectButtonDown()){
    /**
     * Set the encoder count value
     * value range[0, 1023], the setting is invalid when out of range
     * In this example, set the encoder value to zero when detecting the button pressed, and you can see all the LEDs that light up before turning off
     */
    sensor.setEncoderValue(0);
  }
  /**
   * Get the encoder current count
   * Return value range： 0-1023
   */
  uint16_t encoderValue = sensor.getEncoderValue();
  int volumeLevel = map(encoderValue, ENCODER_MIN_VALUE, ENCODER_MAX_VALUE, 0, 100);
  int jx = analogRead(JOYSTICK_VRX);
  int joystickX = (jx >= JOYSTICK_X_MAX_LIMIT_NEUTRAL) ? X_INVERTED : ((jx <= JOYSTICK_X_MIN_LIMIT_NEUTRAL) ? -1*X_INVERTED : 0 );
  int jy = analogRead(JOYSTICK_VRY);
  int joystickY = (jy >= JOYSTICK_Y_MAX_LIMIT_NEUTRAL) ? Y_INVERTED : ((jy <= JOYSTICK_Y_MIN_LIMIT_NEUTRAL) ? -1*Y_INVERTED : 0 );
  int jsw = 1-digitalRead(JOYSTICK_SW);
  int button1State = 1-digitalRead(BUTTON_1);
  int button2State = 1-digitalRead(BUTTON_2);

  if(DEBUG_STATUSES) {
    Serial.print("Volume level: ");
    Serial.print(volumeLevel);
    Serial.println("%");

    Serial.print("Joystick X:");
    Serial.print(joystickX, DEC);
    Serial.print(" / ");
    Serial.print(jx, DEC);

    Serial.print(" | Y:");
    Serial.print(joystickY, DEC);
    Serial.print(" / ");
    Serial.print(jy, DEC);
    
    Serial.print(" |  Button: ");
    Serial.println(jsw, DEC);

    Serial.print("Button1: ");
    Serial.print(button1State, DEC);
    Serial.print(" | Button2: ");
    Serial.println(button2State, DEC);
    
    Serial.println();
  }
  if(bleKeyboard.isConnected()) {
    if(DEBUG_STATUSES) Serial.println("BT Keyboard is connected!");
    if(joystickX < 0) { bleKeyboard.write(JOYSTICK_LEFT);delay(BT_KEYBOARD_DELAY);  if(DEBUG_EVENTS) Serial.println("JOYSTICK LEFT!");}
    if(joystickX > 0) { bleKeyboard.write(JOYSTICK_RIGHT);delay(BT_KEYBOARD_DELAY);  if(DEBUG_EVENTS) Serial.println("JOYSTICK RIGHT!");}
    if(joystickY > 0) { bleKeyboard.write(JOYSTICK_UP);delay(BT_KEYBOARD_DELAY);  if(DEBUG_EVENTS) Serial.println("JOYSTICK UP!");}
    if(joystickY < 0) { bleKeyboard.write(JOYSTICK_DOWN);delay(BT_KEYBOARD_DELAY);  if(DEBUG_EVENTS) Serial.println("JOYSTICK DOWN!");}
    if((jsw != 0) && (joystickX == 0) && (joystickY == 0)) { bleKeyboard.write(JOYSTICK_SW_CODE);delay(BT_KEYBOARD_DELAY); if(DEBUG_EVENTS) Serial.println("JOYSTICK CLICK!");}
    if(button1State != 0) { bleKeyboard.write(BUTTON_1_CODE);delay(BT_KEYBOARD_DELAY);  if(DEBUG_EVENTS) Serial.println("BUTTON1 CLICK!");}
    if(button2State != 0) { bleKeyboard.write(BUTTON_2_CODE);delay(BT_KEYBOARD_DELAY);  if(DEBUG_EVENTS) Serial.println("BUTTON2 CLICK!");}
    if(lastVolumeLevel != volumeLevel) {
      //BT_KEYBOARD_DELAY
      for(int i=0;i<abs(volumeLevel-lastVolumeLevel);i++) {
        bleKeyboard.write((lastVolumeLevel < volumeLevel) ? ENCODER_VOLUME_UP : ENCODER_VOLUME_DOWN);
        if(DEBUG_EVENTS) {
          Serial.print("VOLUME CHANGE:");
          Serial.println((lastVolumeLevel < volumeLevel) ? "UP!" : "DOWN!");
        }
        delay(BT_KEYBOARD_DEFAULT_DELAY);
      }
      lastVolumeLevel = volumeLevel;
    }
  }
  delay(LOOP_DELAY);
}
