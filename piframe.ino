#include <MD_MAX72xx.h>         // Library for controlling LED matrices
#include <MD_Parola.h>          // Advanced library for LED matrix animations
#include <SPI.h>                // SPI communication library
#include <LiquidCrystal_I2C.h>  // Library for I2C-controlled LCD displays
#include <millisDelay.h>        // Non-blocking delay library

// Code by Alisdair Gurney and Kev Milne - Tightened up and checked by CGPT.

// LCD Configuration
#define LCD_I2C_ADDRESS 0x27    // I2C address for the LCD
#define LCD_WIDTH 16            // LCD width in characters
#define LCD_HEIGHT 2            // LCD height in rows
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_WIDTH, LCD_HEIGHT);  // LCD instance

// LED Matrix Configuration
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW  // Hardware type for LED matrix
#define MAX_DEVICES 4                      // Number of connected LED matrix modules
#define CLK_PIN 13                         // Clock pin for LED matrix
#define DATA_PIN 11                        // Data pin for LED matrix
#define CS_PIN 10                          // Chip Select pin for LED matrix
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); // LED matrix instance

// Timer for LCD updates
millisDelay lcdUpdateTimer;                // Timer for periodic LCD updates
const unsigned long LCD_UPDATE_INTERVAL = 4000;  // Update interval in milliseconds

// Variables
int lcdRow = 0; // Tracks the current row for alternating messages on the LCD

// Function Prototypes
void setupLCD();             // Initialize the LCD
void updateLCD();            // Update the LCD display content
void setupLEDMatrix();       // Initialize the LED matrix
void randomLEDFlashing();    // Create a random LED flashing effect

void setup() {
  Serial.begin(115200);      // Initialize serial communication for debugging
  setupLCD();                // Set up the LCD
  setupLEDMatrix();          // Set up the LED matrix
  lcdUpdateTimer.start(LCD_UPDATE_INTERVAL);  // Start the LCD update timer
}

void loop() {
  // Check if it's time to update the LCD
  if (lcdUpdateTimer.justFinished()) {
    updateLCD();             // Update the LCD display
    lcdUpdateTimer.restart();// Restart the timer
  }
  // Continuously run the random LED flashing effect
  randomLEDFlashing();
}

void setupLCD() {
  lcd.init();                // Initialize the LCD
  lcd.begin(LCD_WIDTH, LCD_HEIGHT); // Set LCD dimensions
  lcd.backlight();           // Turn on the LCD backlight
  lcd.clear();               // Clear any existing content on the LCD
  lcd.setCursor(0, 0);       // Set cursor to the first row
  lcd.print("Pi-Frame: ACTIVE "); // Display initial message
  lcd.setCursor(0, 1);       // Move cursor to the second row
  lcd.print("MVS38J-81-IPL150"); // Display additional information
}

void updateLCD() {
  lcd.clear();               // Clear the LCD before updating
  lcd.setCursor(0, lcdRow);  // Set cursor to the current row
  lcd.print("Pi-Frame: ACTIVE  "); // Display active message
  lcd.setCursor(0, 1 - lcdRow);    // Alternate to the other row
  lcd.print("MVS38J-81-IPL150");  // Display secondary message
  lcdRow = 1 - lcdRow;       // Toggle between rows for the next update
}

void setupLEDMatrix() {
  mx.begin();                // Initialize the LED matrix
  mx.control(MD_MAX72XX::INTENSITY, 5); // Set matrix brightness to medium
  mx.clear();                // Clear any previous data on the matrix
}

void randomLEDFlashing() {
  static unsigned long lastUpdate = 0; // Track the last update time
  const unsigned long updateInterval = 350; // Time between updates in milliseconds

  // Check if enough time has passed for the next update
  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();   // Update the last update time
    mx.clear();              // Clear the LED matrix

    int ledCount = 64;       // Number of LEDs to light up randomly
    uint8_t brightness = random(1, 3); // Random brightness level (1 to 3)
    mx.control(MD_MAX72XX::INTENSITY, brightness); // Apply the brightness

    // Randomly light up LEDs on the matrix
    for (int i = 0; i < ledCount; i++) {
      uint8_t x = random(0, 8); // Random row (0-7)
      uint8_t y = random(0, mx.getColumnCount()); // Random column
      mx.setPoint(x, y, true); // Turn on the LED at (x, y)
    }

    mx.update();             // Update the LED matrix with new data
  }
}
