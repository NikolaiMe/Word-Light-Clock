// WLC BY NIKOLAI UND SARAH


#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "DCF77.h"
#include <DS3231.h>
#include "Time.h"
#include <TimeLib.h>

#define LED_PIN   8
#define SETTING_BUTTON_PIN 6
#define KILLSWITCH_PIN 9
#define LED_COUNT 253
#define DCF77_DATAPIN 2
#define DCF77_INTERRUPT 0
#define NIGHTMODE_START 1
#define NIGHTMODE_END 5
#define FADEOUT_TIME 2000
#define FADEIN_TIME 2000
#define FADEBRIGHT_TIME 2000
#define ANIVERSARY_DAY 16
#define ANIVERSARY_MONTH 11

// ++ ARRAYS FOR WORD-LED-MATCHING ++
const PROGMEM int w_es[]  = {252, 251, 207, 208, 255};
const PROGMEM int w_ist[]  = { 161, 162, 160, 159, 115, 116, 255};
const PROGMEM int w_fuenf_min[]  = {69, 70, 68, 67, 23, 24, 22, 21, 255};
const PROGMEM int w_zehn_min[]  = {250, 249, 209, 210, 204, 203, 163, 164, 255};
const PROGMEM int w_vor_1[]  = {71, 72, 65, 66, 25, 26, 255};
const PROGMEM int w_nach_1[]  = {246, 247, 248, 212, 213, 202, 201, 200, 166, 167, 255};
const PROGMEM int w_viertel[]  = {154, 155, 120, 121, 109, 108, 74, 75, 62, 63, 28, 29, 16, 17, 18, 255};
const PROGMEM int w_halb[]  = {245, 244, 214, 215, 199, 198, 168, 169, 255};
const PROGMEM int w_vor_2[]  = {153, 152, 122, 123, 107, 106, 255};
const PROGMEM int w_nach_2[]  = {76, 77, 61, 60, 30, 31, 15, 14, 255};
const PROGMEM int w_ein_hour[]  = {242, 243, 216, 217, 197, 196, 255};
const PROGMEM int w_eins_hour[]  = {242, 243, 216, 217, 197, 196, 170, 171, 255};
const PROGMEM int w_zwei_hour[]  = {78, 79, 59, 58, 32, 33, 13, 12, 255};
const PROGMEM int w_drei_hour[]  = {240, 239, 219, 220, 194, 193, 173, 174, 255};
const PROGMEM int w_vier_hour[]  = {81, 82, 56, 55, 35, 36, 10, 9, 255};
const PROGMEM int w_fuenf_hour[]  = {238, 237, 221, 222, 191, 192, 175, 176, 255};
const PROGMEM int w_sechs_hour[]  = {100, 99, 83, 84, 54, 53, 37, 38, 8, 7, 255};
const PROGMEM int w_sieben_hour[]  = {236, 235, 223, 224, 190, 189, 177, 178, 144, 143, 132, 133, 255};
const PROGMEM int w_acht_hour[]  = { 85, 86, 52, 51, 39, 40, 6, 5, 255};
const PROGMEM int w_neun_hour[]  = { 233, 232, 226, 227, 187, 186, 180, 181, 255};
const PROGMEM int w_zehn_hour[]  = { 141, 140, 134, 135, 95, 94, 88, 89, 255};
const PROGMEM int w_elf_hour[]  = {49, 48, 42, 43, 3, 2, 255};
const PROGMEM int w_zwoelf_hour[]  = {231, 230, 228, 229, 185, 184, 183, 182, 139, 138, 255};
const PROGMEM int w_uhr[]  = {47, 46, 45, 44, 0, 1, 255};


const PROGMEM int w_sundn[]  = {170, 171, 150, 151, 124, 125, 255};
const PROGMEM int w_mist[]  = {161, 162, 160, 159, 115, 116, 206, 205, 255};
const PROGMEM int w_by[]  = {117, 118, 158, 157, 255};
const PROGMEM int w_funkuhr[]  = {139, 138, 136, 137, 93, 92, 91, 90, 47, 46, 44, 45, 0, 1, 255};


// ++ DATATYPES ++

enum OperatingMode {
  normal,
  loveless,
  rec_finder,
  startup_mode,
  night_mode
};

// +++ BASIC GLOBAL VARIABLES +++
OperatingMode opMode = startup_mode;//startup_mode;      // indicates in which working Mode the WLC is right now
OperatingMode lastMode = startup_mode;// startup_mode;
bool makeDefault = false;

// +++ VARIABLES FOR SETTING BUTTON +++
bool settingBtnPsd = false;         // saves the last State of the Setting Button
unsigned long settingBtnPsdTime = 0;    // saves the time when the setting button was pressed

// +++ VARIABLES FOR LED STRIPE +++
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t col_white = strip.Color(255, 255, 255);
uint32_t col_red =  strip.gamma32(strip.Color(194,92,66)); 
uint32_t col_black = 0;

uint32_t frontcolor; 
uint32_t frontcolor_fade;  
int front_red;
int front_green;
int front_blue;

uint32_t backcolor;
uint32_t backcolor_fade;
int back_red;
int back_green;
int back_blue;

int div_red;
int div_green;
int div_blue;

int curr_back_red;
int curr_back_green;
int curr_back_blue;

int curr_front_red;
int curr_front_green;
int curr_front_blue;

int colormode = 0;

// +++ VARIABLES FOR LIGHT SENSOR +++
int maxLightValue = 100;
int minLightValue = 20;
int brightness = 100;
bool ledField[253];

// +++ VARIABLES FOR RECEPTION MODE +++
int prevSensorValue = 0;
unsigned long signalStart = 0;
unsigned long signalChange = 0;
bool receptionGood = false;
bool recLed = false;


// +++ VARIABLES FOR TIME +++
int lastSecond = 0;
bool secSync = false;
int lastFiveMin = 0;
bool fiveMinSync = false;
int lastMin = 0;
bool minSync = false;
int lastFiveSec = 0;
bool fiveSecSync = false;
bool fadeTimeSync = false;
unsigned long lastFadeTime = 0;
bool dcfSyncPerformed = false;
bool dcfSyncStarted = false;
DCF77 DCF = DCF77(DCF77_DATAPIN,DCF77_INTERRUPT);
DS3231 RTC;


// +++ VARIABLES FOR FADING +++
unsigned long fadeStartTime = 0;
bool fadeOngoing = false;

bool textFadeOngoing = false;
bool textFadeRequest = false;
bool textFadeOutOngoing = false;
bool textFadeInOngoing = false;
bool brightFadeOngoing = false;
bool brightFadeRequest = false;

// +++ VARIABLES FOR EASTEREGGS +++
bool s_n_active = false;



void setup() {
  // Start Light Sensor
  startLightSensor();

  // Start Serial Communication
  Serial.begin(9600);

  // Start LED Stripe
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.println("Show Init");

  // Initialize the Brightness Taster
  pinMode(SETTING_BUTTON_PIN, INPUT_PULLUP);

  // Initialize the Killswitch
  pinMode(KILLSWITCH_PIN, INPUT);

  // Funkuhr Stuff
  pinMode(DCF77_DATAPIN, INPUT);
  digitalWrite(DCF77_DATAPIN, LOW);
  DCF.Start();

  setLEDColors(col_white, col_red);

  



  // Wait one Second to give everything some time for initialisation
  delay(1000);

}


void loop() {

  secSyncer();
  fiveSecSyncer();
  minSyncer();
  fiveMinSyncer();
  fadeTimeSyncer();
  setClockManually();
  correctInternalClock();
  digitalClockDisplay();


  checkNightMode();
  checkButton();
  checkKillswitch();

  if(makeDefault)
  {
    secSync = false;
    fiveSecSync = false;
    minSync = false;
    fiveMinSync = false;
    fadeTimeSync = false;
    fadeOngoing = false;
    textFadeOngoing = false;
    textFadeRequest = false;
    textFadeOutOngoing = false;
    textFadeInOngoing = false;
    brightFadeOngoing = false;
    brightFadeRequest = false;
    makeDefault = false;
  }


  
  switch (opMode)
  {
    case normal:
      changeTimeDisplay();
      checkBrightness();
      checkSundN();
      if(textFadeOngoing || textFadeRequest)
      {
        fadeTextDisplay();
      }

      if(brightFadeOngoing)
      {
        fadeBrightness();
      }
      break;
  
    case loveless:
      changeTimeDisplay();
      checkBrightness();
      if(textFadeOngoing || textFadeRequest)
      {
        fadeTextDisplay();
      }

      if(brightFadeOngoing)
      {
        fadeBrightness();
      }
      break;
  
    case rec_finder:
      showCurrentReception();
      break;

    case startup_mode:
      correctTimeIfAvailable();
      if(dcfSyncPerformed)
        {
          opMode = normal;
          showDefault();
        }
      break;

    case night_mode:
        if(!dcfSyncPerformed && dcfSyncStarted)
        {
          correctTimeIfAvailable();
        }
      break;
  
    default:
      showCurrentReception();
      break;
  }


  

  secSync = false;
  fiveSecSync = false;
  minSync = false;
  fiveMinSync = false;
  fadeTimeSync = false;
  
}


void showDefault()
{
  brightness = getLightValue();
  calcBackBrightness(brightness);
  calcFrontBrightness(brightness);
  minSync = true;
  correctInternalClock();
  refreshLedMatrix();
  writeWordToLedMatrix(w_es);
  writeWordToLedMatrix(w_ist);
  writeTime(hour(), minute());
  showLedMatrix(frontcolor);
  makeDefault = true;
}



// +++ LIGHT SENSOR FUNCTIONS +++

void startLightSensor()
{
  //I2C Interface is started
    Wire.begin();
    delay(300);
    Wire.beginTransmission(35); // --> ADDR Byte Low  --> 0100011 = 35
    Wire.write(16);
    Wire.endTransmission();
    delay(300);
}

int getLightValue()
{

    float helper = 0;
    int returnValue = 0;
    int receivedData = getLightSensorData();
  
    if(receivedData>maxLightValue) receivedData= maxLightValue;
    
    helper = (float)receivedData/maxLightValue;
    helper = helper * 100;
    returnValue = (int) helper;
    if (returnValue < minLightValue) returnValue = minLightValue;
  
    return returnValue;
  
}

int getLightSensorData()
{
  int receivedData=0;
  int counter = 0;
  Wire.requestFrom(35, 2);    // request 2 bytes from slave device #35
  while(Wire.available())    // slave may send less than requested
  { 
    int c = Wire.read(); // receive a byte as character
    receivedData|=(c << 8 - (counter * 8));
    if (counter < 1) counter++; else counter=0;
  }

  return receivedData;

}





// +++ CLOCK FUNCTIONS +++

void correctInternalClock()
{
  if(minSync)
  {
    setTime(getRTC());
    Serial.println("Timesync von RTC");
  }
}

void correctTimeIfAvailable()
{
  time_t DCFtime = DCF.getTime();
    if (DCFtime!=0)
    {
      Serial.println("Time is updated");
      dcfSyncPerformed = true;
      setRTC(DCFtime);
      DCF.Stop();
    }  
}

void showCurrentReception()
{
  int sensorValue = digitalRead(DCF77_DATAPIN);
  if (sensorValue==1 && prevSensorValue==0) 
  { 
    unsigned long t_now = millis();
    unsigned long signalTime = t_now - signalStart;
    unsigned long signalTime2 = signalChange - signalStart;

    if(signalTime>980 && signalTime<1020 && signalTime2<220)
    {
      receptionGood = true;
    }
    else 
    {
      receptionGood = false;
    }
   
    uint32_t recColor = (receptionGood) ?  strip.Color(0, 200, 0) : strip.Color(200, 0, 0) ;

    strip.clear();
    int ledNr;  
        
    if(recLed)
    {
      ledNr = 1;
      recLed = false;
    }
    else
    {
      ledNr = 3;
      recLed = true;
    }

    strip.setPixelColor(ledNr, recColor);

    prevSensorValue = sensorValue;
    signalStart = t_now;

    strip.show();
    Serial.println("Show Curr Reception");


  } else if (sensorValue==0 && prevSensorValue==1) 
  {
    signalChange = millis();
    prevSensorValue = sensorValue;
  }
}

void secSyncer()
{
  if (lastSecond != second())
  {
    secSync = true;
    lastSecond = second();
  }
}

void minSyncer()
{
    if (lastMin != (minute()))
  {
    minSync = true;
    lastMin = minute();
  }
}

void fiveMinSyncer()
{
  if (lastFiveMin != (minute()/5))
  {
    fiveMinSync = true;
    lastFiveMin = minute()/5;
  }
}

void fiveSecSyncer()
{
  if (lastFiveSec != (second()/5))
  {
    fiveSecSync = true;
    lastFiveSec = second()/5;
  }
}


void fadeTimeSyncer()
{
  if (lastFadeTime != (millis()/20))
  {
    fadeTimeSync = true;
    lastFadeTime = millis()/20;
  }
}

void setRTC(time_t time)
{
    RTC.setClockMode(false);  // set to 24h
    int year_temp = year(time)%100;
    byte year_ = (byte) year_temp & 0xFF;
    RTC.setYear((byte) year_temp & 0xFF);
    RTC.setMonth((byte)month(time) & 0xFF);
    RTC.setDate((byte)day(time) & 0xFF);
    RTC.setDoW((byte) weekday(time));
    RTC.setHour((byte) hour(time) & 0xFF);
    RTC.setMinute((byte) minute(time) & 0xFF);
    RTC.setSecond((byte) second(time) & 0xFF);
}

time_t getRTC()
{
  tmElements_t tm;
  bool pm;
  bool h12;
  bool century;
  tm.Second = (int) RTC.getSecond();
  tm.Minute = (int) RTC.getMinute();
  tm.Hour  = (int) RTC.getHour(h12, pm);
  tm.Day = (int) RTC.getDate();
  tm.Month = (int) RTC.getMonth(century);
  tm.Year = (int) RTC.getYear()+30;
  tm.Wday = (int) RTC.getDoW();

  return makeTime(tm);
}

void checkNightMode()
{
  if(fiveMinSync)
  {
    int stunde = hour();
    if(stunde>=NIGHTMODE_START && stunde<NIGHTMODE_END)
    {
      if(opMode != night_mode)
      {
        strip.clear();
        strip.show();
        opMode = night_mode;
      }

      if(!dcfSyncStarted && (stunde>=4))
      {
        dcfSyncPerformed = false;
        dcfSyncStarted = true;
        DCF.Start();
      }
      
    }
    else
    {
      if(opMode == night_mode)
      {
        dcfSyncPerformed = true;
        dcfSyncStarted = false;
        DCF.Stop();
        showDefault();
        opMode = normal;
      }
    }
  }
}


// +++ UI CHECKER FUNCTIONS +++

void checkButton()
{
  if( (digitalRead(SETTING_BUTTON_PIN)==0) && (!settingBtnPsd) )
  {
    settingBtnPsd = true;
    settingBtnPsdTime = millis();
  }
  else if ( (digitalRead(SETTING_BUTTON_PIN)==1) && (settingBtnPsd) )
  {
    unsigned long timePressed = millis() - settingBtnPsdTime;
    if(timePressed < 2000)
    {
      switch (colormode)
      {
        case 0 :
          colormode = 1;
          setLEDColors(col_white, col_black);
          minLightValue = 10;
          break;

        case 1 :
          colormode = 2;
          setLEDColors(col_red, col_black);
          minLightValue = 20;
          break;

        case 2 :
          colormode = 3;
          setLEDColors(col_black, col_black);
          break;

        case 3 :
          colormode = 0;
          setLEDColors(col_white, col_red);
          minLightValue = 20;
          break;

        default:
          colormode = 0;
          setLEDColors(col_white, col_red);
          minLightValue = 20;
          break;
      }

      showDefault();
      
    }
    else if (timePressed < 6000)
    {
      maxLightValue = getLightSensorData();
    } else {
      if(opMode != rec_finder)
      {
        strip.clear();
        strip.show(); 
        Serial.println("Show CheckButton");
        lastMode = opMode;
        opMode = rec_finder;
        makeDefault = true;
      }else
      {
        strip.clear();
        strip.show(); 
        Serial.println("Show CheckButton");
        
        opMode=lastMode;
        if(opMode == loveless || opMode == normal) showDefault();
      }
    }

    settingBtnPsd = false;
  }
}

void checkKillswitch()
{
  if((digitalRead(KILLSWITCH_PIN)==1) && (opMode == normal))
    {
      opMode = loveless;
    }
    else if ((digitalRead(KILLSWITCH_PIN)==0) && (opMode == loveless))
    {
      opMode = normal;
    }
}


// +++ LED FUNCTIONS +++

void showWord(int wort[], uint32_t color)
{
  int i = 0;
  int readnr = pgm_read_word_near(wort);
  while(readnr!=255)
  {
    strip.setPixelColor(readnr, color);
    i++;
    readnr = pgm_read_word_near(wort+i);
  }
}

void writeWordToLedMatrix(int wort[])
{
  int i = 0;
  int readnr = pgm_read_word_near(wort);
  while(readnr!=255)
  {
    ledField[readnr] = true;
    i++;
    readnr = pgm_read_word_near(wort+i);
  }
}


void refreshLedMatrix()
{
  for(int i = 0; i<253; i++)
  {
    ledField[i] = false;
  }
}


void showLedMatrix(uint32_t color)
{
  strip.clear();
  fillBackground(backcolor);
  for(int i = 0; i<253; i++)
  {
    if(ledField[i])
    {
      strip.setPixelColor(i, color);
    }
  }
  
  strip.show();
  Serial.println("Show LedMatrix");
}

void changeTimeDisplay()
{
  if(fiveMinSync == true)
  {
    refreshLedMatrix();
    writeWordToLedMatrix(w_es);
    writeWordToLedMatrix(w_ist);
    writeTime(hour(), minute());
    fadeTextDisplay();
  }
}



void writeTime(int currHour, int currMinute)
{
  int fiveminute = currMinute/5;
  fiveminute = fiveminute * 5;

  switch (fiveminute)
  {
    case 0:
      writeWordToLedMatrix(w_uhr);
    break;

    case 5:
      writeWordToLedMatrix(w_fuenf_min);
      writeWordToLedMatrix(w_nach_1);
    break;

    case 10:
      writeWordToLedMatrix(w_zehn_min);
      writeWordToLedMatrix(w_nach_2);
    break;

    case 15:
      writeWordToLedMatrix(w_viertel);
      writeWordToLedMatrix(w_nach_2);
    break;

    case 20:
      writeWordToLedMatrix(w_zehn_min);
      writeWordToLedMatrix(w_vor_1);
      writeWordToLedMatrix(w_halb);
    break;

    case 25:
      writeWordToLedMatrix(w_fuenf_min);
      writeWordToLedMatrix(w_vor_1);
      writeWordToLedMatrix(w_halb);
    break;

    case 30:
      writeWordToLedMatrix(w_halb);
    break;

    case 35:
      writeWordToLedMatrix(w_fuenf_min);
      writeWordToLedMatrix(w_nach_1);
      writeWordToLedMatrix(w_halb);
    break;

    case 40:
      writeWordToLedMatrix(w_zehn_min);
      writeWordToLedMatrix(w_nach_1);
      writeWordToLedMatrix(w_halb);
    break;

    case 45:
      writeWordToLedMatrix(w_viertel);
      writeWordToLedMatrix(w_vor_2);
    break;
    
    case 50:
      writeWordToLedMatrix(w_zehn_min);
      writeWordToLedMatrix(w_vor_2);
    break;
    
    case 55:
      writeWordToLedMatrix(w_fuenf_min);
      writeWordToLedMatrix(w_vor_2);
    break;
    
    default:

    break;
  }

   if(currMinute > 19 )
  {
    currHour = currHour + 1;
  }

  switch (currHour)
  {
    case 1:
    case 13:
      if(fiveminute < 5 )
      {
         writeWordToLedMatrix(w_ein_hour);
      }
      else
      {
         writeWordToLedMatrix(w_eins_hour);
      }
    break;
    
    case 2:
    case 14:
      writeWordToLedMatrix(w_zwei_hour);
    break;

    case 3:
    case 15:
      writeWordToLedMatrix(w_drei_hour);
    break;

    case 4:
    case 16:
      writeWordToLedMatrix(w_vier_hour);
    break;

    case 5:
    case 17:
      writeWordToLedMatrix(w_fuenf_hour);
    break;
    
    case 6:
    case 18:
      writeWordToLedMatrix(w_sechs_hour);
    break;

    case 7:
    case 19:
      writeWordToLedMatrix(w_sieben_hour);
    break;

    case 8:
    case 20:
      writeWordToLedMatrix(w_acht_hour);
    break;

    case 9:
    case 21:
      writeWordToLedMatrix(w_neun_hour);
    break;

    case 10:
    case 22:
      writeWordToLedMatrix(w_zehn_hour);
    break;

    case 11:
    case 23:
      writeWordToLedMatrix(w_elf_hour);
    break;

    case 0:
    case 12:
    case 24:
      writeWordToLedMatrix(w_zwoelf_hour);
    break;

    default:
    break;

  }
}

void fillBackground(uint32_t backcolor)
{

  strip.fill(backcolor, 0, LED_COUNT);
  for (int i = 0; i<5; i++)
  {
     strip.setPixelColor(4+i*46, 0);
     strip.setPixelColor(11+i*46, 0);
     strip.setPixelColor(27+i*46, 0);
     strip.setPixelColor(34+i*46, 0);
     strip.setPixelColor(41+i*46, 0);
     
     
  }
  strip.setPixelColor(4+5*46, 0);
  strip.setPixelColor(11+5*46, 0);
}

void setLEDColors(uint32_t new_frontcolor, uint32_t new_backcolor)
{
  frontcolor = new_frontcolor;
  backcolor = new_backcolor;
  
  back_red = getRedValue(new_backcolor);
  back_green = getGreenValue(new_backcolor);
  back_blue = getBlueValue(new_backcolor);

  front_red = getRedValue(new_frontcolor);
  front_green = getGreenValue(new_frontcolor);
  front_blue = getBlueValue(new_frontcolor);

  div_red = front_red - back_red;
  div_green = front_green - back_green;
  div_blue = front_blue - back_blue;
}

void calcBackBrightness(int brightness)
{
  uint32_t red = (brightness*back_red)/100;
  uint32_t green = (brightness*back_green)/100;
  uint32_t blue = (brightness*back_blue)/100;

  curr_back_red = red;
  curr_back_green = green;
  curr_back_blue = blue;

  backcolor = strip.Color(red, green, blue);


  div_red = curr_front_red - red;
  div_green = curr_front_green - green;
  div_blue = curr_front_blue - blue;
}


void calcFrontBrightness(int brightness)
{
  uint32_t red = (brightness*front_red)/100;
  uint32_t green = (brightness*front_green)/100;
  uint32_t blue = (brightness*front_blue)/100;

  frontcolor =  strip.Color(red, green, blue);

  curr_front_red = red;
  curr_front_green = green;
  curr_front_blue = blue;

  div_red = red - curr_back_red;
  div_green = green - curr_back_green;
  div_blue = blue - curr_back_blue;

}

int fade(unsigned long fadeMillis)
{
  int fadeValue;
  if(!fadeOngoing) fadeOngoing = true;

  unsigned long currMillis = millis();
  
  if(currMillis>fadeStartTime+fadeMillis || currMillis<fadeStartTime)
  {
    fadeValue = 100;
    fadeOngoing = false;
  }
  else
  {
    unsigned long currFadeTime = currMillis - fadeStartTime;
    unsigned long fadePercentage = (currFadeTime * 100) / fadeMillis;



    fadeValue = (int) fadePercentage;
  }

  return fadeValue;
}

void checkBrightness()
{
  if((fiveSecSync == true) && (lastFiveSec != 11) && (!textFadeOngoing) &&(!brightFadeOngoing) )
  {
    int currBrightness = getLightValue();
    int brightDiv = brightness - currBrightness;
    Serial.print("currBrightness: ");
    Serial.println(currBrightness);
    Serial.print("brightness: ");
    Serial.println(brightness);
    if(brightDiv>9 || brightDiv<-9)
    {
      backcolor_fade = backcolor;
      frontcolor_fade = frontcolor;
      calcFrontBrightness((currBrightness/10)*10);
      calcBackBrightness((currBrightness/10)*10);
      brightness = currBrightness;
      fadeBrightness();
    }
  }
}

void fadeBrightness()
{
  if(!brightFadeOngoing)
  {
    Serial.println("Start");
    brightFadeOngoing = true;
    fadeStartTime = millis();
    fade(FADEBRIGHT_TIME);
  }else{
    if(fadeTimeSync)
    {
      if(fadeOngoing)
      {
        int fadevalue = fade(FADEBRIGHT_TIME);
        int red_bc = getRedValue(backcolor_fade) - (((getRedValue(backcolor_fade) - getRedValue(backcolor))*fadevalue)/100);
        int green_bc = getGreenValue(backcolor_fade) - (((getGreenValue(backcolor_fade) - getGreenValue(backcolor))*fadevalue)/100);
        int blue_bc = getBlueValue(backcolor_fade) - (((getBlueValue(backcolor_fade) - getBlueValue(backcolor))*fadevalue)/100);

        int red_fc = getRedValue(frontcolor_fade) - (((getRedValue(frontcolor_fade) - getRedValue(frontcolor))*fadevalue)/100);
        int green_fc = getGreenValue(frontcolor_fade) - (((getGreenValue(frontcolor_fade) - getGreenValue(frontcolor))*fadevalue)/100);
        int blue_fc = getBlueValue(frontcolor_fade) - (((getBlueValue(frontcolor_fade) - getBlueValue(frontcolor))*fadevalue)/100);
        
        fillBackground(strip.Color(red_bc, green_bc, blue_bc));

        for(int i = 0; i<253; i++)
        {          
          if(ledField[i])
           {
             strip.setPixelColor(i, strip.Color(red_fc, green_fc, blue_fc));
           }
        }
        strip.show();
      }else{
        showLedMatrix(frontcolor);
        brightFadeOngoing = false;
        Serial.println("Ende");
      }
    }
  }
}

void fadeTextDisplay()
{

  // Diese Funktion managed alles was das Text Fading betrifft
    if(!textFadeOngoing) 
    {
      if(brightFadeOngoing)
      {
        textFadeRequest = true;
      } else {
        textFadeOngoing = true;
        textFadeRequest = false;
      }
    } 

    if(fadeTimeSync && textFadeOngoing)
    {
      if(!textFadeOutOngoing && !textFadeInOngoing && !fadeOngoing)
      {
        fadeoutText();
      }else if(textFadeOutOngoing && !textFadeInOngoing && fadeOngoing)
      {
        fadeoutText();
      }else if(textFadeOutOngoing && !textFadeInOngoing && !fadeOngoing)
      {
        textFadeOutOngoing = false;
        fadeinText();
      }else if(!textFadeOutOngoing && textFadeInOngoing && fadeOngoing)
      {
        fadeinText();
      }else{
        // Schreibe die komplette Matrix nochmal, damit alle LEDs die aus sein sollen auch aus sind
        showLedMatrix(frontcolor);
        textFadeInOngoing = false;
        textFadeOutOngoing = false;
        textFadeOngoing = false;
      }
    }else if(fadeTimeSync && !textFadeOngoing){
      if(textFadeInOngoing||textFadeInOngoing)
      {
        textFadeInOngoing = false;
        textFadeOutOngoing = false;
      }
    }
  
}

void fadeoutText()
{
  if(!textFadeOutOngoing)
  {
    // Finde Heraus welche LEDs VG sind, aber HG sein sollten
    textFadeOutOngoing = true;
    bool fadingNecessary = false;
    fadeStartTime = millis();
    // Schalte diese LEDs ein bisschen runter
    if(backcolor != frontcolor)
    {
      uint32_t littleLowerLight;

      if(curr_front_red != 0) littleLowerLight = strip.Color(curr_front_red - 1, curr_front_green, curr_front_blue);
      else if (curr_front_green != 0) littleLowerLight = strip.Color(curr_front_red, curr_front_green - 1, curr_front_blue);
      else if (curr_front_blue != 0) littleLowerLight = strip.Color(curr_front_red, curr_front_green, curr_front_blue - 1);
      else littleLowerLight = strip.Color(curr_front_red, curr_front_green, curr_front_blue);

      for(int i = 0; i<253; i++)
      {
        if(strip.getPixelColor(i)==frontcolor && !ledField[i] )
        {
          strip.setPixelColor(i, littleLowerLight);
          fadingNecessary = true;
        }
      }
      if (fadingNecessary) fade(FADEOUT_TIME);
    }
  }else{
    // danach fade immer die LEDs, die schon ein bisschen runter gedimmt sind bis sie die HG Farbe erreicht haben
    int fadevalue = fade(FADEOUT_TIME);
    int redNow = curr_front_red - ((div_red * fadevalue)/100);
    int greenNow = curr_front_green - ((div_green * fadevalue)/100);
    int blueNow = curr_front_blue  - (((div_blue) * fadevalue)/100);

    // Abchecken, dass sich beim ersten mal irgendwas verändert hat, sonst wird nämlich das "Anfaden" (siehe littleLowerLight oben) rückgängig gemacht
    if(blueNow != curr_front_blue || greenNow != curr_front_green || redNow != curr_front_red)
    {
      for(int i = 0; i<253; i++)
      {
        if(strip.getPixelColor(i)!=frontcolor && strip.getPixelColor(i)!=backcolor && strip.getPixelColor(i)!=0)
        {
          strip.setPixelColor(i, strip.Color(redNow, greenNow, blueNow));
        }
      }
      strip.show();
    }
  }
}
  

void fadeinText()
{
  if(!textFadeInOngoing)
  {
    // Finde heraus welche LEDs HG oder aus sind, aber VG sein sollten
    textFadeInOngoing = true;
    fadeStartTime = millis();
    bool fadingNecessary = false;
    // Schalte diese LEDs ein bisschen hoch
    if(backcolor != frontcolor)
    {
      uint32_t littleBrighterLight;


      int red_n = 255;
      int green_n = 255;
      int blue_n = 255;
      if(curr_back_red != 255) red_n = curr_back_red + 1;
      if(curr_back_green != 255) green_n = curr_back_green +1;
      if (curr_back_blue != 255) blue_n = curr_back_blue +1;

      littleBrighterLight = strip.Color(red_n, green_n, blue_n);

      for(int i = 0; i<253; i++)
      {
        if((strip.getPixelColor(i)== backcolor && ledField[i]) || (strip.getPixelColor(i) == 0 && ledField[i]))
        {
          strip.setPixelColor(i, littleBrighterLight);
          fadingNecessary = true;
        }
      }
      if(fadingNecessary) fade(FADEIN_TIME);
    }
  }else{
    // danach fade immer die LEDs, die schon ein bisschen hoch gedimmt sind bis sie die VG Farbe erreicht haben
    int fadevalue = fade(FADEIN_TIME);
    int redNow = curr_back_red + ((div_red * fadevalue)/100);
    int greenNow = curr_back_green + ((div_green * fadevalue)/100);
    int blueNow = curr_back_blue +(((div_blue) * fadevalue)/100);

    // Abchecken, dass sich beim ersten mal irgendwas verändert hat, sonst wird nämlich das "Anfaden" (siehe littleBrighterLight oben) rückgängig gemacht
    if(blueNow != curr_back_blue || greenNow != curr_back_green || redNow != curr_back_red)
    {
      for(int i = 0; i<253; i++)
      {
        if(strip.getPixelColor(i)!=frontcolor && strip.getPixelColor(i)!=backcolor && strip.getPixelColor(i)!=0)
        {
          strip.setPixelColor(i, strip.Color(redNow, greenNow, blueNow));
        }
      }
      strip.show();
    }
  }

}


int getRedValue(uint32_t color)
{
  int redValue = 0;
  redValue |= (color >> 16);
  redValue &= 0xff;
  return redValue;
}

int getGreenValue(uint32_t color)
{
  int greenValue = 0;
  greenValue |= (color >> 8);
  greenValue &= 0xff;
  return greenValue;
}

int getBlueValue(uint32_t color)
{
  int blueValue = 0;
  blueValue = color & 0xff;
  return blueValue;
}


//   +++ EASTEREGGS +++

void checkSundN()
{
  if(minSync)
  {
    if((hour()==ANIVERSARY_DAY)&&(minute()==ANIVERSARY_MONTH))
    {
      refreshLedMatrix();
      writeWordToLedMatrix(w_sundn);
      fadeTextDisplay();
    }

    if((hour()==ANIVERSARY_DAY)&&(minute()==ANIVERSARY_MONTH+1))
    {
      refreshLedMatrix();
      writeWordToLedMatrix(w_es);
      writeWordToLedMatrix(w_ist);
      writeTime(hour(), minute());
      fadeTextDisplay();
    }
   
  }
}







// TESTFUNKTIONEN --> FUER DEBUGGING RELEVANT 
int recCounter = 0;

void readNumberFromSerial()
{
    if(Serial.available())
  {
    char buffer[3]; 
    recCounter = 0;
    strip.clear();

    while(Serial.available() && recCounter < 3)
    {
      buffer[recCounter] = Serial.read();
      recCounter++;
      delay(100);

    }

    if (recCounter < 3) buffer[recCounter] = "\0";

    while(Serial.available())
    {
      int i = Serial.read();
    }

    int number = strtoul(buffer, NULL, 10);
    Serial.println(number);

    if (number > 23) number=number - 23;

  }
}


void digitalClockDisplay(){
  // digital clock display of the time
  if(secSync)
  {
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
  }
}

void printDigits(int digits)
{
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)  Serial.print('0');
  Serial.print(digits);
}

byte Year;
byte Month;
byte Date;
byte DoW;
byte Hour;
byte Minute;
byte Second;

void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
    byte& Hour, byte& Minute, byte& Second) {
  boolean GotString = false;
  char InChar;
  byte Temp1, Temp2;
  char InString[20];

  byte j=0;
  while (!GotString) {
    if (Serial.available()) {
      InChar = Serial.read();
      InString[j] = InChar;
      j += 1;
      if (InChar == 'x') {
        GotString = true;
      }
    }
  }
  Serial.println(InString);
  // Read Year first
  Temp1 = (byte)InString[0] -48;
  Temp2 = (byte)InString[1] -48;
  Year = Temp1*10 + Temp2;
  // now month
  Temp1 = (byte)InString[2] -48;
  Temp2 = (byte)InString[3] -48;
  Month = Temp1*10 + Temp2;
  // now date
  Temp1 = (byte)InString[4] -48;
  Temp2 = (byte)InString[5] -48;
  Day = Temp1*10 + Temp2;
  // now Day of Week
  DoW = (byte)InString[6] - 48;   
  // now Hour
  Temp1 = (byte)InString[7] -48;
  Temp2 = (byte)InString[8] -48;
  Hour = Temp1*10 + Temp2;
  // now Minute
  Temp1 = (byte)InString[9] -48;
  Temp2 = (byte)InString[10] -48;
  Minute = Temp1*10 + Temp2;
  // now Second
  Temp1 = (byte)InString[11] -48;
  Temp2 = (byte)InString[12] -48;
  Second = Temp1*10 + Temp2;
}

void setClockManually()
{
  // Send YYMMDDwHHMMSS, with an 'x' at the end via Serial Port. (w = Day of week, zahl zw. 0 und 6), bspw.: 2004101094453x (Achtung! Kein Zeilenende muss ausgewählt sein) 
  if (Serial.available()) {
    GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);

    RTC.setClockMode(false);  // set to 24h
    //setClockMode(true); // set to 12h

    RTC.setYear(Year);
    RTC.setMonth(Month);
    RTC.setDate(Date);
    RTC.setDoW(DoW);
    RTC.setHour(Hour);
    RTC.setMinute(Minute);
    RTC.setSecond(Second);

    minSync = true;
  }
}
