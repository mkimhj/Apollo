//Local Libraries
#include <RFduinoBLE.h>
#include "Pins.h"
#include "ML8511.h"
#include "Neopixel.h"

//Arduino Libraries
#include <Time.h>

//Flash memory class
//struct data_t
//{
//  // we will use java's famous 0xCAFEBABE magic number to indicate
//  // that the flash page has been initialized previously by our
//  // sketch
//  int magic_number;
//  int hour;
//  int minute;
//};

static const char* NAME = "Violet Watch";
static const int ADVERTISEMENT_INTERVAL = 675 * 5;
static float UV_THRESHOLD = .5;
static const int UV_GOAL = 24;
static bool connectionStatus = false;
int goldenHour;
int goldenMinute;
int uvMinutes = 0;
boolean goalMet = false;
boolean sunsetFlag = false;
boolean DEMO = true;

void advertise(const char *data, uint32_t ms) {
  RFduinoBLE.advertisementData = data;

  //start advertising
  RFduinoBLE.begin();

  //advertise for ms milliseconds
  RFduino_ULPDelay(ms);

  //  //stop advertising
  //  RFduinoBLE.end();
}

//This function will automatically run everytime the device is connected
void RFduinoBLE_onConnect() {
  //Code here
  Serial.println("connected");
  connectionStatus = true;
}


//This function will automatically run everytime the device is disconnected
void RFduinoBLE_onDisconnect() {
  //Code here
  Serial.println("disconnected");
  connectionStatus = false;
}

int tapCallback(uint32_t ulPin) {
  setHands();
  return 0;
}

void turnOffLights() {
  for (int i = 0; i < 12; i++) {
    set_pixel_color(i, Color(0, 0, 0));
  }
  show();
}

void setHands() {
  int h = hourFormat12() % 12;
  int m = floor(minute()/5);
  unsigned long millisecs = millis();
  if (h == m) {
    set_pixel_color(m, Color(32, 0, 32));
  } else {
    set_pixel_color(h, Color(64, 0, 0));
    set_pixel_color(m, Color(0,  0, 64));
  }
  show();
  delay(50);
  while ((millis() - millisecs) < 3000) {
    if (digitalRead(PIEZO) == LOW) {
      displayUV();
      break;
    }
  }
  turnOffLights();
}

void displayUV() {
  int uvHands = floor(uvMinutes/2);
  for (int i = 0; i <= uvHands; i++) {
    set_pixel_color(i, Color(32, 32, 32));
    delay(75);
    show();
  }
  
  for (int i = 0; i < 2; i++) {
    set_pixel_color(uvHands, Color(0,0,0));
    delay(200);
    show();
    set_pixel_color(uvHands, Color(32,32,32));
    delay(200);
    show();
  }
  
  delay(500);
}

void checkUV() {
  digitalWrite(ENABLE_PIN, HIGH);
  RFduino_ULPDelay(1);
  float UVIntensity = 0;
  for (int i = 0; i < 4; i++) {
    UVIntensity += .25*getUVIntensity();
  }
  
  if (hour() < 4 && hour() > 10) {
    UV_THRESHOLD = .9*UV_THRESHOLD + .1*UVIntensity;
  }
  
//  Serial.println(UVIntensity);
  if (UVIntensity > UV_THRESHOLD*2) {
    uvMinutes++;
  }
  digitalWrite(ENABLE_PIN, LOW);
}

void checkSunset() {
  if (hourFormat12() == goldenHour && minute() >= goldenMinute && hour() > 12 && sunsetFlag == false) {
    sunsetFlag = true;
    for (int i = 0; i < 3; i++) {
      digitalWrite(MOTOR, HIGH);
      RFduino_ULPDelay(500);
      digitalWrite(MOTOR, LOW);
      RFduino_ULPDelay(150);
    }
    
    RFduino_ULPDelay(500);
    for (int i = 0; i < 3; i++) {
      digitalWrite(MOTOR, HIGH);
      RFduino_ULPDelay(500);
      digitalWrite(MOTOR, LOW);
      RFduino_ULPDelay(150);
    }
  }
}

void checkUVGoal() {
  if (uvMinutes >= UV_GOAL && goalMet == false) {
    goalMet = true;
    digitalWrite(MOTOR, HIGH);
    RFduino_ULPDelay(1000);
    digitalWrite(MOTOR, LOW);
    RFduino_ULPDelay(100);
    digitalWrite(MOTOR, HIGH);
    RFduino_ULPDelay(100);
    digitalWrite(MOTOR,LOW);
  }
}

void RFduinoBLE_onReceive(char *data, int len) {
  Serial.println(data);
  
  int hours = (data[0] - '0') * 10 + (data[1] - '0');
  int minutes =  (data[2] - '0') * 10 + (data[3] - '0');
  goldenHour =  (data[4] - '0') * 10 + (data[5] - '0');
  goldenMinute =  (data[6] - '0') * 10 + (data[7] - '0');
  setTime(hours, minutes, second(), 0, 0, 0);
  
  setHands();
  
  RFduinoBLE.sendInt(uvMinutes);
}

void setup() {
  // put your setup code here, to run once:

  //Set Device Parameters
  RFduinoBLE.deviceName = NAME;
  RFduinoBLE.advertisementInterval = ADVERTISEMENT_INTERVAL;

  //Setup Motor
  pinMode(MOTOR, OUTPUT);
  digitalWrite(MOTOR, LOW);

  //Setup Piezo
  pinMode(PIEZO, INPUT);
  RFduino_pinWakeCallback(PIEZO, LOW, tapCallback);

  //Setup UV Sensor
  analogReference(DEFAULT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(UV, INPUT);
  digitalWrite(ENABLE_PIN, LOW);

  //start advertising and serial connection
  begin();
  RFduinoBLE.begin();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //reset daily uv tracking
  if (DEMO) {
    //Check UV Data and if it's sunset every minute
    checkUV();
    checkSunset();
    checkUVGoal();
    RFduino_ULPDelay(SECONDS(1));    
  } else {
    if (hour() == 0 && minute() == 0) {
      uvMinutes = 0;
      goalMet = false;
      sunsetFlag = false;
    }
    //Check UV Data and if it's sunset every minute
    checkUV();
    checkSunset();
    checkUVGoal();
    RFduino_ULPDelay(MINUTES(1));
  }
}
