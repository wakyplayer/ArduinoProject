#include <Arduino.h>
#include <Wire.h>
#include <I2C_LCD.h>

#include "lcd_wrapper.h"
#include "config.h"

I2C_LCD lcd(LCD_I2C_ADDRESS);

void lcd_init() {
    Wire.begin();
    lcd.begin(LCD_COLS, LCD_ROWS);
    lcd.backlight();
    lcd.clear();
}

void lcd_clear() {
    lcd.clear();
}

void lcd_set_cursor(int y, int x) {
    lcd.setCursor(x, y);
}

void lcd_print(char* text) {
    lcd.print(text);
}

void lcd_print_at(int y, int x, char* text) {
    lcd_set_cursor(y, x);
    lcd_print(text);
}

void lcd_backlight(bool state) {
    lcd.setBacklight(state);
}

void lcd_print_two_lines(const char *line1, const char *line2) {
    lcd_clear();
    lcd_print_at(0, 0, (char *)line1);
    lcd_print_at(1, 0, (char *)line2);
}
