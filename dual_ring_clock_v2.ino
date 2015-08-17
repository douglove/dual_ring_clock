#include <Wire.h>

#include <Adafruit_NeoPixel.h>

#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
#include <avr/power.h>
#endif

// Pin assignments for Arduino Micro
// 2  to clock sda
// 3  to clock scl
// 5  to ring 12 data
// 6  to ring 24 data
// 7  to hour button
// 8  to minute button
// 9  to second button
// 10 to mode button

// define the pins
#define PIN12            5
#define PIN24            6
#define BTN_HOURS_UP     7
#define BTN_MINUTES_DOWN 8
#define BTN_SECONDS      9
#define BTN_MODE         10

// construct the rings
Adafruit_NeoPixel pixels12 = Adafruit_NeoPixel(12, PIN12, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels24 = Adafruit_NeoPixel(24, PIN24, NEO_GRB + NEO_KHZ800);


uint8_t brightness = 32; // Global brightness (0-255) higher value is brighter

// Pixel colors
uint32_t grey24    = pixels24.Color(24, 24, 24);
uint32_t grey32    = pixels24.Color(32, 32, 32);
uint32_t red       = pixels24.Color(255, 0, 0);
uint32_t red2      = pixels24.Color(32, 0, 0);
uint32_t red3      = pixels24.Color(16, 0, 0);
uint32_t green     = pixels24.Color(0, 255, 0);
uint32_t blue      = pixels24.Color(0, 0, 255);
uint32_t blue2     = pixels24.Color(0, 0, 128);
uint32_t white     = pixels24.Color(255, 255, 255);
uint32_t off       = pixels24.Color(0, 0, 0);

boolean modeButton;
boolean hoursUpButton;
boolean minutesDownButton;
boolean secondsButton;

// Modes
int MODE_CLOCK = 0;
int MODE_SETTINGS = 1;
int mode = MODE_CLOCK;
int numberOfModes = 2;

// Settings  
int[] settings = new int[23];
int[] settingsCount = new int[23];
int currentSetting = 1;

int CHASER_NONE    = 0;
int CHASER_HOURS   = 1;
int CHASER_MINUTES = 2;
int CHASER_SECONDS = 3;
int chaserMode = CHASER_NONE;  

int seconds = 30;     //00-59
int minutes = 22;     //00-59
int hours = 9;        //1-12 - 00-23
int dayOfWeek;        //1-7
int dayOfMonth;       //1-31
int month;            //1-12
int year;             //0-99

int ring12ChaserLed = -1;

int DELAY = 50;

//*********************************************************************************
// SETUP
//*********************************************************************************
void setup() {
  Serial.begin(9600);   
  
  for(int i = 0; i < 24; i++){
    settings[i] = 1; 
    settingsCount[i] = 2;
  }

  #ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
    if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
    // Seed random number generator from an unused analog input:
    randomSeed(analogRead(2));
  #else
    randomSeed(analogRead(A0));
  #endif

  //*********************************************************************************
  // CLOCK START
  //*********************************************************************************
  Wire.begin();
  //*********************************************************************************
  // CLOCK END
  //*********************************************************************************


  //*********************************************************************************
  // BUTTON SETUP START
  //*********************************************************************************
  pinMode(BTN_HOURS_UP, INPUT);
  digitalWrite(BTN_HOURS_UP, HIGH);

  pinMode(BTN_MINUTES_DOWN, INPUT);
  digitalWrite(BTN_MINUTES_DOWN, HIGH);

  pinMode(BTN_SECONDS, INPUT);
  digitalWrite(BTN_SECONDS, HIGH);

  pinMode(BTN_MODE, INPUT);
  digitalWrite(BTN_MODE, HIGH);
  //*********************************************************************************
  // BUTTON SETUP END
  //*********************************************************************************


  //*********************************************************************************
  // NEOPIXELS START
  //*********************************************************************************
  pixels12.begin();
  pixels24.begin();

  pixels12.show();
  pixels24.show();

  pixels12.setBrightness(brightness);
  pixels24.setBrightness(brightness);
  //*********************************************************************************
  // NEOPIXELS END
  //*********************************************************************************
}


//*********************************************************************************
// LOOP
//*********************************************************************************
void loop(){
  //*********************************************************************************
  // READ BUTTONS
  //*********************************************************************************
  modeButton        = (digitalRead(BTN_MODE) == LOW);
  hoursUpButton     = (digitalRead(BTN_HOURS_UP) == LOW);
  minutesDownButton = (digitalRead(BTN_MINUTES_DOWN) == LOW);
  secondsButton     = (digitalRead(BTN_SECONDS) == LOW);
  
  getTime();
  getDate();

  if(modeButton){
     mode++;
     
     if(mode >= numberOfModes){
       mode = 0;
     }
  }
  
  clearRing12();
  clearRing24();

  switch(mode){
    case MODE_CLOCK:
      clock();
      break;
      
    case MODE_SETTINGS:
      settings();
      break;
  }
  
  pixels12.show();
  pixels24.show();
  
  delay(DELAY);
}

//*********************************************************************************
// CLOCK
//*********************************************************************************
void clock(){
  
}

//*********************************************************************************
// SETTINGS
//*********************************************************************************
void settings(){
  // set the zero marker
  pixels24.setPixelColor(0, grey32); 
  
  // set the current setting
  pixels24.setPixelColor(currentSetting, white); 

  // set the current setting's value
  int settingsValue = settings[currentSetting];
  pixels12.setPixelColor(settingsValue);
  
  if(hoursUpButton){
     
  }
  else if(minutesDownButton){
    
  }
}




















//*********************************************************************************
// LOOP
//*********************************************************************************
void loopold() {
  //*********************************************************************************
  // MODE BUTTON
  //*********************************************************************************
  if(digitalRead(BTN_MODE) == LOW){  
    mode++;

    if(mode > numberOfModes){
      mode = 1; 
    }

    delay(250);
  }

  //*********************************************************************************
  // HOURS BUTTON
  //*********************************************************************************
  if(digitalRead(BTN_HOURS) == LOW){  
    // increase hours
    hours++;

    if(hours > 11){
      hours = 0;  
    }

    // set time on chrono dot
    setTime();    

    delay(250);
  }

  //*********************************************************************************
  // MINUTES BUTTON
  //*********************************************************************************
  if(digitalRead(BTN_MINUTES) == LOW){  
    // increase minutes
    minutes++;

    if(minutes > 59){
      minutes = 0;  
    }

    // set time on chrono dot
    setTime();    

    delay(250);
  }

  //*********************************************************************************
  // SECONDS BUTTON
  //*********************************************************************************
  if(digitalRead(BTN_SECONDS) == LOW){  
    // increase seconds
    seconds++;

    if(seconds > 59){
      seconds = 0;  
    }

    // set time on chrono dot
    setTime();    

    delay(250);
  }

  // retrieve the time from the chronodot
  getTime();

  // retrieve the date from the chronodot
  getDate();

  switch(mode){
  case 1:
    chaserMode = CHASER_MINUTES;

    clearRing12();
    clearRing24();

    ring12Chaser(white);

    singleDotRing12(hours, green);
    twoDotBlendRing24(minutes, blue);
    twoDotBlendRing24(seconds, red);
    break;

  case 2:
    chaserMode = CHASER_MINUTES;

    clearRing12();
    clearRing24();

    setQuartersRing12(grey32);
    ring12Chaser(white);

    singleDotRing12(hours, green);
    twoDotBlendRing24(minutes, blue);
    twoDotBlendRing24(seconds, red);
    break;

  case 3:
    // Single dot for hours on neopixel 12 ring, minutes and seconds on neopixel 24 ring
    chaserMode = CHASER_NONE;

    clearRing12();
    clearRing24();

    singleDotRing12(hours, green);
    singleDotRing24(minutes, blue);
    singleDotRing24(seconds, red);
    break;

  case 4:
    // clear the rings
    chaserMode = CHASER_NONE;

    clearRing12();
    clearRing24();

    setQuartersRing12(grey32);

    singleDotRing12(hours, green);
    singleDotRing24(minutes, blue);
    singleDotRing24(seconds, red);
    break; 

  case 5:
    // clear the rings
    chaserMode = CHASER_NONE;

    clearRing12();
    clearRing24();

    singleDotRing12(hours, green);
    singleDotRing24(minutes, blue);
    trailDotsRing24(seconds, red, red2, red3);
    break; 

  case 6:
    // clear the rings
    chaserMode = CHASER_NONE;

    clearRing12();
    clearRing24();

    setQuartersRing12(grey32);

    singleDotRing12(hours, green);
    singleDotRing24(minutes, blue);
    trailDotsRing24(seconds, red, red2, red3);
    break; 
  }

  pixels12.show();
  pixels24.show();

  delay(DELAY);
}













//*********************************************************************************
// SET ALL LEDS ON THE NEOPIXEL 12 RING TO THE OFF COLOR
//*********************************************************************************
void clearRing12(){
  for(int i = 0; i < 12; i++){
    pixels12.setPixelColor(i, off); 
  }
}

//*********************************************************************************
// SET ALL LEDS ON THE NEOPIXEL 24 RING TO THE OFF COLOR
//*********************************************************************************
void clearRing24(){
  for(int i = 0; i < 24; i++){
    pixels24.setPixelColor(i, off); 
  }
}

//*********************************************************************************
// SET THE CHASER DOT ON THE NEOPIXEL 12 RING TO THE PASSED IN COLOR
//*********************************************************************************
void ring12Chaser(uint32_t color){
  if(ring12ChaserLed > -1){
    singleDotRing12(ring12ChaserLed, color);
    ring12ChaserLed++;

    if(ring12ChaserLed > 11){
      ring12ChaserLed = -1;
    }
  }
}

//*********************************************************************************
// GETS THE LED POSITION ON THE NEOPIXEL 24 RING FOR PASSED IN VALUE (0-59)
//*********************************************************************************
int getMinuteSecondLed(int current){
  return (int)((float)current * (24.0 / 60.0));
}

//*********************************************************************************
// SET A SINGLE DOT ON THE NEOPIXEL 12 RING
//*********************************************************************************
void singleDotRing12(int value, uint32_t color){
  pixels12.setPixelColor(value, color); 
}

//*********************************************************************************
// SET A SINGLE DOT ON THE NEOPIXEL 24 RING
//*********************************************************************************
void singleDotRing24(int value, uint32_t color){
  int led = getMinuteSecondLed(value);
  pixels24.setPixelColor(led, color); 
}

//*********************************************************************************
// SET A SINGLE DOT WITH TWO TRAILING DOTS ON THE NEOPIXEL 24 RING
//*********************************************************************************
void trailDotsRing24(int value, uint32_t color, uint32_t trailColor1, uint32_t trailColor2){
  int led = getMinuteSecondLed(value);
  pixels24.setPixelColor(led, color); 

  int trailValue = led - 1;

  if(trailValue < 0){
    trailValue = 23; 
  }

  pixels24.setPixelColor(trailValue, trailColor1); 

  trailValue--;

  if(trailValue < 0){
    trailValue = 23; 
  }

  pixels24.setPixelColor(trailValue, trailColor2); 
}

//*********************************************************************************
// BLEND COLOR ACROSS TWO DOTS ON THE NEOPIXEL 24 RING
//*********************************************************************************
void twoDotBlendRing24(int value, uint32_t color){
  float v = (float)value * (24.0 / 60.0);

  int led1 = (int)v;
  int led2 = (int)v + 1;

  float percent1 = (float)led2 - v;
  float percent2 = 1.0 - percent1;

  //  Serial.println();
  //  Serial.print("value = ");
  //  Serial.println(value);
  //  Serial.print("led1 = ");
  //  Serial.println(led1);
  //  Serial.print("led2 = ");
  //  Serial.println(led2);
  //  Serial.print("percent1 = ");
  //  Serial.println(percent1);
  //  Serial.print("percent2 = ");
  //  Serial.println(percent2);
  //  Serial.println();

  if(led1 > 23){
    led1 = led1 - 24;
  }

  if(led2 > 23){
    led2 = led2 - 24;
  }

  int r = getRed(color);
  int g = getGreen(color);
  int b = getBlue(color);

  int r1 = (int)((float)r * percent1);
  int g1 = (int)((float)g * percent1);
  int b1 = (int)((float)b * percent1);

  if(r1 > 0 || g1 > 0 || b1 > 0){
    uint32_t c1 = pixels24.Color(r1, g1, b1);
    pixels24.setPixelColor(led1, c1);
  } 

  int r2 = (int)((float)r * percent2);
  int g2 = (int)((float)g * percent2);
  int b2 = (int)((float)b * percent2);

  if(r2 > 0 || g2 > 0 || b2 > 0){
    uint32_t c2 = pixels24.Color(r2, g2, b2);
    pixels24.setPixelColor(led2, c2); 
  }
}

//*********************************************************************************
// SET THE QUARTERS ON THE NEOPIXEL 12 RING
//*********************************************************************************
void setQuartersRing12(uint32_t color){
  pixels12.setPixelColor(0, color); 
  pixels12.setPixelColor(3, color); 
  pixels12.setPixelColor(6, color); 
  pixels12.setPixelColor(9, color); 
}

//*********************************************************************************
// SET THE QUARTERS ON THE NEOPIXEL 24 RING
//*********************************************************************************
void setQuartersRing24(uint32_t color){
  pixels24.setPixelColor(23, color); 
  pixels24.setPixelColor(0, color); 
  pixels24.setPixelColor(5, color); 
  pixels24.setPixelColor(6, color); 
  pixels24.setPixelColor(11, color); 
  pixels24.setPixelColor(12, color); 
  pixels24.setPixelColor(17, color); 
  pixels24.setPixelColor(18, color); 
}

//*********************************************************************************
// GETS THE TIME (HOURS, MINUTES, SECONDS) FROM THE CHRONODOT
//*********************************************************************************
void getTime(){
  Wire.beginTransmission(104); 
  Wire.write(0);//set register to 0
  Wire.endTransmission();
  Wire.requestFrom(104, 3);//get 3 bytes (seconds,minutes,hours);

  int previousHours = hours;
  int previousMinutes = minutes;
  int previousSeconds = seconds;

  seconds = bcdToDec(Wire.read() & 0x7f);
  minutes = bcdToDec(Wire.read());
  hours = bcdToDec(Wire.read() & 0x3f); 

  if(chaserMode == CHASER_NONE){
    ring12ChaserLed = -1;
  }

  if(chaserMode == CHASER_HOURS && previousHours != hours){
    ring12ChaserLed = 0;
  }

  if(chaserMode == CHASER_MINUTES && previousMinutes != minutes){
    ring12ChaserLed = 0;
  }

  if(chaserMode == CHASER_SECONDS &&previousSeconds != seconds){
    ring12ChaserLed = 0;
  }

  if(hours > 11){
    hours = hours - 12;
  }
}

//*********************************************************************************
// SETS THE TIME (HOURS, MINUTES, SECONDS) TO THE CHRONODOT
//*********************************************************************************
void setTime()
{
  Wire.beginTransmission(104);
  Wire.write(0);
  Wire.write(decToBcd(seconds));
  Wire.write(decToBcd(minutes));
  Wire.write(decToBcd(hours));
  Wire.endTransmission();
}

//*********************************************************************************
// GETS THE DATE (YEAR, MONTH, DAY OF WEEK, DATE) FROM THE CHRONODOT
//*********************************************************************************
void getDate(){
  Wire.beginTransmission(104); 
  Wire.write(3);//set register to 3 (day)
  Wire.endTransmission();
  Wire.requestFrom(104, 4); //get 5 bytes(day,date,month,year,control);
  dayOfMonth = bcdToDec(Wire.read());
  dayOfWeek  = bcdToDec(Wire.read());
  month      = bcdToDec(Wire.read());
  year       = bcdToDec(Wire.read());
}

//*********************************************************************************
// SETS THE DATE (HOURS, MINUTES, SECONDS) TO THE CHRONODOT
//*********************************************************************************
void setDate()
{
  Wire.beginTransmission(104);
  Wire.write(3);
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

//*********************************************************************************
// CONVERTS FROM ARDUINO VALUE TO CHRONODOT VALUE
//*********************************************************************************
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

//*********************************************************************************
// CONVERTS FROM CHRONODOT VALUE TO ARDUINO VALUE
//*********************************************************************************
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

int getRed(uint32_t c){
  uint8_t
    r = (uint8_t)(c >> 16),
  g = (uint8_t)(c >>  8),
  b = (uint8_t)c;

  return r;
}

int getGreen(uint32_t c){
  uint8_t
    r = (uint8_t)(c >> 16),
  g = (uint8_t)(c >>  8),
  b = (uint8_t)c;

  return g;
}

int getBlue(uint32_t c){
  uint8_t
    r = (uint8_t)(c >> 16),
  g = (uint8_t)(c >>  8),
  b = (uint8_t)c;

  return b;
}


