#include <ESP8266WiFi.h>

const char *ssid     = "WIFI";
const char *password = "PASS";

//int ledPin = 5; //NodeMCU
int ledPin = 1; //ESP01
//int alarmPin = 4; //NodeMCU
int alarmPin = 3; //ESP01

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 //32 o 64 OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
const unsigned char PROGMEM bolt_icon [] = {
0x00, 0x07, 0xE0, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x03, 0xFF, 0xFF, 0xC0,
0x07, 0xFF, 0xFF, 0xE0, 0x0F, 0xFF, 0xFF, 0xF0, 0x1F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFC, 0x00, 0xF8,
0x3F, 0xF8, 0x00, 0xFC, 0x3F, 0xF8, 0x01, 0xFC, 0x7F, 0xF0, 0x03, 0xFE, 0x7F, 0xF0, 0x03, 0xFE,
0x7F, 0xE0, 0x07, 0xFE, 0xFF, 0xE0, 0x0F, 0xFF, 0xFF, 0xC0, 0x0F, 0xFF, 0xFF, 0xC0, 0x03, 0xFF,
0xFF, 0xC0, 0x03, 0xFF, 0xFF, 0xF0, 0x07, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0x7F, 0xF0, 0x1F, 0xFE,
0x7F, 0xE0, 0x7F, 0xFE, 0x7F, 0xE0, 0xFF, 0xFE, 0x3F, 0xC1, 0xFF, 0xFC, 0x3F, 0xC3, 0xFF, 0xFC,
0x1F, 0x87, 0xFF, 0xF8, 0x1F, 0x9F, 0xFF, 0xF8, 0x0F, 0xBF, 0xFF, 0xF0, 0x07, 0xFF, 0xFF, 0xE0,
0x03, 0xFF, 0xFF, 0xC0, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x07, 0xE0, 0x00
};

const unsigned char PROGMEM Clock_icon [] = {
0x0F, 0x80, 0x01, 0xF0, 0x38, 0xE0, 0x07, 0x1C, 0x60, 0x30, 0x0C, 0x06, 0x40, 0x18, 0x18, 0x02,
0xC0, 0x70, 0x0E, 0x03, 0x81, 0xC0, 0x03, 0x81, 0x83, 0x0F, 0xE0, 0xC1, 0x87, 0x3C, 0x3C, 0xE1,
0xC9, 0xE0, 0x07, 0xB3, 0x59, 0x80, 0x01, 0x9A, 0x73, 0x00, 0x00, 0xCE, 0x22, 0x00, 0x02, 0x44,
0x04, 0x00, 0x06, 0x60, 0x0C, 0x00, 0x0C, 0x30, 0x08, 0x18, 0x18, 0x30, 0x08, 0x0C, 0x30, 0x10,
0x18, 0x06, 0x60, 0x10, 0x18, 0x03, 0xC0, 0x18, 0x18, 0x01, 0x80, 0x18, 0x18, 0x00, 0x00, 0x18,
0x18, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x10, 0x0C, 0x00, 0x00, 0x30,
0x04, 0x00, 0x00, 0x20, 0x06, 0x00, 0x00, 0x60, 0x03, 0x00, 0x00, 0xC0, 0x07, 0x80, 0x01, 0xE0,
0x04, 0xC0, 0x03, 0x20, 0x08, 0x78, 0x1C, 0x30, 0x18, 0x1F, 0xF0, 0x18, 0x10, 0x00, 0x00, 0x08
};

#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 30 
//#define PIN 5 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, ledPin, NEO_GRB + NEO_KHZ800);
byte selectedEffect;

#include <BlynkSimpleEsp8266.h>
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "owfgQ_NwA-QEk3SBVofNma9wXPsX1YTj";

#include <TimeLib.h> //Time Alarms
#include <TimeAlarms.h> //Time Alarms
#include <NTPClient.h> //NTP
#include <WiFiUdp.h> //NTP
int alarmHour, alarmHourSet, alarmMin, alarmMinSet;


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

#include "Adafruit_MQTT.h" //Google Assistant
#include "Adafruit_MQTT_Client.h" //Google Assistant

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  PORT                  
#define AIO_USERNAME    "username"
#define AIO_KEY         "key"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);       
Adafruit_MQTT_Subscribe LED_Control = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Luces");
Adafruit_MQTT_Subscribe ALARM_Control = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Alarma");

void MQTT_connect();
void NTP_setTime();
void NTP_printTime();
void WIFI_connect();
void LED_turnOn(byte selectedEffect);
void LED_turnOff();
void ALARMA_turnOn();

bool ledState;
bool alarmState;
int r;
int g;
int b;

void setup() {

  pinMode(ledPin, OUTPUT);

  Wire.begin(0,2); // set I2C pins (SDA = GPIO2, SCL = GPIO0), default clock is 100kHz
  
  pinMode(alarmPin,OUTPUT);
  digitalWrite(alarmPin, HIGH);
  delay(50);
  digitalWrite(alarmPin, LOW);
  delay(50);
  digitalWrite(alarmPin, HIGH);
  delay(50);
  digitalWrite(alarmPin, LOW);
  delay(50);
  digitalWrite(alarmPin, HIGH);
  delay(50);
  digitalWrite(alarmPin, LOW);
  
  DISPLAY_init();

  selectedEffect = 7;
  ledState = true;
  alarmState = false;
  r = 255;
  g = 255;
  b = 255;
  alarmHour = 0;
  alarmMin = 0;
    
//  Serial.println("Program started");
//  Serial.print("Effect selected:");
//  Serial.println(selectedEffect);

  strip.begin();
  strip.show();
  
  WIFI_connect();

  NTP_setTime();

  delay(3000);

  DISPLAY_ALH();

  /*CylonBounce(r, g, b, 1, 20, 10);
  CylonBounce(r, g, b, 1, 20, 10);
  colorWipe(r,g,b, 20);
  colorWipe(0x00,0x00,0x00, 20);
  colorWipe(r,g,b, 20);
  colorWipe(0x00,0x00,0x00, 20);
  Strobe(r, g, b, 5, 200, 0);
  Strobe(r, g, b, 5, 100, 0);
  Strobe(r, g, b, 30, 12, 0);*/
  rainbowCycle(5);
  
  mqtt.subscribe(&LED_Control);
  mqtt.subscribe(&ALARM_Control);

  Blynk.begin(auth, ssid, password);

}
  
void loop() {
  NTP_printTime();
  Blynk.run();
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1))) {
    if (subscription == &LED_Control) {
//      Serial.print(F("Got: "));
//      Serial.println((char *)LED_Control.lastread);
       if (!strcmp((char*) LED_Control.lastread, "1")){
        ledState = true;
        delay(1000);
//        Serial.println("LUCES ON");
      }
      else if (!strcmp((char*) LED_Control.lastread, "0")){
        ledState = false;
//        Serial.println("LUCES OFF");
      }

    }
    else if (subscription == &ALARM_Control){
//      Serial.print(F("Got: "));
//      Serial.println((char *)ALARM_Control.lastread);
       if (!strcmp((char*) ALARM_Control.lastread, "1")){
        delay(1000);
//        Serial.println("ALARMA ON");
      }
      else if (!strcmp((char*) ALARM_Control.lastread, "0")){
//        Serial.println("ALARMA OFF");
        alarmState = false;
        ledState = true;
        selectedEffect = 7;
      }
    }
  }

  if((ledState == true) && (alarmState == true)){
    ALARMA_turnOn();
    LED_turnOn(2);
  }
  else if((ledState == false) && (alarmState == true)){
    ALARMA_turnOn();
    LED_turnOn(2);
  }
  else if((ledState == true) && (alarmState == false)){
    LED_turnOn(selectedEffect);
  }
  else if((ledState == false) && (alarmState == false)){
    LED_turnOff();
  }  
  Alarm.delay(0);
}

void MQTT_connect() {
  int8_t ret, retries = 3;
  if (mqtt.connected()) {
    return;
  }
//  Serial.print("Connecting to MQTT... ");
  while ((ret = mqtt.connect()) != 0) {
//       Serial.println(mqtt.connectErrorString(ret));
//       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000); 
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Alarm.alarmOnce(alarmHour,alarmMin,1, MorningAlarm);
//  Serial.println("MQTT Connected!");
}

void NTP_setTime(){
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  timeClient.setTimeOffset(3600);
  timeClient.update();

    //Get a time structure
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  
  int currentHour = timeClient.getHours();
//  Serial.print("Hour: ");
//  Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
//  Serial.print("Minutes: ");
//  Serial.println(currentMinute); 

  int monthDay = ptm->tm_mday;
//  Serial.print("Month day: ");
//  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
//  Serial.print("Month: ");
//  Serial.println(currentMonth);
  
  int currentYear = ptm->tm_year+1900;
//  Serial.print("Year: ");
//  Serial.println(currentYear);

  DISPLAY_displayText(String(currentHour)+":"+String(currentMinute)+" "+String(currentYear)+"/"+String(currentMonth)+"/"+String(monthDay));
  
  if(currentYear == 1970){
    Strobe(255, 0, 0, 5, 200, 0);
//    Serial.println("Reset..");
    ESP.restart();
  }

  setTime(currentHour,currentMinute,0,monthDay,currentMonth,currentYear);
}
void DISPLAY_init(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
//    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
}
void DISPLAY_ALH(){

  display.clearDisplay();
  display.setTextSize(3,3);
  display.setCursor(15,5);
  display.setTextColor(WHITE);
  display.println("A");
  display.display();
  delay(1000);
  
  display.setCursor(15,5);
  display.println("AL");
  display.display();  
  delay(1000);
  
  display.setCursor(15,5);
  display.println("ALH");
  display.display();
  delay(1000);
  
  display.drawBitmap(80, 0, bolt_icon, 32, 32, 1);
  display.display();

  display.setTextSize(1);
  display.setCursor(0,0);
}
void DISPLAY_displayAlarmIcon(){
  display.clearDisplay();
  display.drawBitmap(45, 0, Clock_icon, 32, 32, 1);
  display.display();
}
void DISPLAY_displayText(String val){
  display.println(val);
  display.display();
}

void DISPLAY_displayTime(int h, int m){
  display.clearDisplay();
  display.setTextSize(3,3);
  display.setCursor(15,10);
  if((m < 10)&&(h < 10)){
    display.println("0"+String(h)+":"+"0"+String(m)); 
  }
  else if(h < 10){
    display.println("0"+String(h)+":"+String(m)); 
  }
  else if(m < 10){
    display.println(String(h)+":"+"0"+String(m)); 
  }
  else{
    display.println(String(h)+":"+String(m));
  }
  display.display();
}
void DISPLAY_displayAlarm(String h, String m){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("Alarm set:");
  display.println(h+":"+m);
  display.display();
  delay(3000);
}

void WIFI_connect(){
  int i = 0;
//  Serial.begin(115200);
//  Serial.print("Connecting to ");
//  Serial.println(ssid);

  /*WIFI Connection"*/
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
//    Serial.print(".");
    DISPLAY_displayText(".");
    display.setCursor(i,0);
    i = i + 5;
  }
   DISPLAY_displayText("");
//  Serial.println();
//  Serial.println("WiFi connected");
  DISPLAY_displayText("WiFi connected");
//  Serial.println("IP address: ");

  IPAddress myIP = WiFi.localIP();
  String ipStr = String(myIP[0])+"."+String(myIP[1])+"."+String(myIP[2])+"."+String(myIP[3]);
//  Serial.println(WiFi.localIP());
  DISPLAY_displayText(ipStr);
}

BLYNK_WRITE(V0){
//  Serial.println("Horas establecidas: ");
  int pinValue = param.asInt();
//  Serial.println(pinValue);
  alarmHour = pinValue;
  //Alarm.alarmOnce(alarmHour,alarmMin,1, MorningAlarm);
}
BLYNK_WRITE(V1){
//  Serial.println("Minutos establecidos: ");
  int pinValue = param.asInt();
//  Serial.println(pinValue);
  alarmMin = pinValue;
  //Alarm.alarmOnce(alarmHour,alarmMin,1, MorningAlarm);
}
BLYNK_WRITE(V8){
//  Serial.println("Alarma establecida");
  alarmHourSet = alarmHour;
  alarmMinSet = alarmMin;
  DISPLAY_displayAlarm(String(alarmHour), String(alarmMin));
  Alarm.alarmOnce(alarmHour,alarmMin,1, MorningAlarm);
}

BLYNK_WRITE(V5){
  int pinValue = param.asInt();
  selectedEffect = pinValue;
//  Serial.println("Efecto actualizado.");
}
BLYNK_WRITE(V6){
  
  int pinValue = param.asInt();
  if(pinValue == true){
//    Serial.println("Encendiendo luces.");
    ledState = true;
  }
  else{
//    Serial.println("Apagando luces.");
    ledState = false;
  }
}
void MorningAlarm(){
//  Serial.println("Ha saltado la alarma");
  alarmState = true;
}
BLYNK_WRITE(V2) // V5 is the number of Virtual Pin  
{
//  Serial.println("Verificando alarma en app");
  Blynk.virtualWrite(V3, alarmHourSet);
  Blynk.virtualWrite(V4, alarmMinSet);
}

void NTP_printTime(){
  
  time_t t = now();
  DISPLAY_displayTime(hour(t), minute(t));
  
//  Serial.print(day(t));
//  Serial.print(+ "/") ;
//  Serial.print(month(t));
//  Serial.print(+ "/") ;
//  Serial.print(year(t)); 
//  Serial.print( " ") ;
//  Serial.print(hour(t));  
//  Serial.print(+ ":") ;
//  Serial.print(minute(t));
//  Serial.print(":") ;
//  Serial.println(second(t));
}

// *************************
// ** LEDEffect Functions **
// *************************

void RGBLoop(){
  for(int j = 0; j < 3; j++ ) { 
    // Fade IN
    for(int k = 0; k < 256; k++) { 
      switch(j) { 
        case 0: setAll(2,26,16); break;
        case 1: setAll(26,2,0); break;
        case 2: setAll(0,0,k); break;
      }
      showStrip();
     // delay(0);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) { 
      switch(j) { 
        case 0: setAll(2,26,16); break;
        case 1: setAll(26,2,0); break;
        case 2: setAll(0,0,k); break;
      }
      showStrip();
      //delay(0);
    }
  }
}
void RGBLoopEdited(){
  for(int j = 0; j < 3; j++ ) { 
    // Fade IN
    for(int k = 0; k < 256; k++) { 
      switch(j) { 
        case 0: setAll(2,26,16); break;
        case 1: setAll(26,2,0); break;
        case 2: setAll(24,2,6); break;
      }
      showStrip();
     // delay(0);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) { 
      switch(j) { 
        case 0: setAll(2,26,16); break;
        case 1: setAll(26,2,0); break;
        case 2: setAll(0,0,k); break;
      }
      showStrip();
      //delay(0);
    }
  }
}

void FadeInOut(byte red, byte green, byte blue){
  float r, g, b;
      
  for(int k = 0; k < 256; k=k+1) { 
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
  delay(1000);
     
  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
  delay(200);
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++) {
    setAll(red,green,blue);
    showStrip();
    delay(FlashDelay);
    setAll(0,0,0);
    showStrip();
    delay(FlashDelay);
  }
 
 delay(EndPause);
}

void HalloweenEyes(byte red, byte green, byte blue, 
                   int EyeWidth, int EyeSpace, 
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause){
  randomSeed(analogRead(0));
  
  int i;
  int StartPoint  = random( 0, NUM_LEDS - (2*EyeWidth) - EyeSpace );
  int Start2ndEye = StartPoint + EyeWidth + EyeSpace;
  
  for(i = 0; i < EyeWidth; i++) {
    setPixel(StartPoint + i, red, green, blue);
    setPixel(Start2ndEye + i, red, green, blue);
  }
  
  showStrip();
  
  if(Fade==true) {
    float r, g, b;
  
    for(int j = Steps; j >= 0; j--) {
      r = j*(red/Steps);
      g = j*(green/Steps);
      b = j*(blue/Steps);
      
      for(i = 0; i < EyeWidth; i++) {
        setPixel(StartPoint + i, r, g, b);
        setPixel(Start2ndEye + i, r, g, b);
      }
      
      showStrip();
      delay(FadeDelay);
    }
  }
  
  setAll(0,0,0); // Set all black
  
  delay(EndPause);
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){

  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  
  delay(ReturnDelay);
}

void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

// used by NewKITT
void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i =((NUM_LEDS-EyeSize)/2); i>=0; i--) {
    setAll(0,0,0);
    
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue); 
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);
    
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// used by NewKITT
void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i<=((NUM_LEDS-EyeSize)/2); i++) {
    setAll(0,0,0);
    
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue); 
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);
    
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// used by NewKITT
void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

// used by NewKITT
void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),red,green,blue);
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) { 
       setAll(0,0,0); 
     }
   }
  
  delay(SpeedDelay);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) { 
       setAll(0,0,0); 
     }
   }
  
  delay(SpeedDelay);
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(red,green,blue);
  
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,0xff,0xff,0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position=0;
  
  for(int i=0; i<NUM_LEDS*2; i++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<NUM_LEDS; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
      }
      
      showStrip();
      delay(WaveDelay);
  }
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
      setPixel(i, red, green, blue);
      showStrip();
      delay(SpeedDelay);
  }
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

// used by rainbowCycle and theaterChaseRainbow
byte * Wheel(byte WheelPos) {
  static byte c[3];
  
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUM_LEDS; i=i+3) {
        setPixel(i+q, red, green, blue);    //turn every third pixel on
      }
      showStrip();
     
      delay(SpeedDelay);
     
      for (int i=0; i < NUM_LEDS; i=i+3) {
        setPixel(i+q, 0,0,0);        //turn every third pixel off
      }
    }
  }
}

void theaterChaseRainbow(int SpeedDelay) {
  byte *c;
  
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < NUM_LEDS; i=i+3) {
          c = Wheel( (i+j) % 255);
          setPixel(i+q, *c, *(c+1), *(c+2));    //turn every third pixel on
        }
        showStrip();
       
        delay(SpeedDelay);
       
        for (int i=0; i < NUM_LEDS; i=i+3) {
          setPixel(i+q, 0,0,0);        //turn every third pixel off
        }
    }
  }
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

//used by Fire
void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void BouncingColoredBalls(int BallCount, byte colors[][3], boolean continuous) {
  float Gravity = -9.81;
  int StartHeight = 1;
  
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
  boolean ballBouncing[BallCount];
  boolean ballsStillBouncing = true;
  
  for (int i = 0 ; i < BallCount ; i++) {   
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0; 
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
    ballBouncing[i]=true; 
  }

  while (ballsStillBouncing) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
  
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
  
        if ( ImpactVelocity[i] < 0.01 ) {
          if (continuous) {
            ImpactVelocity[i] = ImpactVelocityStart;
          } else {
            ballBouncing[i]=false;
          }
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }

    ballsStillBouncing = false; // assume no balls bouncing
    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i],colors[i][0],colors[i][1],colors[i][2]);
      if ( ballBouncing[i] ) {
        ballsStillBouncing = true;
      }
    }
    
    showStrip();
    setAll(0,0,0);
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
  
  for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) {
    
    
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
    
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        setPixel(i-j, red, green, blue);
      } 
    }
   
    showStrip();
    delay(SpeedDelay);
  }
}

// used by meteorrain
void fadeToBlack(int ledNo, byte fadeValue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
    
    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
    
    strip.setPixelColor(ledNo, r,g,b);
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[ledNo].fadeToBlackBy( fadeValue );
 #endif  
}

// *** REPLACE TO HERE ***



// ***************************************
// ** FastLed/NeoPixel Common Functions **
// ***************************************

// Apply LED color changes
void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}

void LED_turnOn(byte selectedEffect){
    switch(selectedEffect) {
    
    case 0  : {
                FadeInOut(2,26,16);
                FadeInOut(22,3,0);
                FadeInOut(24,2,6);
                break;
              }

        case 1  : {
                // FadeInOut - Color (red, green. blue)
                FadeInOut(r, g, b); // red
                break;
              }
              
    case 2  : {
                // Strobe - Color (red, green, blue), number of flashes, flash speed, end pause
                Strobe(r, g, b, 5, 200, 0);
                break;
              }

    case 3  : {
                // meteorRain - Color (red, green, blue), meteor size, trail decay, random trail decay (true/false), speed delay 
                meteorRain(r,g,b,10, 64, true, 100);
                break;
              }
              
    case 4  : {
                // CylonBounce - Color (red, green, blue), eye size, speed delay, end pause
                CylonBounce(r, g, b, 1, 100, 50);
                break;
              }
                         
    case 5 : {
                // Running Lights - Color (red, green, blue), wave dealy
                RunningLights(r,g,b, 200);  // red
                break;
              }
              
    case 6 : {
                // colorWipe - Color (red, green, blue), speed delay
                colorWipe(r,g,b, 100);
                colorWipe(0x00,0x00,0x00, 100);
                break;
              }

    case 7 : {
                // rainbowCycle - speed delay
                rainbowCycle(5);
                break;
              }

    case 8 : {
                setAll(r,g,b);
                break;
              }
  }
}
void LED_turnOff(){
  setAll(0,0,0);
}
BLYNK_WRITE(V7){
  r = param[0].asInt();
  g = param[1].asInt();
  b = param[2].asInt();
}
void ALARMA_turnOn(){
  DISPLAY_displayAlarmIcon();
  for(int i = 0; i < 4; i++){
//    Serial.println("Alarmaaaaaa!");
  digitalWrite(alarmPin, HIGH);
  delay(50);
  digitalWrite(alarmPin, LOW);
  delay(50);
  digitalWrite(alarmPin, HIGH);
  delay(50);
  digitalWrite(alarmPin, LOW);
  delay(50);
  digitalWrite(alarmPin, HIGH);
  delay(50);
  digitalWrite(alarmPin, LOW);
  delay(1000);
  }
}
