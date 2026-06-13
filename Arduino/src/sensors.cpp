#include <Arduino.h>
#include <DHT.h>

#include "config.h"
#include "sensors.h"

void sensors_init() {
    // DHT is created while reading, so this module does not need a global variable.
}

float get_temperature() {
    DHT dht(DHT_PIN, DHT11);
    dht.begin();

    float value = dht.readTemperature();
    if (isnan(value)) {
        return 0;
    }
    return value;
}

int get_humidity() {
    DHT dht(DHT_PIN, DHT11);
    dht.begin();

    float value = dht.readHumidity();
    if (isnan(value)) {
        return 0;
    }
    return (int)value;
}

void format_environment_lines(char *line1, int line1_size, char *line2, int line2_size) {
    char temp_text[8];

    dtostrf(get_temperature(), 4, 1, temp_text);
    snprintf(line1, line1_size, "Temp:%s C", temp_text);
    snprintf(line2, line2_size, "Hum:%d %%", get_humidity());
}
