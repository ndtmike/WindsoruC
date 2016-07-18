//******************************************************************************
//  Filename: Windsor.c
//
//  Copyright 2006-2010, NDT James Instruments Inc.  All rights reserved.
//
//  Description:
//  ============
//  This file contains all routines associated with the Windsor Probe.
//
//  Revision History:8
//  =================
//  Date        Author         Description
//  ----------  -------------  -------------------------------------------------
//  10/26/2010  Charlie Green  Initial version
//  6/3/2016    M.W. Hoag      remove wait for 'S' for upload
//
//******************************************************************************

//******************************************************************************
//  Include Files
//******************************************************************************
#include <16F77.h>
#include "Windsor.h"

//******************************************************************************
//  Global Variables
//******************************************************************************
byte                adcData[3];
byte                adcFullScale;
signed long         adcReading;
long                adcScale;
byte                adcZero;
short int           calStartCal;
short int           dataClear;
byte                dataTestNumber;
int32         distance;
long                eepromMemPtr;
short int           keyClear;
signed              keyCount;
short int           keyCountNew;
short int           keyEnterEscape;
signed              keyMax;
signed              keyMin;
short int           keyNewDetection;
short int           keySet;
byte                lcdData[16];
byte                lcdPosition;
short int           menuInitSubmenu;
byte                menuLocationNum;
short int           menuShowSubmenu;
short int           showTest;
short int           showTime;
short int           showTitle;
byte                submenuAggSize;
byte                submenuDensity;
byte                submenuMohs;
byte                submenuPower;
short int           submenuSetAggSize;
short int           submenuSetDensity;
short int           submenuSetMohs;
short int           submenuSetPower;
short int           submenuSetUnits;
short int           submenuSetWeight;
byte                submenuUnits;
byte                submenuWeight;
short int           testClearT;
short int           testDone;
short int           testError;
short int           testOk;
byte                testSetCount;
//short int           testSetCount;
short int           testShowT;
byte                timeRTCData[7];
short int           timeSetClock;
short int           timeUpdate;


//******************************************************************************
//  Config Functions
//******************************************************************************

//******************************************************************************
//
//  Function: Config_initialize()
//
//  Description:
//  ============
//  This function initializes most global variables.
//
//  NOTE: The initializations of submenuSetAggSize, submenuSetPower, and
//        submenuSetUnits have been commented out due to code memory space.
//        They should be added back in if code memory is conserved someplace
//        else.
//
//******************************************************************************
void Config_initialize(void)
{
  calStartCal = false;
  dataClear = false;
  dataTestNumber = 0;
  keyClear = false;
  keyCount = MENU_MEASURE;
  keyCountNew = false;
  keyEnterEscape = false;
  keyMax = MENU_ENTER_SETUP;
  keyMin = MENU_MEASURE;
  keyNewDetection = false;
  keySet = false;
  menuInitSubmenu = false;
  menuLocationNum = 0;
  menuShowSubmenu = false;
  showTest = false;
  showTime = true;
  showTitle = true;
  submenuSetDensity = false;
  submenuSetMohs = false;
  submenuSetWeight = false;
  testClearT = false;
  testDone = false;
  testError = false;
  testOk = false;
  testShowT = false;
  timeSetClock = false;
  timeUpdate = false;

  Config_loadSetup();
}


//******************************************************************************
//
//  Function: Config_loadSetup()
//
//  Description:
//  ============
//  This function loads submenu settings from EEPROM.
//
//******************************************************************************
void Config_loadSetup(void)
{
  eepromMemPtr = EEPROM_POWER;
  submenuPower = Peripheral_readEEPROM();

  eepromMemPtr = EEPROM_DENSITY;
  submenuDensity = Peripheral_readEEPROM();

  eepromMemPtr = EEPROM_WEIGHT;
  submenuWeight = Peripheral_readEEPROM();

  eepromMemPtr = EEPROM_MOHS;
  submenuMohs = Peripheral_readEEPROM();

  eepromMemPtr = EEPROM_UNITS;
  submenuUnits = Peripheral_readEEPROM();
  if ((submenuUnits < SUBMENU_UNITS_MPA) || (submenuUnits > SUBMENU_UNITS_PSI))
  {
    submenuUnits = SUBMENU_UNITS_PSI;
    Peripheral_writeEEPROM(submenuUnits);
  }

  eepromMemPtr = EEPROM_AGG_SIZE;
  submenuAggSize = Peripheral_readEEPROM();

  eepromMemPtr = EEPROM_ZERO;
  adcZero = Peripheral_readEEPROM();

  eepromMemPtr = EEPROM_FULL_SCALE;
  adcFullScale = Peripheral_readEEPROM();

  eepromMemPtr = EEPROM_TESTS;
  testSetCount = Peripheral_readEEPROM();
  
  Peripheral_scaleADC();
}


//******************************************************************************
//
//  Function: Config_saveSetup()
//
//  Description:
//  ============
//  This function saves submenu settings to EEPROM.
//
//******************************************************************************
void Config_saveSetup(void)
{
  eepromMemPtr = EEPROM_POWER;
  Peripheral_writeEEPROM(submenuPower);

  eepromMemPtr = EEPROM_DENSITY;
  Peripheral_writeEEPROM(submenuDensity);

  eepromMemPtr = EEPROM_WEIGHT;
  Peripheral_writeEEPROM(submenuWeight);

  eepromMemPtr = EEPROM_MOHS;
  Peripheral_writeEEPROM(submenuMohs);

  eepromMemPtr = EEPROM_UNITS;
  Peripheral_writeEEPROM(submenuUnits);

  eepromMemPtr = EEPROM_AGG_SIZE;
  Peripheral_writeEEPROM(submenuAggSize);
}


//******************************************************************************
//
//  Function: Config_setSettings()
//
//  Description:
//  ============
//  This function handles the Set Settings menu.  When variables are set for
//  the next menu following a press of the enter key, keyCount must be saved
//  to the appropriate submenu* variable prior to doing anything else.
//
//******************************************************************************
void Config_setSettings(void)
{
  if (menuInitSubmenu)
  {
    keyCount = submenuPower;
    keyMax = SUBMENU_POWER_HIGH;
    keyMin = SUBMENU_POWER_STD;
    keySet = false;
    menuInitSubmenu = false;
    menuShowSubmenu = true;
    submenuSetPower = true;
  }

  if (keySet)
  {
    keySet = false;
    if (submenuSetPower)     
    {
      submenuPower = keyCount;                    // This line must be first.
      submenuSetPower = false;
      if (submenuPower < SUBMENU_POWER_HIGH)
      {
        keyCount = submenuDensity;
        keyMax = SUBMENU_DENSITY_LIGHT;
        keyMin = SUBMENU_DENSITY_STD;
        submenuSetDensity = true;
      }
      else
      {
        keyCount = submenuUnits;
        keyMax = SUBMENU_UNITS_PSI;
        keyMin = SUBMENU_UNITS_MPA;
        submenuSetUnits = true;
      }
    }
    else if (submenuSetDensity)
    {
      submenuDensity = keyCount;                  // This line must be first.
      submenuSetDensity = false;
      if (submenuDensity == SUBMENU_DENSITY_STD)
      {
        keyCount = submenuMohs;
        keyMax = SUBMENU_MOH_7;
        keyMin = SUBMENU_MOH_3;
        submenuSetMohs = true;
      }
      else
      {
        keyCount = submenuWeight;
        keyMax = SUBMENU_WEIGHT_LOW;
        keyMin = SUBMENU_WEIGHT_HIGH;
        submenuSetWeight = true;
      }
    }
    else if (submenuSetWeight)
    {
      submenuWeight = keyCount;                   // This line must be first.
      keyCount = submenuUnits;
      keyMax = SUBMENU_UNITS_PSI;
      keyMin = SUBMENU_UNITS_MPA;
      submenuMohs = SUBMENU_MOH_3;
      submenuSetUnits = true;
      submenuSetWeight = false;
    }
    else if (submenuSetMohs)
    {
      submenuMohs = keyCount;                     // This line must be first.
      keyCount = submenuUnits;
      keyMax = SUBMENU_UNITS_PSI;
      keyMin = SUBMENU_UNITS_MPA;
      submenuSetMohs = false;
      submenuSetUnits = true;
      submenuWeight = SUBMENU_WEIGHT_HIGH;
    }
    else if (submenuSetUnits)
    {
      submenuUnits = keycount;                    // This line must be first.
      keyCount = submenuAggSize;
      keyMax = SUBMENU_AGG_SIZE_LARGE;
      keyMin = SUBMENU_AGG_SIZE_MED;
      submenuSetAggSize = true;
      submenuSetUnits = false;
    }
    else if (submenuSetAggSize)
    {
      submenuAggSize = keyCount;                  // This line must be first.
      keyClear = true;
      submenuSetAggSize = false;
      if (submenuAggSize == SUBMENU_AGG_SIZE_MED)
      {
        submenuMohs = SUBMENU_MOH_3;
      }
      Config_saveSetup();
    }
  }

  LCD_clearDisplay();
  if (submenuSetPower)
  {
    strcpy(lcdData, "Set Power");
    keyCount = keycount>SUBMENU_POWER_HIGH?SUBMENU_POWER_STD:keyCount;
    keyCount = keycount<SUBMENU_POWER_STD?SUBMENU_POWER_STD:keyCount;//keycount comes in here out of bounds displays set power twice?
  }
  else if (submenuSetDensity)
  {
    strcpy(lcdData, "Set Density");
    keyCount = keycount>SUBMENU_DENSITY_LIGHT?SUBMENU_DENSITY_STD:keyCount;
    keyCount = keycount<SUBMENU_DENSITY_STD?SUBMENU_DENSITY_STD:keyCount;    
  }
  else if (submenuSetWeight)
  {
    strcpy(lcdData, "Set Weight");
    keyCount = keycount>SUBMENU_WEIGHT_SUPER_LOW?SUBMENU_WEIGHT_MED:keyCount;
    keyCount = keycount<SUBMENU_WEIGHT_HIGH?SUBMENU_WEIGHT_MED:keyCount;
  }
  else if (submenuSetMohs)
  {
    strcpy(lcdData, "Set Mohs");
    keyCount = keycount>SUBMENU_MOH_7?SUBMENU_MOH_4:keyCount;
    keyCount = keycount<SUBMENU_MOH_3?SUBMENU_MOH_4:keyCount;
  }
  else if (submenuSetUnits)
  {
    strcpy(lcdData, "Set Units");
    keyCount = keycount<SUBMENU_UNITS_MPA?SUBMENU_UNITS_MPA:keyCount;
    keyCount = keycount>SUBMENU_UNITS_PSI?SUBMENU_UNITS_PSI:keyCount;
  }
  else
  {
    strcpy(lcdData, "Set Aggr Size");
    keyCount = keycount>SUBMENU_AGG_SIZE_LARGE?SUBMENU_AGG_SIZE_SMALL:keyCount;
    keyCount = keycount<SUBMENU_AGG_SIZE_MED?SUBMENU_AGG_SIZE_SMALL:keyCount;
  }
  LCD_setCursorPosition(1, 1);
  LCD_updateDisplay();
  Display_showSubmenuSetSettings(keyCount);
  LCD_setCursorPosition(2, 1);
  LCD_updateDisplay();
}


//******************************************************************************
//  Display Functions
//******************************************************************************

//******************************************************************************
//
//  Function: Display_checkTestData()
//
//  Description:
//  ============
//  This function checks the test data and reports an error when appropriate.
//
//******************************************************************************
void Display_checkTestData(void)
{
  byte              error_code;
  byte              error_count;
  long              limit;
  byte              temp;

  if (submenuAggSize == SUBMENU_AGG_SIZE_MED)
  {
    limit = AGG_SIZE_LIMIT_1_MPA / adcScale;
  }
  else if (submenuAggSize == SUBMENU_AGG_SIZE_SMALL)
  {
    limit = AGG_SIZE_LIMIT_2_MPA / adcScale;
  }
  else
  {
    limit = AGG_SIZE_LIMIT_3_MPA / adcScale;
  }

  error_code = 0;
  error_count = 0;
  temp = 0;
  if (adcData[0] > adcData[1])
  {
    temp = adcData[0] - adcData[1];
  }
  if (temp > limit)
  {
    error_code = 1;
    ++error_count;
  }

  temp = 0;
  if (adcData[0] > adcData[2])
  {
    temp = adcData[0] - adcData[2];
  }
  if (temp > limit)
  {
    error_code += 10;
    ++error_count;
  }

  temp = 0;
  if (adcData[1] > adcData[0])
  {
    temp = adcData[1] - adcData[0];
  }
  if (temp > limit)
  {
    error_code += 3;
    ++error_count;
  }

  temp = 0;
  if (adcData[1] > adcData[2])
  {
    temp = adcData[1] - adcData[2];
  }
  if (temp > limit)
  {
    error_code += 30;
    ++error_count;
  }

  temp = 0;
  if (adcData[2] > adcData[0])
  {
    temp = adcData[2] - adcData[0];
  }
  if (temp > limit)
  {
    error_code += 5;
    ++error_count;
  }

  temp = 0;
  if (adcData[2] > adcData[1])
  {
    temp = adcData[2] - adcData[1];
  }
  if (temp > limit)
  {
    error_code += 50;
    ++error_count;
  }

  if (error_count)
  {
    LCD_clearDisplay();
    strcpy(lcdData, "Error - Repeat");
    LCD_setCursorPosition(1, 1);
    LCD_updateDisplay();
    if (error_count > 2 || testError)
    {
      strcpy(lcdData, "Entire Test");
      testDone = false;
      dataTestNumber = 0;
      testError = false;
    }
    else
    {
      if (error_code == 8 || error_code == 11)
      {
        strcpy(lcdData, "Test No:1");
        dataTestNumber = 0;
      }
      if (error_code == 33 || error_code == 51)
      {
        strcpy(lcdData, "Test No:2");
        dataTestNumber = 1;
      }
      if (error_code == 40 || error_code == 55)
      {
        strcpy(lcdData, "Test No:3");
        dataTestNumber = 2;
      }
      testError = true;
    }
    LCD_setCursorPosition(2, 1);
    LCD_updateDisplay();
    delay_ms(2000);
  }
  else
  {
    testOk = true;
  }
}


//******************************************************************************
//
//  Function: Display_doCalculation()
//
//  Description:
//  ============
//  
//  exp(x)
//******************************************************************************
/*#separate*/ int32 Display_doCalculation(float x)
{
  float             x_temp;
  float             y_temp;
  int32              y;

  x_temp = (x  * x) / 2;
  y_temp = 1 + x + x_temp;
  x_temp = (x_temp * x) / 3;
  y_temp += x_temp;
  x_temp = (x_temp * x) / 4;
  y_temp += x_temp;
  x_temp = (x_temp * x) / 5;
  y_temp += x_temp;
  x_temp = (x_temp * x) / 6;
  y_temp += x_temp;
  y = (int32) y_temp;
  return (y_temp);
}


//******************************************************************************
//
//  Function: Display_showData()
//
//  Description:
//  ============
//  This function copies the appropriate data to the display.
//
//******************************************************************************
void Display_showData(void)
{
  Display_showDistance();
  if (testClearT || testShowT)
  {
    if (testShowT)
    {
      lcdData[++lcdPosition] = 'T';
      ++lcdPosition;
      Display_showDecimal(keyCount);
    }
    if (testClearT)
    {
      lcdData[++lcdPosition] = ' ';
      lcdData[++lcdPosition] = ' ';
      lcdData[++lcdPosition] = ' ';
    }
    if (!testOk)
    {
      lcdData[++lcdPosition] = ' ';
      lcdData[++lcdPosition] = 'N';
      lcdData[++lcdPosition] = 'o';
      lcdData[++lcdPosition] = ':';
      lcdData[++lcdPosition] = '1' + dataTestNumber;
    }
    else
    {
      lcdData[++lcdPosition] = ' ';
      lcdData[++lcdPosition] = 'A';
      lcdData[++lcdPosition] = 'v';
      lcdData[++lcdPosition] = 'e';
    }
  }
  lcdData[++lcdPosition] = 0;
  LCD_setCursorPosition(1, 1);
  LCD_updateDisplay();
  lcdData[lcdPosition] = 0;
  if (testOk)
  {
    distance = DISTANCE_OFFSET_METRIC + ((adcReading - adcZero) * adcScale);    // For unknown reasons, the value of distance
                                                                                // is overwritten between line 580 and here.
  }
  Display_showPressure();

  if (submenuPower == SUBMENU_POWER_HIGH)
  {
    lcdData[++lcdPosition] = 'H';
    lcdData[++lcdPosition] = 'P';
    lcdData[++lcdPosition] = ' ';
  }
  else
  {
    if (submenuPower == SUBMENU_POWER_STD)
    {
      lcdData[++lcdPosition] = 'S';
    }
    else
    {
      lcdData[++lcdPosition] = 'L';
    }

    if (submenuDensity == SUBMENU_DENSITY_STD)
    {
      lcdData[++lcdPosition] = 's';
      lcdData[++lcdPosition] =(submenuMohs - 3) + '0';
    }
    else
    {
      lcdData[++lcdPosition] = 'l';
      if (submenuWeight == SUBMENU_WEIGHT_HIGH)
      {
        lcdData[++lcdPosition] = 'h';
      }
      if (submenuWeight == SUBMENU_WEIGHT_MED)
      {
        lcdData[++lcdPosition] = 'm';
      }
      if (submenuWeight == SUBMENU_WEIGHT_LOW)
      {
        lcdData[++lcdPosition] = 'l';
      }
    }
  }
  
  if (submenuAggSize == SUBMENU_AGG_SIZE_MED)
  {
    lcdData[++lcdPosition] = 'M';
  }
  else if (submenuAggSize == SUBMENU_AGG_SIZE_SMALL)
  {
    lcdData[++lcdPosition] = 'S';
  }
  else if (submenuAggSize == SUBMENU_AGG_SIZE_LARGE)
  {
    lcdData[++lcdPosition] = 'L';
  }

  lcdData[++lcdPosition] = 0;
  LCD_setCursorPosition(2, 1);
  LCD_updateDisplay();
}

//******************************************************************************
//
//  Function: Display_showDecimal()
//
//  Description:
//  ============
//  This function copies the passed data to the display.
//
//******************************************************************************
void Display_showDecimal(int data)
{
  byte              temp;

  temp = 0;
  while (data > 9)
  {
    data -= 10;
    temp++;
  }
  lcdData[lcdPosition++] = temp + '0';
  lcdData[lcdPosition  ] = data + '0';
}

//******************************************************************************
//
//  Function: Display_showDistance()
//
//  Description:
//  ============
//  This function displays the distance data.
//
//******************************************************************************
void Display_showDistance(void)
{
  byte              digit;
  long              divisor;
  byte              i;
  long              temp;

  distance = DISTANCE_OFFSET_METRIC + ((adcReading - adcZero) * adcScale);
  if (submenuUnits == SUBMENU_UNITS_MPA)
  {
    // Show metric units
    strcpy(lcdData, "mm:");
    temp = distance * 10;
  }
  else
  {
    // Show imperial units
    strcpy(lcdData, "in:");
    temp = distance * DISTANCE_CONV_FACTOR;
  }
  lcdPosition = 3;
  divisor = 10000;
  for (i = 0 ; i  < 3 ; i++)
  {
    digit = temp / divisor;
    temp -= digit * divisor;
    divisor /= 10;
    if (((submenuUnits == SUBMENU_UNITS_MPA) && (i == 2)) ||
        ((submenuUnits == SUBMENU_UNITS_PSI) && (i == 1)))
    {
      lcdData[lcdPosition++] = '.';
    }
    lcdData[lcdPosition] = digit + '0';
    ++lcdPosition;
  }
  lcdData[lcdPosition] = ' ';
}


//******************************************************************************
//
//  Function: Display_showMenuDownloadTests()
//
//  Description:
//  ============
//  This function uploads the data to the PC.
//
//******************************************************************************
void Display_showMenuDownloadTests()
{

   if (menuInitSubmenu)
   {
      keySet = false;
      menuInitSubmenu = false;
      menuShowSubmenu = true;
      strcpy(lcdData, "Connect PC");
      LCD_setCursorPosition(2, 1);
      LCD_updateDisplay();
   }

   if (keySet && !keyEnterEscape)
   {
      keySet = false;
      if (dataClear)
      {
         testSetCount = 0;
         eepromMemPtr = EEPROM_TESTS;
         Peripheral_writeEEPROM(testSetCount);
         keyClear = true;
      }
      else
      {
      putc(testSetCount + 48);
  //eepromMemPtr = testSetCount * TEST_SET_SIZE;  // Since this line that doesn't work, it has been replaced with a loop.
      int16  totalmemory = 0;
      for (int16 i = 0; i < testSetCount ; i++)
      {
         totalmemory += TEST_SET_SIZE;
      }      
      
      for (eepromMemPtr = 0 ; eepromMemPtr < totalmemory/*testSetCount * TEST_SET_SIZE*/ ; ++eepromMemPtr)
      {
        putc(Peripheral_readEEPROM() + 48);
      }
      LCD_clearDisplay();
      strcpy(lcdData, "Clear Tests?");
      LCD_setCursorPosition(1, 1);
      LCD_updateDisplay();
      strcpy(lcdData, "Enter-YES ESC-NO");
      LCD_setCursorPosition(2, 1);
      LCD_updateDisplay();
      keyEnterEscape = true;
      }
   }

   if (keyEnterEscape && keySet)
   {
    dataClear = true;
    keyClear = true;
    testSetCount = 0;
    eepromMemPtr = EEPROM_TESTS;
    Peripheral_writeEEPROM(testSetCount);
    keyEnterEscape = false;
   }
   else if (keyEnterEscape && keyClear)
   {      
    keyEnterEscape = false;
   }
}


//******************************************************************************
//
//  Function: Display_showMenuEnterSetup()
//
//  Description:
//  ============
//  This function displays the setup menu.
//
//******************************************************************************
void Display_showMenuEnterSetup(void)
{
  if (menuInitSubmenu)
  {
    keyCount = 1;
    keyMax = 4;
    keyMin = 1;
    keySet = false;
    menuInitSubmenu = false;
    menuShowSubmenu = true;
  }

  if (keySet)
  {
    keySet = false;
    menuInitSubmenu = true;
    switch(keyCount){      
    case SUBMENU_SET_SHOW:
      menuLocationNum = MENU_SHOW_SETTINGS;
      break;
    case SUBMENU_SET_SET:
      menuLocationNum = MENU_SET_SETTINGS;
      break;
    case SUBMENU_SET_CLOCK:
      menuLocationNum = MENU_SET_CLOCK;
      break;
    case SUBMENU_SET_CALIBRATE:
      menuLocationNum = MENU_CALIBRATE;
      break;
    }
  }
  
  LCD_clearDisplay();
      switch(keyCount){      
    case SUBMENU_SET_SHOW:
      strcpy(lcdData, "Show Settings");
      break;
    case SUBMENU_SET_SET:
      strcpy(lcdData, "Set Settings");
      break;
    case SUBMENU_SET_CLOCK:
      strcpy(lcdData, "Set Clock");
      break;
    case SUBMENU_SET_CALIBRATE:
      strcpy(lcdData, "Calibrate");
      break;
    }

  LCD_setCursorPosition(1, 1);
  LCD_updateDisplay();
}


//******************************************************************************
//
//  Function: Display_showMenuMeasure()
//
//  Description:
//  ============
//  This function performs a measurement.
//
//******************************************************************************
void Display_showMenuMeasure(void)
{
  if (menuInitSubmenu)
  {
    menuInitSubmenu = false;
    menuShowSubmenu = true;
    LCD_clearDisplay();
  }
  Peripheral_getADC();
  Display_showData();
  keyNewDetection = true;
}


//******************************************************************************
//
//  Function: Display_showMenuRunTest()
//
//  Description:
//  ============
//  This function performs a test.
//
//******************************************************************************
void Display_showMenuRunTest(void)
{
  byte              i;
  static long       total;

  if (menuInitSubmenu)
  {
    if (testSetCount == TEST_MAX_SETS)
    {
      strcpy(lcdData, "Memory Full");
      LCD_setCursorPosition(2, 1);
      LCD_updateDisplay();
      delay_ms(2000);
      keyClear = true;
    }
    else
    {
      dataTestNumber = 0;
      keySet = false;
      menuInitSubmenu = false;
      menuShowSubmenu = true;
      testClearT = true;
      testDone = false;
      testError = false;
      testOk = false;
    }
  }

  if (keySet)
  {
    keySet = false;
    LCD_clearDisplay();

    if (testOk)
    {
      // Update the time and store the data set
      Peripheral_readRTC();
      Peripheral_saveData();
      keyClear = true;
    }

    if (!testDone)
    {
      ++dataTestNumber;
      if (dataTestNumber == 3)
      {
        testDone = true;
      }
    }

    if (testDone)
    {
      Display_checkTestData();
    }
  }

  if (!testOk)
  {
    Peripheral_getADC();
    adcData[dataTestNumber] = adcReading;
  }
  else
  {
    total = 0;
    for (i = 0 ; i < 3 ; i++)
    {
      total += adcData[i];
    }
    adcReading = total / 3;
  }
  Display_showData();
  keyNewDetection = true;
}


//******************************************************************************
//
//  Function: Display_showMenuShowTests()
//
//  Description:
//  ============
//  This function executes the show test menu item.
//
//******************************************************************************
void Display_showMenuShowTests(void)
{
  static long       total;

  if (menuInitSubmenu)
  {
    testShowT = true;
    keyMax = testSetCount;
    keyMin = 1;
    keySet = false;
    menuInitSubmenu = false;
    menuShowSubmenu = true;

    LCD_clearDisplay();
    strcpy(lcdData, "Test No.");
    LCD_setCursorPosition(1, 1);
    LCD_updateDisplay();

    dataTestNumber = 0;
    keyCount = 1;
    total = 0;
  }

  if (keySet)
  {
    keySet = false;
    showTest = true;
    if (dataTestNumber < 3)
    {
      adcReading = adcData[dataTestNumber];
      total += adcReading;
    }
    if (dataTestNumber == 3)
    {
      adcReading = total / 3;
      testOk = true;
    }
    if (dataTestNumber < 4)
    {
      LCD_clearDisplay();
      Display_showData();
    }
    else
    {
      keyClear = true;
    }
    ++dataTestNumber;
  }
  else if (!showTest)
  {
    lcdPosition = 0;
    Display_showDecimal(keyCount);
    lcdData[++lcdPosition] = 0;
    LCD_setCursorPosition(1, 9);
    LCD_updateDisplay();
    eepromMemPtr = (keyCount - 1) * TEST_SET_SIZE;
    timeRTCData[1] = Peripheral_readEEPROM();     // EEPROM Data Offset  0
    ++eepromMemPtr;
    timeRTCData[2] = Peripheral_readEEPROM();     // EEPROM Data Offset  1
    ++eepromMemPtr;
    timeRTCData[4] = Peripheral_readEEPROM();     // EEPROM Data Offset  2
    ++eepromMemPtr;
    timeRTCData[5] = Peripheral_readEEPROM();     // EEPROM Data Offset  3
    ++eepromMemPtr;
    timeRTCData[6] = Peripheral_readEEPROM();     // EEPROM Data Offset  4
    ++eepromMemPtr;
    submenuPower = Peripheral_readEEPROM();       // EEPROM Data Offset  5
    ++eepromMemPtr;
    submenuDensity = Peripheral_readEEPROM();     // EEPROM Data Offset  6
    ++eepromMemPtr;
    submenuWeight = Peripheral_readEEPROM();      // EEPROM Data Offset  7
    ++eepromMemPtr;
    submenuMohs = Peripheral_readEEPROM();        // EEPROM Data Offset  8
    ++eepromMemPtr;
    submenuUnits = Peripheral_readEEPROM();       // EEPROM Data Offset  9
    ++eepromMemPtr;
    submenuAggSize = Peripheral_readEEPROM();     // EEPROM Data Offset 10
    ++eepromMemPtr;
    adcZero = Peripheral_readEEPROM();            // EEPROM Data Offset 11
    ++eepromMemPtr;
    adcFullScale = Peripheral_readEEPROM();       // EEPROM Data Offset 12
    ++eepromMemPtr;
    adcData[0] = Peripheral_readEEPROM();         // EEPROM Data Offset 13
    ++eepromMemPtr;
    adcData[1] = Peripheral_readEEPROM();         // EEPROM Data Offset 14
    ++eepromMemPtr;
    adcData[2] = Peripheral_readEEPROM();         // EEPROM Data Offset 15
    Display_showTime();
    LCD_setCursorPosition(2, 1);
    LCD_updateDisplay();
    Peripheral_scaleADC();
  }
}


//******************************************************************************
//
//  Function: Display_showPressure()
//
//  Description:
//  ============
//  This function displays the pressure data as MPa or PSI.  Do all the
//  calculations in MPa and then convert to PSI if necessary.
//
//******************************************************************************
void Display_showPressure(void)
{
  int16             b;
  int16             m;
  int16             min_dist;
  int32             y;
  float           fltdistance;
  float           flty;
  int32           td;

  if (submenuPower == SUBMENU_POWER_HIGH)
  {
     fltdistance = (float) distance;
      fltdistance /= 100.0;
      flty = 28.0 * Display_doCalculation( fltdistance * 0.0602 );
      y = (int32)flty; 
      min_dist = 0;
  }
  else
  {
    if (submenuPower == SUBMENU_POWER_STD)
    {
      switch (submenuMohs){
      case SUBMENU_MOH_3:
         b = OFFSET_SMALL_3_MPA;
         m = SLOPE_SMALL_3_MPA;
         min_dist = 2800;
         break;
      case SUBMENU_MOH_4:
         b = OFFSET_SMALL_4_MPA;
         m = SLOPE_SMALL_4_MPA;
         min_dist = 2900;
         break;
      case SUBMENU_MOH_5:
         b = OFFSET_SMALL_5_MPA;
         m = SLOPE_SMALL_5_MPA;
         min_dist = 3300;         
         break;
      case SUBMENU_MOH_6:
         b = OFFSET_SMALL_6_MPA;
         m = SLOPE_SMALL_6_MPA;
         min_dist = 3600;         
         break;
      case SUBMENU_MOH_7:
         b = OFFSET_SMALL_7_MPA;
         m = SLOPE_SMALL_7_MPA;
         min_dist = 3900;         
         break;         
      }
    }
    if (submenuPower == SUBMENU_POWER_LOW)
    {
      switch (submenuMohs){
      case SUBMENU_MOH_3:
         b = OFFSET_LARGE_3_MPA;
         m = SLOPE_LARGE_3_MPA;
         min_dist = 2800;
         break;
      case SUBMENU_MOH_4:
         b = OFFSET_LARGE_4_MPA;
         m = SLOPE_LARGE_4_MPA;
         min_dist = 2900;         
         break;
      case SUBMENU_MOH_5:
         b = OFFSET_LARGE_5_MPA;
         m = SLOPE_LARGE_5_MPA;
         min_dist = 3300;
         break;
      case SUBMENU_MOH_6:
         b = OFFSET_LARGE_6_MPA;
         m = SLOPE_LARGE_6_MPA;
         min_dist = 3600;         
         break;
      case SUBMENU_MOH_7:
         b = OFFSET_LARGE_7_MPA;
         m = SLOPE_LARGE_7_MPA;
         min_dist = 3900;
         break;
      }
    }
    y = (m*distance/1000) - b; //scaled m up by 100
  }

  // Scale for low and super-low weights
  if (submenuWeight == SUBMENU_WEIGHT_LOW)
  {
    y = y * 84 / 100;
  }
  else if (submenuWeight == SUBMENU_WEIGHT_SUPER_LOW)
  {
    y = y * 66 / 100;
  }
  if (distance < min_dist)
  {
    y = 0;
  }

  Display_updateDisplayPressure(y);
}


//******************************************************************************
//
//  Function: Display_showSubmenuCalibrate()
//
//  Description:
//  ============
//  This function handles the calibration procedure.
//
//******************************************************************************
void Display_showSubmenuCalibrate(void)
{
  static short int  calSetZero = false;
  static byte       temp;

  if (menuInitSubmenu)
  {
    calStartCal = true;
    keySet = false;
    menuInitSubmenu = false;
  }

  if (keySet)
  {
    keySet = false;
    if (calStartCal)
    {
      calSetZero = true;
      calStartCal = false;
    }
    else if (calSetZero)
    {
      calSetZero = false;
      Peripheral_getADC();
      temp = adcReading;
    }
    else
    {
      adcZero = temp;
      Peripheral_getADC();
      adcFullScale = adcReading;
      eepromMemPtr = EEPROM_ZERO;
      Peripheral_writeEEPROM(adcZero);
      eepromMemPtr = EEPROM_FULL_SCALE;
      Peripheral_writeEEPROM(adcFullScale);
      keyClear = true;
    }
  }
  LCD_clearDisplay();

  if (calStartCal)
  {
    strcpy(lcdData, "Calibrate");
  }
  else if (calSetZero)
  {
    strcpy(lcdData, "Zero Press Enter");
  }
  else
  {
    strcpy(lcdData, "Max Press Enter");
  }
  LCD_setCursorPosition(1, 1);
  LCD_updateDisplay();

  if (calStartCal)
  {
    strcpy(lcdData, "Enter-YES ESC-NO");
    LCD_setCursorPosition(2, 1);
    LCD_updateDisplay();
  }
}

//******************************************************************************
//
//  Function: Display_showSubmenuSetClock()
//
//  Description:
//  ============
//  This function sets the clock.
//
//******************************************************************************
void Display_showSubmenuSetClock(void)
{
  byte              countOnes;
  byte              countTens;

  if (menuInitSubmenu)
  {
    keyCountNew = false;
    keySet = false;
    menuInitSubmenu = false;
    menuShowSubmenu = true;
    timeSetClock = true;
    Peripheral_readRTC();
    Display_showTime();
    keyMax = 12;
    keyMin = 1;
    lcdPosition = 1;
    LCD_setCursorPosition(2, 1);
    LCD_updateDisplay();
    LCD_turnOnCursor();
  }

  if (keySet)
  {
    keySet = false;
    lcdPosition++;
    if (lcdPosition > 14)
    {
      keyClear = true;
    }
    if (lcdPosition == 2 || lcdPosition == 5 || lcdPosition == 8 || lcdPosition == 11)
    {
      lcdPosition = lcdPosition + 2;
    }
  }

  if (keyCountNew)
  {
    keyCountNew = false;
    if (lcdPosition == 14)
    {
      // Update the A/P character
      if (keyCount == 2)
      {
        lcdData[14] = 'P';
      }
      else
      {
        lcdData[14] = 'A';
      }
    }
    else
    {
      --lcdPosition;
      Display_showDecimal(keyCount);
    }
    Peripheral_setRTC();
    Peripheral_readRTC();
    LCD_setCursorPosition(2, 1);
    LCD_updateDisplay();
  }

  if (lcdPosition == 7 || lcdPosition == 13)
  {
    keyMin = 0;
  }
  else
  {
    keyMin = 1;
  }

  if (lcdPosition == 1 || lcdPosition == 10)
  {
    keyMin = 1;
    keyMax = 12;
  }

  if (lcdPosition == 4)
  {
    keyMax = 31;
  }

  if (lcdPosition == 7)
  {
    keyMax = 99;
  }

  if (lcdPosition == 13)
  {
    keyMin = 0;
    keyMax = 59;
  }

  if (lcdPosition == 14)
  {
    keyMax = 2;
    if (lcdData[14] == 'P')
    {
      keyCount = 2;
    }
    else
    {
      keyCount = 1;
    }
  }
  else
  {
    countTens = 10 * (lcdData[lcdPosition - 1] - '0');
    countOnes = lcdData[lcdPosition] - '0';
    keyCount = countTens + countOnes;
  }
  LCD_setCursorPosition(2, lcdPosition + 1);
}

//******************************************************************************
//
//  Function: Display_showSubmenuSetSettings()
//
//  Description:
//  ============
//  This function copies the appropriate text to the display based on the
//   passed data.
//
//******************************************************************************
void Display_showSubmenuSetSettings(byte data)
{
   switch(data){
   case SUBMENU_POWER_STD:
      strcpy(lcdData, "Std Power");
      break;
   case SUBMENU_POWER_LOW:
      strcpy(lcdData, "Low Power");
      break;
   case SUBMENU_POWER_HIGH:
      strcpy(lcdData, "High Perf");
      break;
   case SUBMENU_DENSITY_STD:
      strcpy(lcdData, "Std Wght ");
      break;
   case SUBMENU_DENSITY_LIGHT:
      strcpy(lcdData, "Lgt Wght ");
      break;      
   case SUBMENU_MOH_3:
      strcpy(lcdData, "MOH#3 ");
      break;
   case SUBMENU_MOH_4:
      strcpy(lcdData, "MOH#4 ");
      break;      
   case SUBMENU_MOH_5:
      strcpy(lcdData, "MOH#5 ");
      break;      
   case SUBMENU_MOH_6:
      strcpy(lcdData, "MOH#6 ");
      break;      
   case SUBMENU_MOH_7:
      strcpy(lcdData, "MOH#7 ");
      break;      
   case SUBMENU_UNITS_MPA:
      strcpy(lcdData, "MPa");
      break;
   case SUBMENU_UNITS_PSI:
      strcpy(lcdData, "PSI");
      break;
   case SUBMENU_AGG_SIZE_MED:
      strcpy(lcdData, "Mrtr-M");
      break;
   }

   if (submenuUnits == SUBMENU_UNITS_MPA)
  {
      switch(data){
      case SUBMENU_AGG_SIZE_SMALL:
         strcpy(lcdData, "25mm-S");
         break;
      case SUBMENU_AGG_SIZE_LARGE:
         strcpy(lcdData, "50mm-L");
         break;
      case SUBMENU_WEIGHT_HIGH:
         strcpy(lcdData, ">.83-h ");
         break;         
      case SUBMENU_WEIGHT_MED:
         strcpy(lcdData, ".83-79m");
         break;         
      case SUBMENU_WEIGHT_LOW:
         strcpy(lcdData, "<.79-l ");
         break;                  
      }
  }
  else
  {
      switch(data){
      case SUBMENU_AGG_SIZE_SMALL:
         strcpy(lcdData, "1in.-S");
         break;
      case SUBMENU_AGG_SIZE_LARGE:
         strcpy(lcdData, "2in.-L");
         break;
      case SUBMENU_WEIGHT_HIGH:
         strcpy(lcdData, ">120-h ");
         break;         
      case SUBMENU_WEIGHT_MED:
         strcpy(lcdData, "115-20m");
         break;         
      case SUBMENU_WEIGHT_LOW:
         strcpy(lcdData, "<115-l ");
         break;                  
      }
  }
}


//******************************************************************************
//
//  Function: Display_showSubmenuShowSettings()
//
//  Description:
//  ============
//  This function sends the Show Settings submenu data to the display.
//
//******************************************************************************
void Display_showSubmenuShowSettings(void)
{
  if (menuInitSubmenu)
  {
    menuInitSubmenu = false;
    menuShowSubmenu = true;
    keySet = false;
    LCD_clearDisplay();
  }
  if (keySet)
  {
    keyClear = true;
  }
  Display_showSubmenuSetSettings(submenuPower);
  LCD_setCursorPosition(1, 1);
  LCD_updateDisplay();
  Display_showSubmenuSetSettings(submenuAggSize);
  LCD_setCursorPosition(1, 11);
  LCD_updateDisplay();
  if (submenuPower != SUBMENU_POWER_HIGH)
  {
    Display_showSubmenuSetSettings(submenuDensity);
    LCD_setCursorPosition(2, 1);
    LCD_updateDisplay();
    if (submenuDensity == SUBMENU_DENSITY_STD)
    {
      // Show MOHs menu for standard density
      Display_showSubmenuSetSettings(submenuMohs);
    }
    else
    {
      // Show weight menu for light density
      Display_showSubmenuSetSettings(submenuWeight);
    }
    LCD_setCursorPosition(2, 10);
    LCD_updateDisplay();
  }
}


//******************************************************************************
//
//  Function: Display_showTime(()
//
//  Description:
//  ============
//  This function copies the time data to the display array.
//
//******************************************************************************
void Display_showTime(void)
{
  byte              temp;

  // Update the minutes
  temp = timeRTCData[1];
  lcdData[13] = '0' + (temp & 0x0f);
  swap(temp);
  lcdData[12] = '0' + (temp & 0x07);

  // Update the hours
  temp = timeRTCData[2];
  lcdData[10] = '0' + (temp & 0x0f);
  swap(temp);
  if (temp & 0x01)
  {
    lcdData[9] = '1';
  }
  else if (timeSetClock)
  {
    lcdData[9] = '0';
  }
  else
  {
    lcdData[9] = ' ';
  }

  // Update the A / P character
  if (temp & 0x02)
  {
    lcdData[14] = 'P';
  }
  else
  {
    lcdData[14] = 'A';
  }

  // Update the day
  temp = timeRTCData[4];
  lcdData[4] = '0' + (temp & 0x0f);
  swap(temp);
  lcdData[3] = '0' + (temp & 0x03);

  // Update the month
  temp = timeRTCData[5];
  lcdData[1] = '0' + (temp & 0x0f);
  swap(temp);
  lcdData[0] = '0' + (temp & 0x03);

  // Update the year
  temp = timeRTCData[6];
  lcdData[7] = '0' + (temp & 0x0f);
  swap(temp);
  lcdData[6] = '0' + (temp & 0x0f);

  // Update the slashes and colons
  lcdData[ 2] = '/';
  lcdData[ 5] = '/';
  lcdData[ 8] = ' ';
  lcdData[11] = ':';
  lcdData[15] = 0;                      // Null terminate string
}


//******************************************************************************
//
//  Function: Display_updateDisplayPressure()
//
//  Description:
//  ============
//  This function copies the pressure data to the display.
//
//******************************************************************************
void Display_updateDisplayPressure(int32 pressure)
{
  byte              digit;
  int32              divisor;
  byte              i;

  if (submenuUnits == SUBMENU_UNITS_MPA)
  {
    strcpy(lcdData, "MPA:");
    lcdPosition = 4;
  }
  else
  {
    strcpy(lcdData, "PSI:");
    lcdPosition = 4;
    pressure = pressure * 145 / 10;
  }
    divisor = 10000;
  for (i = 0 ; i < 5 ; ++i)
  {
      digit = pressure / divisor;
      pressure -= digit * divisor;
      divisor /= 10;
      if ((i == 4) && (submenuUnits == SUBMENU_UNITS_MPA))
      {
        lcdData[lcdPosition++] = '.';
      }
      lcdData[lcdPosition++] = digit + '0';
  }

  lcdData[lcdPosition++] = ' '; 
  lcdData[lcdPosition] = ' '; 
}


//******************************************************************************
//  Keyboard Functions
//******************************************************************************

//******************************************************************************
//
//  Function: Keyboard_getDownKey()
//
//  Description:
//  ============
//  This function decrements the menu location pointer for down key presses.
//
//******************************************************************************
void Keyboard_getDownKey(void)
{
  keyCount--;
  if (keyCount < keyMin)
  {
    keyCount = keyMax;
  }
  keyCountNew = true;
}


//******************************************************************************
//
//  Function: Keyboard_getKeypress()
//
//  Description:
//  ============
//  This function debounces the keypresses.
//
//******************************************************************************
char Keyboard_getKeypress(void)
{
  static char       keypressNew;
  static char       keypressOld;

  keypressNew = Keyboard_getKeyRaw();
  if (keypressNew == 0)
  {
   keypressOld = 0;
   return (0);
  }
  if (keypressNew == keypressOld)
  {
    return (0);
  }
  delay_ms(50);
  if (Keyboard_getKeyRaw() == keypressNew)
  {
    keypressOld = keypressNew;
    keyNewDetection = true;
    return (keypressNew);
  }
  else
  {
    keypressOld = 0;
    return (0);
  }
}


//******************************************************************************
//
//  Function: Keyboard_getKeyRaw()
//
//  Description:
//  ============
//  This function debounces the keypresses.
//
//******************************************************************************
char Keyboard_getKeyRaw(void)
{
  byte              temp = 0;

  output_high(pin_b3);
  if ((kbd_port & ROW1) != 0) //enter key
  {
    temp = 2;
  }
  if ((kbd_port & ROW2) != 0) //arrow key down
  {
    temp = 3;
  }
  output_low(pin_b3);
  output_high(pin_b2); 
  if ((kbd_port & ROW1) != 0) //escape key
  {
    temp = 12;
  }
  if ((kbd_port & ROW2) != 0) //arrow key Up
  {
    temp = 13;
  }
  output_low(pin_b2);
  return (temp);
}


//******************************************************************************
//
//  Function: Keyboard_getUpKey()
//
//  Description:
//  ============
//  This function increments the menu location pointer for up key presses.
//
//******************************************************************************
void Keyboard_getUpKey(void)
{
  keyCount++;
  if (keyCount > keyMax)
  {
    keyCount = keyMin;
  }
  keyCountNew = true;
}


//******************************************************************************
//  LCD Functions
//******************************************************************************

//******************************************************************************
//
//  Function: LCD_clearDisplay()
//
//  Description:
//  ============
//  This function clears the LCD screen.
//
//******************************************************************************
void LCD_clearDisplay(void)
{
  LCD_waitForReadySignal();
  output_low(PIN_A1);                   // Set the RS line low
  output_low(PIN_A2);                   // Set the R/W line low
  output_high(PIN_A3);                  // Set the enable line high
  delay_cycles(1);                      // Wait
  lcd_port = 0x01;                      // Clear display and put the cursor in the home position
  delay_cycles(2);
  output_low(PIN_A3);                   // Set the enable line low
}


//******************************************************************************
//
//  Function: LCD_setCursorPosition()
//
//  Description:
//  ============
//  This function moves the cursor to the passed location.
//
//******************************************************************************
void LCD_setCursorPosition(byte row, byte col)
{
  byte              temp;

  if (row > 1)
  {
    temp = 0x40;                        // Move cursor to line 2
  }
  else
  {
    temp = 0;                           // Move cursor to line 1
  }
  temp += col - 1;
  LCD_waitForReadySignal();
  output_low(PIN_A1);                   // Set the RS line low
  output_low(PIN_A2);                   // Set the R/W line low
  output_high(PIN_A3);                  // Set the enable line high
  delay_cycles(1);                      // Wait
  lcd_port = 0x80 | temp;
  delay_cycles(2);
  output_low(PIN_A3);                   // Set the enable line low
}


//******************************************************************************
//
//  Function: LCD_turnOffCursor()
//
//  Description:
//  ============
//  This function turns off the cursor.
//
//******************************************************************************
void LCD_turnOffCursor(void)
{
  LCD_waitForReadySignal();
  output_low(PIN_A1);                   // Set the RS line low
  output_low(PIN_A2);                   // Set the R/W low
  output_high(PIN_A3);                  // Set the enable line high
  delay_cycles(1);                      // Wait
  lcd_port = 0x0c;                      // Turn off the cursor
  delay_cycles(2);
  output_low(PIN_A3);                   // Set the enable line low
}


//******************************************************************************
//
//  Function: LCD_turnOnCursor()
//
//  Description:
//  ============
//  This function disables cursor blinking.
//
//******************************************************************************
void LCD_turnOnCursor(void)
{
  LCD_waitForReadySignal();
  output_low(PIN_A1);                   // Set the RS line low
  output_low(PIN_A2);                   // Set the R/W line low
  output_high(PIN_A3);                  // Set the enable line high
  delay_cycles(1);                      // Wait
  lcd_port = 0x0e;                      // Turn off cursor blinking
  delay_cycles(2);
  output_low(PIN_A3);                   // Set the enable line low
}


//******************************************************************************
//
//  Function: LCD_updateDisplay()
//
//  Description:
//  ============
//  This function updates the display with the LCD data.
//
//******************************************************************************
void LCD_updateDisplay(void)
{
  byte              temp = 0;

  while (lcdData[temp]!=0)
  {
    LCD_waitForReadySignal();
    output_high(PIN_A1);                // Set the RS line high
    output_low(PIN_A2);                 // Set the R/W line low
    delay_cycles(1);
    output_high(PIN_A3);                // Set the enable line high
    delay_cycles(1);                    // Wait
    lcd_port = lcdData[temp];
    delay_cycles(2);
    output_low(PIN_A3);                 // Set the enable line low
    temp++;
  }
}


//******************************************************************************
//
//  Function: LCD_waitForReadySignal()
//
//  Description:
//  ============
//  This function waits until the display is free.
//
//******************************************************************************
void LCD_waitForReadySignal(void)
{
  byte              temp;

  output_low(PIN_A3);                   // Set the enable line low
  set_tris_d(0xff);                     // Make all PORT D pins as inputs

  do
  {
    output_low(PIN_A1);                 // Set the RS line low
    output_high(PIN_A2);                // Set the R/W line high
    delay_cycles(1);                    // Wait
    output_high(PIN_A3);                // Set the enable line high
    delay_cycles(1);                    // Wait
    temp = lcd_port;                    // Read LCD busy flag
    delay_cycles(2);
    output_low(PIN_A3);                 // Set the enable line low
  } while (bit_test(temp,7));           // Wait until the display is free
  
  output_high(PIN_A3);                  // Set the enable line high
  output_low(PIN_A2);                   // Set the R/W line low
  set_tris_d(0x00);                     // Make all PORT D pins as outputs
}


//******************************************************************************
//  Main Function
//******************************************************************************

//******************************************************************************
//
//  Function: main()
//
//  Description:
//  ============
//  This function contains the calls of the start-up routines and the main()
//  forever loop.
//
//  NOTE: If the RTC isn't set, then all of the EEPROM submenu values will
//        be overwritten when a hard reset occurs.
//
//******************************************************************************
void main(void)
{
  byte              i;
  byte              key;

  set_tris_a(1);                        // Make PORT A pin 1 an input
  set_tris_b(0xf3);                     // Make PORT B 0-3 in 4-7 out
  set_tris_d(0);                        // Make all PORT D pins outputs
  setup_adc(ADC_CLOCK_INTERNAL);
  output_high(PIN_A3);                  // Enable lcd
  output_low(PIN_A2);                   // Set write mode (R/W)
  output_low(PIN_A1);                   // Instruction mode (RS)
  delay_ms(25);                         // Wait for display to power up

  // Initialize the LCD
  for (i = 0 ; i < 3 ; i++)
  {
    lcd_port = 0x30;
    output_low(PIN_A3);
    delay_ms(5);
    output_high(PIN_A3);
  }
  
  LCD_waitForReadySignal();
  lcd_port = 0x38;                      // Set the LCD to 8 bits and 2 lines
  LCD_turnOffCursor();

  Peripheral_readRTC();
  Config_initialize();

  // Main forever loop
  while (true)
  {
    // Check for keypresses
    key = Keyboard_getKeypress();
    if (keyNewDetection)
    {
      keyNewDetection = false;
      //set keyCount keySet(Enter) keyClear(Esc)
      switch(key){
      case DOWN_KEY:
         Keyboard_getDownKey();
         break;
      case UP_KEY:
         Keyboard_getUpKey();
         break;         
      case ENTER_KEY:
         keySet = true;//enter key pressed
         break;         
      case ESC_KEY:
         keyClear = true; //escape key pressed
         break;          
      }    
      //display main menu
      if (keyCountNew && !menuShowSubmenu)
      {
         switch(keyCount){
         case MENU_MEASURE:
           strcpy(lcdData, "Measure");
           break;
         case MENU_RUN_TEST:
            strcpy(lcdData, "Run Test");
            break;
         case MENU_SHOW_TESTS:
           strcpy(lcdData, "Show Tests");
           break;
         case MENU_DOWNLOAD_TESTS:
            strcpy(lcdData, "Download Tests");
            break;
          case MENU_ENTER_SETUP:
            strcpy(lcdData, "Enter Setup");
            break;
         }
         showTime = false;
         LCD_clearDisplay();
         LCD_updateDisplay();
      }

      if (keySet && !menuShowSubmenu)
      {
        menuLocationNum = keyCount;
        menuInitSubmenu = true;
      }

      if (menuInitSubmenu || menuShowSubmenu)
      {
        switch(menuLocationNum){
        case MENU_MEASURE:
          Display_showMenuMeasure();
          break;
        case MENU_RUN_TEST:
          Display_showMenuRunTest();
          break;
        case MENU_SHOW_TESTS:
          if (testSetCount)
          {
            Display_showMenuShowTests();
          }
          else
          {
            keyClear = true;
          }
          break;
        case MENU_DOWNLOAD_TESTS:
          if (testSetCount)
          {
            Display_showMenuDownloadTests();
          }
          else
          {
            keyClear = true;
          }
          break;
        case MENU_ENTER_SETUP:
          Display_showMenuEnterSetup();
          break;
        case MENU_SHOW_SETTINGS:
          Display_showSubmenuShowSettings();
          break;
        case MENU_SET_SETTINGS:
          Config_setSettings();
          break;
        case MENU_SET_CLOCK:
          Display_showSubmenuSetClock();
          break;
        case MENU_CALIBRATE:
          Display_showSubmenuCalibrate();
          break;        
        }
      }

//reset Enter and ESC Buttons
      keyset = keyset == true? false:keyset;

      if (keyClear)
      {
        Config_initialize();
      }
    }

    // Update the title
    if (showTitle)
    {
      showTitle = false;
      LCD_clearDisplay();
      LCD_turnOffCursor();
      strcpy(lcdData, "Stored Tests:");
      lcdPosition = 13;
      Display_showDecimal(testSetCount);
      lcdData[++lcdPosition] = 0;
      LCD_setCursorPosition(2, 1);
      LCD_updateDisplay();
      timeUpdate = true;
    }

    // Update the time
    if (showTime)
    {
      Peripheral_readRTC();
      if (timeUpdate)
      {
        timeUpdate = false;
        Display_showTime();
        LCD_setCursorPosition(1, 1);
        LCD_updateDisplay();
      }
    }
  }
}


//******************************************************************************
//  Peripheral Functions
//******************************************************************************

//******************************************************************************
//
//  Function: Peripheral_getADC()
//
//  Description:
//  ============
//  This function gets the ADC reading.
//
//******************************************************************************
void Peripheral_getADC(void)
{
  setup_port_a(A_ANALOG);
  set_adc_channel(0);
  delay_ms(100);
  adcReading = read_adc();
  setup_port_a(NO_ANALOGS);
}


//******************************************************************************
//
//  Function: Peripheral_readEEPROM()
//
//  Description:
//  ============
//  This function reads from the EEPROM and returns the read byte.
//
//******************************************************************************
byte Peripheral_readEEPROM(void)
{
  byte              temp;

  i2c_start();
  i2c_write(0xA0);
  
  i2c_write(make8(eepromMemPtr,1));
  i2c_write(make8(eepromMemPtr,0));
  i2c_start();
  i2c_write(0xA0|1);
  temp = i2c_read(0);
  i2c_stop();
  return (temp);
}


//******************************************************************************
//
//  Function: Peripheral_readRTC()
//
//  Description:
//  ============
//  This function reads the real time clock.
//
//******************************************************************************
void Peripheral_readRTC(void)
{
  byte              i;
  byte              temp;

  temp = timeRTCData[1];                // Save the RTC minutes to test for display change
  i2c_start();
  i2c_write(0xD0);                      // I2C slave read mode - rtc clock address
  i2c_write(0x00);                      // Point to the start of the registers
  i2c_start();
  i2c_write(0xD1);                      // I2C slave write mode - RTC clock address
  
  for (i = 0 ; i < 6 ; i++)
  {
    timeRTCData[i] = i2c_read();
  }

   timeRTCData[6] = i2c_read(0);         // + NACK
  i2c_stop();

  if (temp != timeRTCData[1] && !menuShowSubmenu)
  {
    timeUpdate = true;                  // Test RTC clock for minutes change
  }
}


//******************************************************************************
//
//  Function: Peripheral_saveData()
//
//  Description:
//  ============
//  This function saves data to the EEPROM.
//
//******************************************************************************
void Peripheral_saveData(void)
{
  int     i;

  //eepromMemPtr = testSetCount * TEST_SET_SIZE;  // Since this line that doesn't work, it has been replaced with a loop.
  eepromMemPtr = 0;
  for (i = 0; i < testSetCount ; i++)
  {
    eepromMemPtr += TEST_SET_SIZE;
  }
  Peripheral_writeEEPROM(timeRTCData[1]);         // EEPROM Data Offset  0 minutes
  ++eepromMemPtr;
  Peripheral_writeEEPROM(timeRTCData[2]);         // EEPROM Data Offset  1 hours
  ++eepromMemPtr;
  Peripheral_writeEEPROM(timeRTCData[4]);         // EEPROM Data Offset  2 day
  ++eepromMemPtr;
  Peripheral_writeEEPROM(timeRTCData[5]);         // EEPROM Data Offset  3
  ++eepromMemPtr;
  Peripheral_writeEEPROM(timeRTCData[6]);         // EEPROM Data Offset  4
  ++eepromMemPtr;
  Peripheral_writeEEPROM(submenuPower);           // EEPROM Data Offset  5
  ++eepromMemPtr;
  Peripheral_writeEEPROM(submenuDensity);         // EEPROM Data Offset  6
  ++eepromMemPtr;
  Peripheral_writeEEPROM(submenuWeight);          // EEPROM Data Offset  7
  ++eepromMemPtr;
  Peripheral_writeEEPROM(submenuMohs);            // EEPROM Data Offset  8
  ++eepromMemPtr;
  Peripheral_writeEEPROM(submenuUnits);           // EEPROM Data Offset  9
  ++eepromMemPtr;
  Peripheral_writeEEPROM(submenuAggSize);         // EEPROM Data Offset 10
  ++eepromMemPtr;
  Peripheral_writeEEPROM(adcZero);                // EEPROM Data Offset 11
  ++eepromMemPtr;
  Peripheral_writeEEPROM(adcFullScale);           // EEPROM Data Offset 12
  ++eepromMemPtr;
  Peripheral_writeEEPROM(adcData[0]);             // EEPROM Data Offset 13
  ++eepromMemPtr;
  Peripheral_writeEEPROM(adcData[1]);             // EEPROM Data Offset 14
   ++eepromMemPtr;
  Peripheral_writeEEPROM(adcData[2]);             // EEPROM Data Offset 15
  ++testSetCount;
  eepromMemPtr = EEPROM_TESTS;
  Peripheral_writeEEPROM(testSetCount);
  dataClear = false;                                // Added for download data after a test after the first time
}


//******************************************************************************
//
//  Function: Peripheral_scaleADC()
//
//  Description:
//  ============
//  This function sets the ADC reading scaling factor for metric units.
//  If imperial readings are desired, then a conversion factor is used prior to
//  displaying the reading.
//
//******************************************************************************
void Peripheral_scaleADC(void)
{
  byte              span;

  span = (adcFullScale - adcZero) - 1;
  adcScale = ADC_SCALE_FACTOR_METRIC / span;
}


//******************************************************************************
//
//  Function: Peripheral_setRTC()
//
//  Description:
//  ============
//  This function sets the real time clock value.
//
//******************************************************************************
void Peripheral_setRTC(void)
{
  byte               i;
  byte               rtc_set[7];        // Data register to set rtc clock
  byte               temp1;
  byte               temp2;
  byte               temp3;

  rtc_set[0] = 0;                       // enable osc - 0 seconds
  temp2 = lcdData[12] - '0';            // set 10's minute
  swap(temp2);
  temp1  = lcdData[13] - '0';           // set 1's minute
  rtc_set[1] = temp2 | temp1;
  temp2 = lcdData[ 9] - '0';            // set 10's hour
  swap(temp2);
  temp1  = lcdData[10] - '0';           // set 1's hour
  if (lcdData[14] == 'P')
  {
    temp3 = 0x20;                       // set AM-PM
  }
  else
  {
    temp3 = 0;
  }

  rtc_set[2] = 0x40 | temp3 | temp2 | temp1;  // 0x40 - force 12 hour mode
  temp2 = lcdData[ 3] - '0';            // Set 10's date
  swap(temp2);
  temp1 = lcdData[ 4] - '0';            // Set 1's date
  rtc_set[4] = temp2 | temp1;
  temp2 = lcdData[ 0] - '0';            // Set 10's month
  swap(temp2);
  temp1  = lcdData[ 1] - '0';           // Set 1's month
  rtc_set[5] = temp2 | temp1;
  temp2 = lcdData[ 6] - '0';            // Set 10's year
  swap(temp2);
  temp1  = lcdData[ 7] - '0';           // Set 1's year
  rtc_set[6] = temp2 | temp1;
  i2c_start();
  i2c_write(0xD0);                      // I2C slave read mode - rtc clock address
  i2c_write(0x00);                      // Point to the start of the clock registers
  for (i = 0 ; i < 7 ; i++)
  {
    i2c_write(rtc_set[i]);
  }
  i2c_write(0x00);                      // RTC clock control register
  i2c_stop();
//  Peripheral_stopI2C();
}


//******************************************************************************
//
//  Function: Peripheral_writeEEPROM()
//
//  Description:
//  ============
//  This function writes the passed data to the EEPROM.
//
//******************************************************************************
void Peripheral_writeEEPROM(byte data)
{
  i2c_start();
  i2c_write(0xA0);
  i2c_write(make8(eepromMemPtr,1));
  i2c_write(make8(eepromMemPtr,0));
  i2c_write(data);
  i2c_stop();
  delay_ms(12);
}
/*
#ifdef DEBUG
#inline
void Break_Point(void){
      showTime = false;
      LCD_setCursorPosition(2, 1);
      strcpy(lcdData, "Break Point");
      LCD_updateDisplay(); while(true);
}

#endif
*/
