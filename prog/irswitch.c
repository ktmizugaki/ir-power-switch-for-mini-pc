#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/cpufunc.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "lib/pindesc.h"
#include "libir/ir_ctrl.h"
#include "lib/dbg.h"

#define PIN_PWR_SW  B,0
#define PIN_LED     B,1

static void pwr_sw_init(void)
{
    /* configure power switch pin as HiZ or Low output */
    PINDESC_PULLUP_DIS(PIN_PWR_SW);
    PINDESC_SET_INPUT(PIN_PWR_SW);

    /* configure led pin as output */
    PINDESC_SET_LOW(PIN_LED);
    PINDESC_SET_OUTPUT(PIN_LED);
}

static inline void pwr_sw_on(void)
{
    PINDESC_SET_OUTPUT(PIN_PWR_SW);
}

static inline void pwr_sw_off(void)
{
    PINDESC_SET_INPUT(PIN_PWR_SW);
}

static inline void led_on(void)
{
    PINDESC_SET_HIGH(PIN_LED);
}

static inline void led_off(void)
{
    PINDESC_SET_LOW(PIN_LED);
}

static void init(void)
{
    clock_prescale_set(clock_div_8);
    _NOP();
    _NOP();
    cli();
    /* disable modules */
    PRR |= _BV(PRTIM1)|_BV(PRTIM0)|_BV(PRUSI)|_BV(PRADC);
    /* disable analog comparator */
    ACSR &= ~_BV(ACIE);
    ACSR |= _BV(ACD);
    /* enable pull-up resistors for all GPIO */
    MCUCR &= ~_BV(PUD);

    IR_initialize();
    pwr_sw_init();
    dbg_init();

    sei();
}

static const uint8_t PWR_ON_CODE[6] = {
    0xAA, 0x5A, 0x8F, 0x12, 0x12, 0x3B
};

static int timeout = 0;
void IR_ontx(void)
{
    timeout = 1;
}

int main(void) __attribute__((OS_main));
int main(void)
{
    init();
    dbg_print_str_P(PSTR("irswitch\n"));
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    while (1) {
        sleep_mode();
        /* woke up by IR start */
        if (!PINDESC_GET(PIN_IR)) {
            set_sleep_mode(SLEEP_MODE_IDLE);
        }
        if (IrCtrl.stat == IR_RECVED) {
            uint8_t i, l = IrCtrl.len;
            dbg_print_str_P(PSTR("fmt: "));
            dbg_print_num(IrCtrl.fmt, 2);
            dbg_nl();
            dbg_print_str_P(PSTR("data: "));
            dbg_print_num(l, 3);
            l = (l+7)/8;
            for (i = 0; i < l; i++) {
                dbg_print_char(',');
                dbg_print_hex(IrCtrl.rxdata[i], 2);
            }
            dbg_nl();
            if (l == 6 && memcmp(PWR_ON_CODE, (void*)IrCtrl.rxdata, 6) == 0) {
                dbg_print_str_P(PSTR("match\n"));
                led_on();
                _delay_ms(160);
                pwr_sw_on();
                _delay_ms(40);
                pwr_sw_off();
                led_off();
                _delay_ms(200);
                led_on();
                _delay_ms(200);
                led_off();
            }
            IrCtrl.stat = IR_IDLE;
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        }
        if (timeout) {
            dbg_print_str_P(PSTR("timeout\n"));
            timeout = 0;
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        }
    }
    return 0;
}
