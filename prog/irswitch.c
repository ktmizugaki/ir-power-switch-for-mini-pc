#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/cpufunc.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <string.h>
#include "lib/pindesc.h"
#include "libir/ir_ctrl.h"
#include "lib/dbg.h"

#define PIN_PWR_SW  B,0
#define PIN_LED     B,1

static inline int get_ir_st(void)
{
    return !PINDESC_GET(PIN_IR);
}

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

    /* setup ir pin to check learning mode. */
    PINDESC_SET_INPUT(PIN_IR);
    PINDESC_PULLUP_DIS(PIN_IR);

    pwr_sw_init();
    dbg_init();

    sei();
}

static uint8_t* const eeprom = (void*)8;
static uint8_t pwr_on_code[1+(IR_MAX_RCVR+7)/8];
static int learn_mode = 0;

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
    if (get_ir_st()) {
        dbg_print_str_P(PSTR("learn mode\n"));
        led_on();
        learn_mode = 1;
        while (get_ir_st())
            ;
        led_off();
    } else {
        pwr_on_code[0] = eeprom_read_byte(eeprom);
        if (pwr_on_code[0] > 0 && pwr_on_code[0] < IR_MAX_RCVR) {
            int i, c = (pwr_on_code[0]+7)/8;
            dbg_print_str_P(PSTR("studied: "));
            dbg_print_num(pwr_on_code[0], 3);
            eeprom_read_block(pwr_on_code+1, eeprom+1, (pwr_on_code[0]+7)/8);
            dbg_print_num(pwr_on_code[0], 3);
            for (i = 0; i < c; i++) {
                dbg_print_char(',');
                dbg_print_hex(pwr_on_code[i+1], 2);
            }
            dbg_nl();
        } else {
            pwr_on_code[0] = 0;
        }
    }
    IR_initialize();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    while (1) {
        sleep_mode();
        /* woke up by IR start */
        if (get_ir_st()) {
            set_sleep_mode(SLEEP_MODE_IDLE);
        }
        if (IrCtrl.stat == IR_RECVED) {
            uint8_t l = IrCtrl.len;
            uint8_t i, c = (l+7)/8;
            if (learn_mode == 1) {
                pwr_on_code[0] = l;
                memcpy(pwr_on_code+1, (void*)IrCtrl.rxdata, c);
                dbg_print_str_P(PSTR("read 1st signal\n"));
                led_on();
                _delay_ms(200);
                led_off();
                learn_mode = 2;
            } else if (learn_mode == 2) {
                learn_mode = 3;
                dbg_print_str_P(PSTR("read 2nd signal\n"));
                if (pwr_on_code[0] == l && memcmp(pwr_on_code+1, (void*)IrCtrl.rxdata, c) == 0) {
                    dbg_print_str_P(PSTR("match\n"));
                    eeprom_update_block(pwr_on_code, eeprom, 1+c);
                    led_on();
                    _delay_ms(200);
                    led_off();
                    _delay_ms(200);
                    led_on();
                    _delay_ms(200);
                    led_off();
                    eeprom_busy_wait();
                    dbg_print_str_P(PSTR("saved\n"));
                } else {
                    dbg_print_str_P(PSTR("not match\n"));
                    led_on();
                    _delay_ms(100);
                    led_off();
                    _delay_ms(100);
                    led_on();
                    _delay_ms(100);
                    led_off();
                    _delay_ms(100);
                    led_on();
                    _delay_ms(100);
                    led_off();
                    _delay_ms(100);
                    led_on();
                    _delay_ms(100);
                    led_off();
                }
            } else if (learn_mode) {
                learn_mode = 0;
            }
            dbg_print_str_P(PSTR("fmt: "));
            dbg_print_num(IrCtrl.fmt, 2);
            dbg_nl();
            dbg_print_str_P(PSTR("data: "));
            dbg_print_num(l, 3);
            for (i = 0; i < c; i++) {
                dbg_print_char(',');
                dbg_print_hex(IrCtrl.rxdata[i], 2);
            }
            dbg_nl();
            if (!learn_mode && pwr_on_code[0] == l && memcmp(pwr_on_code+1, (void*)IrCtrl.rxdata, c) == 0) {
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
