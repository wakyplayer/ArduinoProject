#include <Arduino.h>

#include "config.h"
#include "context.h"
#include "rtc_wrapper.h"
#include "screens.h"
#include "lcd_wrapper.h"
#include "sensors.h"

void alarm_check(struct context *ctx);
void alarm_update_status_led(struct context *ctx);

enum screen clock_screen(struct context *ctx) {
    byte pins[4] = {BTN1_PIN, BTN2_PIN, BTN3_PIN, BTN4_PIN};
    byte button = 255;

    alarm_check(ctx);
    alarm_update_status_led(ctx);
    if (ctx->alarm_ringing) return ALARM_SCR;

    for (byte i = 0; i < 4; i++) {
        bool current = digitalRead(pins[i]) == LOW;
        if (current && !ctx->last_buttons[i]) button = i;
        ctx->last_buttons[i] = current;
    }

    if (button == 0) return SHOW_ENV_SCR;
    if (button == 1) return SHOW_DATE_SCR;
    if (button == 2) {
        ctx->edit_loaded = false;
        return FACTORY_RESET_SCR;
    }
    if (button == 3) {
        ctx->alarm_edit_field = 0;
        return ALARM_SCR;
    }

    char line1[17];
    char line2[17];

    format_time_line(line1, sizeof(line1));
    snprintf(line2, sizeof(line2), "Alarm %02u:%02u %s",
             ctx->alarm_hour, ctx->alarm_minute, ctx->alarm_enabled ? "ON" : "OFF");

    lcd_print_two_lines(line1, line2);
    delay(200);
    return CLOCK_SCR;
}

enum screen show_env_screen(struct context *ctx) {
    byte pins[4] = {BTN1_PIN, BTN2_PIN, BTN3_PIN, BTN4_PIN};
    byte button = 255;

    alarm_check(ctx);
    alarm_update_status_led(ctx);
    if (ctx->alarm_ringing) return ALARM_SCR;

    for (byte i = 0; i < 4; i++) {
        bool current = digitalRead(pins[i]) == LOW;
        if (current && !ctx->last_buttons[i]) button = i;
        ctx->last_buttons[i] = current;
    }

    if (button == 0) return CLOCK_SCR;

    char line1[17];
    char line2[17];

    format_environment_lines(line1, sizeof(line1), line2, sizeof(line2));

    lcd_print_two_lines(line1, line2);
    delay(300);
    return SHOW_ENV_SCR;
}

enum screen show_date_screen(struct context *ctx) {
    byte pins[4] = {BTN1_PIN, BTN2_PIN, BTN3_PIN, BTN4_PIN};
    byte button = 255;

    alarm_check(ctx);
    alarm_update_status_led(ctx);
    if (ctx->alarm_ringing) return ALARM_SCR;

    for (byte i = 0; i < 4; i++) {
        bool current = digitalRead(pins[i]) == LOW;
        if (current && !ctx->last_buttons[i]) button = i;
        ctx->last_buttons[i] = current;
    }

    if (button == 0) return CLOCK_SCR;
    if (button == 2) {
        ctx->edit_loaded = false;
        return FACTORY_RESET_SCR;
    }

    char line1[17];

    format_date_line(line1, sizeof(line1));
    lcd_print_two_lines(line1, "B3 set time");
    delay(300);
    return SHOW_DATE_SCR;
}

enum screen factory_reset_screen(struct context *ctx) {
    byte pins[4] = {BTN1_PIN, BTN2_PIN, BTN3_PIN, BTN4_PIN};
    byte button = 255;

    alarm_check(ctx);
    alarm_update_status_led(ctx);
    if (ctx->alarm_ringing) return ALARM_SCR;

    for (byte i = 0; i < 4; i++) {
        bool current = digitalRead(pins[i]) == LOW;
        if (current && !ctx->last_buttons[i]) button = i;
        ctx->last_buttons[i] = current;
    }

    if (!ctx->edit_loaded) {
        struct dt t = now();
        ctx->edit_hour = t.hours;
        ctx->edit_minute = t.minutes;
        ctx->edit_day = t.day;
        ctx->edit_month = t.month;
        ctx->edit_year = t.year;
        ctx->edit_field = 0;
        ctx->edit_loaded = true;
    }

    if (button == 0) {
        ctx->edit_loaded = false;
        return CLOCK_SCR;
    }
    if (button == 1) ctx->edit_field = (ctx->edit_field + 1) % 5;

    if (button == 2 || button == 3) {
        int delta = button == 2 ? 1 : -1;

        if (ctx->edit_field == 0) ctx->edit_hour = (ctx->edit_hour + 24 + delta) % 24;
        if (ctx->edit_field == 1) ctx->edit_minute = (ctx->edit_minute + 60 + delta) % 60;
        if (ctx->edit_field == 2) ctx->edit_day = constrain(ctx->edit_day + delta, 1, 31);
        if (ctx->edit_field == 3) ctx->edit_month = constrain(ctx->edit_month + delta, 1, 12);
        if (ctx->edit_field == 4) ctx->edit_year = constrain(ctx->edit_year + delta, 2024, 2099);

        set_datetime(ctx->edit_day, ctx->edit_month, ctx->edit_year, ctx->edit_hour, ctx->edit_minute, 0);
    }

    char line1[17];
    char line2[17];
    const char *field = "HOUR";

    if (ctx->edit_field == 1) field = "MIN";
    if (ctx->edit_field == 2) field = "DAY";
    if (ctx->edit_field == 3) field = "MONTH";
    if (ctx->edit_field == 4) field = "YEAR";

    snprintf(line1, sizeof(line1), "SET %s", field);
    snprintf(line2, sizeof(line2), "%02u:%02u %02u.%02u",
             ctx->edit_hour, ctx->edit_minute, ctx->edit_day, ctx->edit_month);

    lcd_print_two_lines(line1, line2);
    delay(150);
    return FACTORY_RESET_SCR;
}
