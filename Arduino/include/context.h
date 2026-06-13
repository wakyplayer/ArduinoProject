#ifndef CONTEXT_H
#define CONTEXT_H

#include <Arduino.h>

/**
 * Alarm Clock context.
 *
 * This struct stores values that change while the program is running.
 * The same context is passed to screens instead of using global variables.
 */
struct context {
    bool last_buttons[4];

    byte edit_field;
    byte edit_hour;
    byte edit_minute;
    byte edit_day;
    byte edit_month;
    int edit_year;
    bool edit_loaded;

    byte alarm_hour;
    byte alarm_minute;
    byte alarm_edit_field;
    bool alarm_enabled;
    bool alarm_ringing;
    bool snooze_enabled;
    byte snooze_hour;
    byte snooze_minute;
    int last_alarm_key;
};

#endif // CONTEXT_H
