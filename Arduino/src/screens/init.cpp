#include <Arduino.h>

#include "config.h"
#include "context.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "screens.h"
#include "sensors.h"

void alarm_init(struct context *ctx);
void alarm_update_status_led(struct context *ctx);

enum screen init_screen(struct context *ctx) {
    init();
    Serial.begin(BAUD_RATE);

    for (byte i = 0; i < 4; i++) {
        ctx->last_buttons[i] = false;
    }

    ctx->edit_field = 0;
    ctx->edit_hour = 0;
    ctx->edit_minute = 0;
    ctx->edit_day = 1;
    ctx->edit_month = 1;
    ctx->edit_year = 2026;
    ctx->edit_loaded = false;

    ctx->alarm_hour = 7;
    ctx->alarm_minute = 0;
    ctx->alarm_edit_field = 0;
    ctx->alarm_enabled = false;
    ctx->alarm_ringing = false;
    ctx->snooze_enabled = false;
    ctx->snooze_hour = 0;
    ctx->snooze_minute = 0;
    ctx->last_alarm_key = -1;

    pinMode(BTN1_PIN, INPUT_PULLUP);
    pinMode(BTN2_PIN, INPUT_PULLUP);
    pinMode(BTN3_PIN, INPUT_PULLUP);
    pinMode(BTN4_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RGB_GREEN_PIN, OUTPUT);
    pinMode(RGB_RED_PIN, OUTPUT);

    lcd_init();
    clock_init();
    sensors_init();
    alarm_init(ctx);
    alarm_update_status_led(ctx);

    lcd_clear();
    lcd_print_at(0, 0, (char *)"Alarm clock");
    lcd_print_at(1, 0, (char *)"ready");
    delay(1000);

    return CLOCK_SCR;
}
