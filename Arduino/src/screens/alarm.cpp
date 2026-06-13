#include <Arduino.h>
#include <EEPROM.h>

#include "config.h"
#include "context.h"
#include "rtc_wrapper.h"
#include "screens.h"
#include "lcd_wrapper.h"

#define EEPROM_MAGIC_ADDR 0
#define EEPROM_HOUR_ADDR 1
#define EEPROM_MINUTE_ADDR 2
#define EEPROM_ENABLED_ADDR 3
#define EEPROM_MAGIC 42
#define SNOOZE_MINUTES 5

void alarm_init(struct context *ctx) {
    if (EEPROM.read(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC) {
        ctx->alarm_hour = EEPROM.read(EEPROM_HOUR_ADDR);
        ctx->alarm_minute = EEPROM.read(EEPROM_MINUTE_ADDR);
        ctx->alarm_enabled = EEPROM.read(EEPROM_ENABLED_ADDR) == 1;
    }

    if (ctx->alarm_hour > 23) ctx->alarm_hour = 7;
    if (ctx->alarm_minute > 59) ctx->alarm_minute = 0;
}

void alarm_update_status_led(struct context *ctx) {
    if (ctx->alarm_ringing) {
        digitalWrite(RGB_GREEN_PIN, LOW);
        digitalWrite(RGB_RED_PIN, HIGH);
    } else {
        digitalWrite(RGB_GREEN_PIN, ctx->alarm_enabled ? HIGH : LOW);
        digitalWrite(RGB_RED_PIN, LOW);
    }
}

void alarm_check(struct context *ctx) {
    if (ctx->alarm_ringing) {
        return;
    }

    struct dt t = now();
    int alarm_key = t.day * 1440 + t.hours * 60 + t.minutes;

    if (ctx->snooze_enabled) {
        if (t.hours == ctx->snooze_hour && t.minutes == ctx->snooze_minute && alarm_key != ctx->last_alarm_key) {
            ctx->snooze_enabled = false;
            ctx->last_alarm_key = alarm_key;
            ctx->alarm_ringing = true;
            tone(BUZZER_PIN, 1000);
        }
        return;
    }

    if (!ctx->alarm_enabled) {
        return;
    }

    if (t.hours == ctx->alarm_hour && t.minutes == ctx->alarm_minute && alarm_key != ctx->last_alarm_key) {
        ctx->last_alarm_key = alarm_key;
        ctx->alarm_ringing = true;
        tone(BUZZER_PIN, 1000);
    }
}

enum screen alarm_screen(struct context *ctx) {
    byte pins[4] = {BTN1_PIN, BTN2_PIN, BTN3_PIN, BTN4_PIN};
    byte button = 255;

    for (byte i = 0; i < 4; i++) {
        bool current = digitalRead(pins[i]) == LOW;
        if (current && !ctx->last_buttons[i]) button = i;
        ctx->last_buttons[i] = current;
    }

    if (ctx->alarm_ringing) {
        if (button == 1) {
            struct dt t = now();
            int snooze_time = t.hours * 60 + t.minutes + SNOOZE_MINUTES;

            ctx->snooze_hour = (snooze_time / 60) % 24;
            ctx->snooze_minute = snooze_time % 60;
            ctx->snooze_enabled = true;
            ctx->alarm_ringing = false;
            ctx->last_alarm_key = -1;
            noTone(BUZZER_PIN);

            lcd_print_two_lines("SNOOZE", "wait 5 min");
            delay(1000);
            return CLOCK_SCR;
        }

        if (button == 0 || button == 2 || button == 3) {
            ctx->alarm_ringing = false;
            ctx->snooze_enabled = false;
            noTone(BUZZER_PIN);
            return CLOCK_SCR;
        }

        alarm_update_status_led(ctx);
        lcd_print_two_lines("ALARM!", "B1 off B2 +5");
        delay(150);
        return ALARM_SCR;
    }

    alarm_check(ctx);
    alarm_update_status_led(ctx);
    if (ctx->alarm_ringing) return ALARM_SCR;

    if (button == 0) {
        ctx->alarm_edit_field = 0;
        return CLOCK_SCR;
    }
    if (button == 1) ctx->alarm_edit_field = (ctx->alarm_edit_field + 1) % 3;

    if (button == 2 || button == 3) {
        int delta = button == 2 ? 1 : -1;

        if (ctx->alarm_edit_field == 0) ctx->alarm_hour = (ctx->alarm_hour + 24 + delta) % 24;
        if (ctx->alarm_edit_field == 1) ctx->alarm_minute = (ctx->alarm_minute + 60 + delta) % 60;
        if (ctx->alarm_edit_field == 2) ctx->alarm_enabled = !ctx->alarm_enabled;

        EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC);
        EEPROM.update(EEPROM_HOUR_ADDR, ctx->alarm_hour);
        EEPROM.update(EEPROM_MINUTE_ADDR, ctx->alarm_minute);
        EEPROM.update(EEPROM_ENABLED_ADDR, ctx->alarm_enabled ? 1 : 0);
    }

    char line1[17];
    char line2[17];

    if (ctx->alarm_edit_field == 2) {
        snprintf(line1, sizeof(line1), "ALARM %s", ctx->alarm_enabled ? "ON" : "OFF");
    } else {
        snprintf(line1, sizeof(line1), "SET ALARM %s", ctx->alarm_edit_field == 0 ? "HOUR" : "MIN");
    }
    snprintf(line2, sizeof(line2), "%02u:%02u", ctx->alarm_hour, ctx->alarm_minute);

    lcd_print_two_lines(line1, line2);
    delay(150);
    return ALARM_SCR;
}
