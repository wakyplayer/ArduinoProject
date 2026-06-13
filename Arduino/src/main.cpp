#include <Arduino.h>

#include "context.h"
#include "screens.h"





int main() {
    struct context context = {};
    enum screen screen = INIT_SCR;

    for (;;) {
        switch (screen) {
            case INIT_SCR:
                screen = init_screen(&context);
                break;

            case CLOCK_SCR:
                screen = clock_screen(&context);
                break;

            case SHOW_ENV_SCR:
                screen = show_env_screen(&context);
                break;

            case SHOW_DATE_SCR:
                screen = show_date_screen(&context);
                break;

            case FACTORY_RESET_SCR:
                screen = factory_reset_screen(&context);
                break;

            case ALARM_SCR:
                screen = alarm_screen(&context);
                break;
        }
    }
}
