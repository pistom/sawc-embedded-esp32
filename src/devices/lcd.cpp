#include "lcd.h"

#define I2C_ADDR 0x3F
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

void lcdInit() {
  lcd.init();
  lcd.backlight();
}
