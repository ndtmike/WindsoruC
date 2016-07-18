//******************************************************************************
//
//  Filename: Windsor.h
//
//  Copyright 2006-2010, NDT James Instruments Inc.  All rights reserved.
//
//  Description:
//  ============
//  This file contains all #defines, constants, and structures associated with
//   the Windsor Probe.
//
//  Revision History:
//  =================
//  Date        Author         Description
//  ----------  -------------  -------------------------------------------------
//  10/26/2010  Charlie Green  Initial version
//
//******************************************************************************

//******************************************************************************
//  Include Files
//******************************************************************************

//******************************************************************************
//  Definitions
//******************************************************************************
#fuses XT, NOWDT, NOBROWNOUT, NOPROTECT, NOPUT

#ifndef DEBUG
#define DEBUG
#endif

#byte lcd_port = 8                                // LCD port is connected to port D (address 8)
#byte kbd_port = 6                                // Keypad is connected to port B (address 6)

// #define Macro
#define getHighByte(a)                  (*(&a+1))

// Menu locations
#define MENU_MEASURE                    1         //        Main:Measure        Menu
#define MENU_RUN_TEST                   2         //        Main:Run Test       Menu
#define MENU_SHOW_TESTS                 3         //        Main:Show Tests     Menu
#define MENU_DOWNLOAD_TESTS             4         //        Main:Download Tests Menu
#define MENU_ENTER_SETUP                5         //        Main:Enter Setup    Menu
#define MENU_SHOW_SETTINGS              6         // Enter Setup:Show Settings  Submenu
#define MENU_SET_SETTINGS               7         // Enter Setup:Set Settings   Submenu
#define MENU_SET_CLOCK                  8         // Enter Setup:Set Clock      Submenu
#define MENU_CALIBRATE                  9         // Enter Setup:Calibrate      Submenu

//Submenu Setting
#define SUBMENU_SET_SHOW                           1
#define SUBMENU_SET_SET                            2
#define SUBMENU_SET_CLOCK                          3
#define SUBMENU_SET_CALIBRATE                      4

// Submenu: Power
#define SUBMENU_POWER_STD               1
#define SUBMENU_POWER_LOW               2
#define SUBMENU_POWER_HIGH              3

// Submenu: Density
#define SUBMENU_DENSITY_STD             4
#define SUBMENU_DENSITY_LIGHT           5

// Submenu: MOH
#define SUBMENU_MOH_3                   6
#define SUBMENU_MOH_4                   7
#define SUBMENU_MOH_5                   8
#define SUBMENU_MOH_6                   9
#define SUBMENU_MOH_7                   10

// Submenu: Units
#define SUBMENU_UNITS_MPA               11
#define SUBMENU_UNITS_PSI               12

// Submenu: Agg. Size
#define SUBMENU_AGG_SIZE_MED            13
#define SUBMENU_AGG_SIZE_SMALL          14
#define SUBMENU_AGG_SIZE_LARGE          15

// Submenu: Weight
#define SUBMENU_WEIGHT_HIGH             16
#define SUBMENU_WEIGHT_MED              17
#define SUBMENU_WEIGHT_LOW              18
#define SUBMENU_WEIGHT_SUPER_LOW        19

// Maximum Test Storage Locations
#define TEST_MAX_SETS                   99
#define TEST_SET_SIZE                   16

// EEPROM Memory Locations
#define EEPROM_TOP                      8142
#define EEPROM_POWER                    8143
#define EEPROM_DENSITY                  8144
#define EEPROM_WEIGHT                   8145
#define EEPROM_MOHS                     8146
#define EEPROM_UNITS                    8147
#define EEPROM_AGG_SIZE                 8148
#define EEPROM_ZERO                     8149
#define EEPROM_FULL_SCALE               8150
#define EEPROM_TESTS                    8151

// Keypad Connections: Column 0 is B3.
#define COL0                            (1 << 3)
#define COL1                            (1 << 2)
//#define ROW0                            (1 << 7)
#define ROW1                            (1 << 4)
#define ROW2                            (1 << 5)
#define ALL_COL                         (COL0 | COL1)
//#define ALL_ROWS                        (ROW0 | ROW1 | ROW2)
#define ALL_ROWS                        (ROW1 | ROW2)

//Keyboard
#define DOWN_KEY   2
#define UP_KEY    12
#define ENTER_KEY  3
#define ESC_KEY   13

// Agg. Size, Slope, and Offset for Mega-Pascals (MPa)
#define AGG_SIZE_LIMIT_1_MPA            660
#define AGG_SIZE_LIMIT_2_MPA            840
#define AGG_SIZE_LIMIT_3_MPA            1170

#define OFFSET_LARGE_3_MPA              220
#define OFFSET_LARGE_4_MPA              255
#define OFFSET_LARGE_5_MPA              300
#define OFFSET_LARGE_6_MPA              365
#define OFFSET_LARGE_7_MPA              435

#define OFFSET_SMALL_3_MPA              437
#define OFFSET_SMALL_4_MPA              517
#define OFFSET_SMALL_5_MPA              598
#define OFFSET_SMALL_6_MPA              731
#define OFFSET_SMALL_7_MPA              879

#define SLOPE_LARGE_3_MPA               91
#define SLOPE_LARGE_4_MPA               94
#define SLOPE_LARGE_5_MPA               100
#define SLOPE_LARGE_6_MPA               109
#define SLOPE_LARGE_7_MPA               118

#define SLOPE_SMALL_3_MPA               181
#define SLOPE_SMALL_4_MPA               190
#define SLOPE_SMALL_5_MPA               199
#define SLOPE_SMALL_6_MPA               217
#define SLOPE_SMALL_7_MPA               237

// Conversion Factors
#define ADC_SCALE_FACTOR_METRIC         3810
#define DISTANCE_CONV_FACTOR            3.937
#define DISTANCE_OFFSET_METRIC          2540
#define PRESSURE_CONV_FACTOR            1.4503773773

//******************************************************************************
//  Enumerations
//******************************************************************************

//******************************************************************************
//  Constants
//******************************************************************************

//******************************************************************************
//  Structures
//******************************************************************************

//******************************************************************************
//  Prototypes (Global)
//******************************************************************************
#use                                    DELAY(clock = 4000000)
#use                                    fixed_io(b_outputs = pin_b2, pin_b3)
#use                                    i2c(MASTER, sda = PIN_C4, scl = PIN_C3, SLOW, FORCE_SW)
#use                                    rs232(baud = 9600, xmit = PIN_C6, rcv = PIN_C7, brgh1ok)

// Config
void                                    Config_initialize(void);
void                                    Config_loadSetup(void);
void                                    Config_saveSetup(void);
void                                    Config_setSettings(void);

// Display
void                                    Display_checkTestData(void);
/*#separate*/ int32                   Display_doCalculation(float x);
void                                    Display_showData(void);
void                                    Display_showDecimal(int data);
void                                    Display_showDistance(void);
void                                    Display_showMenuDownloadTests(void);
void                                    Display_showMenuEnterSetup(void);
void                                    Display_showMenuMeasure(void);
void                                    Display_showMenuRunTest(void);
void                                    Display_showMenuShowTests(void);
void                                    Display_showPressure(void);
void                                    Display_showSubmenuCalibrate(void);
void                                    Display_showSubmenuSetClock(void);
void                                    Display_showSubmenuSetSettings(byte data);
void                                    Display_showSubmenuShowSettings(void);
void                                    Display_showTime(void);
void                                    Display_updateDisplayPressure(int32 pressure);

// Keyboard
void                                    Keyboard_getDownKey(void);
char                                    Keyboard_getKeypress(void);
char                                    Keyboard_getKeyRaw(void);
void                                    Keyboard_getUpKey(void);

// LCD
void                                    LCD_clearDisplay(void);
void                                    LCD_setCursorPosition(byte row, byte col);
void                                    LCD_turnOffCursor(void);
void                                    LCD_turnOnCursor(void);
void                                    LCD_updateDisplay(void);
void                                    LCD_waitForReadySignal(void);

// Main
void                                    main(void);

// Peripheral
void                                    Peripheral_getADC(void);
byte                                    Peripheral_readEEPROM(void);
void                                    Peripheral_readRTC(void);
void                                    Peripheral_saveData(void);
void                                    Peripheral_scaleADC(void);
void                                    Peripheral_setRTC(void);
//void                                    Peripheral_startI2C(void);
//void                                    Peripheral_stopI2C(void);
void                                    Peripheral_writeEEPROM(byte data);

#ifdef DEBUG
#inline
void Break_Point(void);
#endif
