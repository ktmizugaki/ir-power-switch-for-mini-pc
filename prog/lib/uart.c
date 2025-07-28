#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"

#if !UART_HARD
void uart_xmit(uint8_t c)
{
#define UART_TX_NUM_DELAY_CYCLES    ((F_CPU/UART_BAUDRATE-16)/4+1)
#define UART_TX_NUM_ADD_NOP         ((F_CPU/UART_BAUDRATE-16)%4)
    uint8_t sreg;
    uint16_t tmp;
    uint8_t numiter = 10;

    sreg = SREG;
    cli();

    asm volatile (
        /* put the START bit */
        "in %A0, %3"            "\n\t"  /* 1 */
        "cbr %A0, %4"           "\n\t"  /* 1 */
        "out %3, %A0"           "\n\t"  /* 1 */
        /* compensate for the delay induced by the loop for the
         * other bits */
        "nop"                   "\n\t"  /* 1 */
        "nop"                   "\n\t"  /* 1 */
        "nop"                   "\n\t"  /* 1 */
        "nop"                   "\n\t"  /* 1 */
        "nop"                   "\n\t"  /* 1 */

        /* delay */
   "1:" "ldi %A0, lo8(%5)"      "\n\t"  /* 1 */
        "ldi %B0, hi8(%5)"      "\n\t"  /* 1 */
   "2:" "sbiw %A0, 1"           "\n\t"  /* 2 */
        "brne 2b"               "\n\t"  /* 1 if EQ, 2 if NEQ */
#if UART_TX_NUM_ADD_NOP > 0
        "nop"                   "\n\t"  /* 1 */
#if UART_TX_NUM_ADD_NOP > 1
        "nop"                   "\n\t"  /* 1 */
#if UART_TX_NUM_ADD_NOP > 2
        "nop"                   "\n\t"  /* 1 */
#endif
#endif
#endif
        /* put data or stop bit */
        "in %A0, %3"            "\n\t"  /* 1 */
        "sbrc %1, 0"            "\n\t"  /* 1 if false,2 otherwise */
        "sbr %A0, %4"           "\n\t"  /* 1 */
        "sbrs %1, 0"            "\n\t"  /* 1 if false,2 otherwise */
        "cbr %A0, %4"           "\n\t"  /* 1 */
        "out %3, %A0"           "\n\t"  /* 1 */

        /* shift data, putting a stop bit at the empty location */
        "sec"                   "\n\t"  /* 1 */
        "ror %1"                "\n\t"  /* 1 */

        /* loop 10 times */
        "dec %2"                "\n\t"  /* 1 */
        "brne 1b"               "\n\t"  /* 1 if EQ, 2 if NEQ */
        : "=&w" (tmp),                  /* scratch register */
          "=r" (c),                     /* we modify the data byte */
          "=r" (numiter)                /* we modify number of iter.*/
        : "I" (_SFR_IO_ADDR(DESC2PORT(UART_TX_DESC))),
          "M" DESC2BV(UART_TX_DESC),
          "i" (UART_TX_NUM_DELAY_CYCLES),
          "1" (c),                      /* data */
          "2" (numiter)
    );
    SREG = sreg;
#undef UART_TX_NUM_DELAY_CYCLES
#undef UART_TX_NUM_ADD_NOP
}

uint16_t uart_recv(void)
{
    /* not implemented */
    return EOF;
}
#endif

static int uart_putchar(char c, FILE *stream)
{
    (void)stream;
    uart_xmit(c);
    return 0;
}

static int uart_getchar(FILE *stream)
{
    (void)stream;
    return uart_recv();
}

static FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void uart_init_stdio(void)
{
    stdin = &uart_io;
    stdout = &uart_io;
    stderr = &uart_io;
}

void uart_puts(const char *str)
{
    fputs(str, &uart_io);
}

void uart_puts_P(const char *str)
{
    fputs_P(str, &uart_io);
}
