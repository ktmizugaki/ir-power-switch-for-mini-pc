#include <avr/pgmspace.h>
#include <stdint.h>
#include "uart.h"
#undef NDEBUG
#include "dbg.h"

#define xmit   uart_xmit

void dbg_init(void)
{
    uart_start(19200);
}

void dbg_nl(void)
{
    xmit('\r');
    xmit('\n');
}

void dbg_print_char(unsigned char c)
{
    xmit(c);
}

void dbg_print_str(const char *str)
{
    char c;
    while((c=*str++)) {
        xmit(c);
    }
}

void dbg_print_str_P(const char *str)
{
    char c;
    while((c=pgm_read_byte(str++))) {
        xmit(c);
    }
}

void dbg_print_num(int value, unsigned char digits)
{
    char work[5];
    unsigned char i;
    if (value < 0) {
        xmit('-');
        value = -value;
    }
    for (i = 0; i < digits; i++) {
        if (i == 0 || value != 0) {
            work[i] = '0' + value%10;
        } else {
            work[i] = ' ';
        }
        value /= 10;
    }
    do {
        xmit(work[--i]);
    } while (i > 0);
}

void dbg_print_num_ex(long value, unsigned char digits)
{
    char work[10];
    unsigned char i;
    if (value < 0) {
        xmit('-');
        value = -value;
    }
    for (i = 0; i < digits; i++) {
        if (i == 0 || value != 0) {
            work[i] = '0' + value%10;
        } else {
            work[i] = ' ';
        }
        value /= 10;
    }
    do {
        xmit(work[--i]);
    } while (i > 0);
}

static char ntohchar(uint8_t h)
{
    if (h < 10) {
        return '0'+h;
    } else {
        return 'A'+h-10;
    }
}

void dbg_print_hex(unsigned short value, unsigned char digits)
{
    value = value<<(sizeof(value)*8-digits*4);
    while (digits) {
        xmit(ntohchar(value>>(sizeof(value)*8-4)));
        value <<= 4;
        digits--;
    }
}

void dbg_print_bits(unsigned char value, unsigned char digits)
{
    value = value<<(8-digits);
    while (digits) {
        xmit((value&0x80)? '1': '0');
        value <<= 1;
        digits--;
    }
}
