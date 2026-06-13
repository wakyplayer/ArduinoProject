#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

#include "rtc_wrapper.h"
#include "config.h"

ThreeWire myWire(RTC_DAT_PIN, RTC_CLK_PIN, RTC_RST_PIN);
RtcDS1302<ThreeWire> rtc(myWire);

void clock_init() {
    rtc.Begin();

    if (rtc.GetIsWriteProtected()) {
        rtc.SetIsWriteProtected(false);
    }

    if (!rtc.GetIsRunning()) {
        rtc.SetIsRunning(true);
    }

    if (!rtc.IsDateTimeValid()) {
        rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
    }
}

byte get_day() {
    RtcDateTime dt = rtc.GetDateTime();
    return dt.Day();
}

byte get_month() {
    RtcDateTime dt = rtc.GetDateTime();
    return dt.Month();
}

int get_year() {
    RtcDateTime dt = rtc.GetDateTime();
    return dt.Year();
}

byte get_hours() {
    RtcDateTime dt = rtc.GetDateTime();
    return dt.Hour();
}

byte get_minutes() {
    RtcDateTime dt = rtc.GetDateTime();
    return dt.Minute();
}

byte get_seconds() {
    RtcDateTime dt = rtc.GetDateTime();
    return dt.Second();
}

struct dt now() {
    RtcDateTime current = rtc.GetDateTime();

    struct dt result;
    result.day = current.Day();
    result.month = current.Month();
    result.year = current.Year();
    result.hours = current.Hour();
    result.minutes = current.Minute();
    result.seconds = current.Second();

    return result;
}

void set_date(const byte day, const byte month, const int year) {
    RtcDateTime current = rtc.GetDateTime();
    rtc.SetDateTime(RtcDateTime(year, month, day, current.Hour(), current.Minute(), current.Second()));
}

void set_time(const byte hours, const byte minutes, const byte seconds) {
    RtcDateTime current = rtc.GetDateTime();
    rtc.SetDateTime(RtcDateTime(current.Year(), current.Month(), current.Day(), hours, minutes, seconds));
}

void set_datetime(const byte day, const byte month, const int year, const byte hours, const byte minutes, const byte seconds) {
    rtc.SetDateTime(RtcDateTime(year, month, day, hours, minutes, seconds));
}

void format_time_line(char *line, int line_size) {
    struct dt t = now();
    snprintf(line, line_size, "%02u:%02u:%02u", t.hours, t.minutes, t.seconds);
}

void format_date_line(char *line, int line_size) {
    struct dt t = now();
    snprintf(line, line_size, "%02u.%02u.%04u", t.day, t.month, t.year);
}
