//Will animate the first 3 keys on VIA/ORGB switch as either white or R G B
#include "quantum.h"
#include "indicator.h"
#include "lpm.h"
#if defined(PROTOCOL_CHIBIOS)
#    include <usb_main.h>
#elif if defined(PROTOCOL_LUFA)
#    include "lufa.h"
#endif
#include "eeprom.h"

#if (defined(LED_MATRIX_ENABLE) || defined(RGB_MATRIX_ENABLE))

#ifndef ANI_GROWING_INTERVAL
#    define ANI_GROWING_INTERVAL 300
#endif

#ifndef ANI_ON_INTERVAL
#    define ANI_ON_INTERVAL 2000
#endif

#ifdef LED_MATRIX_ENABLE
#    define LED_DRIVER_IS_ENABLED led_matrix_is_enabled
#endif

#ifdef RGB_MATRIX_ENABLE
#    define LED_DRIVER_IS_ENABLED rgb_matrix_is_enabled
#endif

enum {
    ANI_NONE,
    ANI_GROWING,
    ANI_BLINK_OFF,
    ANI_BLINK_ON,
};

bool cur_is_switch_orgb;
static uint8_t  animation_state = 0;
static uint32_t ani_timer_buffer = 0;
static uint8_t  cur_led;
static uint32_t time_interval;
#ifdef RGB_MATRIX_ENABLE
static uint8_t r, g, b;
#endif

extern indicator_config_t indicator_config;
extern backlight_state_t  original_backlight_state;

void switch_animation_start(bool is_switch_orgb) {
    cur_is_switch_orgb = is_switch_orgb;
    indicator_enable();
    animation_state = ANI_GROWING;
    ani_timer_buffer = timer_read32();
    cur_led = 0;
    time_interval = ANI_GROWING_INTERVAL;
#ifdef RGB_MATRIX_ENABLE
    r = g = b = 0;
#endif
}

void switch_animation_stop(void) {
    animation_state = ANI_NONE;
}

bool switch_animation_isactive(void) {
    return animation_state;
}

void switch_animation_indicate(void) {
#ifdef LED_MATRIX_ENABLE

    for (uint8_t i = 0; i <= LED_MATRIX_LED_COUNT; i++) {
        led_matrix_set_value(i, 0);
    }

    if (animation_state == ANI_GROWING || animation_state == ANI_BLINK_ON)
        for (uint8_t i = 0; i < 3; i++)
            led_matrix_set_value(i, 255);
#endif

#ifdef RGB_MATRIX_ENABLE

    // for (uint8_t i = 0; i <= RGB_MATRIX_LED_COUNT; i++) {
    //     rgb_matrix_set_color(i, 0, 0, 0);
    // }

    if (animation_state == ANI_GROWING) {
        if(cur_is_switch_orgb){
            switch(cur_led) {
                case 0:
                    r = 255; g = b = 0;
                    break;
                case 1:
                    r = 0; g = 255; b = 0;
                    break;
                default:
                    r = g = 0; b = 255;
                    break;
            }
        }else{
            r = 255; g = 166; b = 173;
        }

        rgb_matrix_set_color(cur_led, r, g, b);
    }
    else if (animation_state == ANI_BLINK_ON) {
        for (uint8_t i = 0; i < 3; i++) {
            if(cur_is_switch_orgb){
                switch(i) {
                    case 0:
                        r = 255; g = b = 0;
                        break;
                    case 1:
                        r = 0; g = 255; b = 0;
                        break;
                    default:
                        r = g = 0; b = 255;
                        break;
                }
            }else{
                r = 255; g = 166; b = 173;
            }

            rgb_matrix_set_color(i, r, g, b);
        }
    }
#endif
}

void switch_animation_update(void) {
    switch (animation_state) {
        case ANI_GROWING:
            if (cur_led < 2)
                cur_led += 1;
            else {
                if (cur_led == 0) cur_led = 2;
                animation_state = ANI_BLINK_OFF;
            }
            break;

        case ANI_BLINK_OFF:
            time_interval   = ANI_ON_INTERVAL;
            animation_state = ANI_BLINK_ON;
            break;

        case ANI_BLINK_ON:
            animation_state = ANI_NONE;
            indicator_eeconfig_reload();
            if (indicator_config.value == 0 && !LED_DRIVER_IS_ENABLED()) {
                indicator_disable();
            }
            lpm_timer_reset();
            break;

        default:
            break;
    }

    ani_timer_buffer = timer_read32();
}

void switch_animation_task(void) {
    if (animation_state && sync_timer_elapsed32(ani_timer_buffer) > time_interval) {
        switch_animation_update();
    }
}

#endif