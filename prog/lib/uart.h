#ifndef LIB_UART_H
#define LIB_UART_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#if defined(UCSR0A)
#define UART_HARD   1
#else
#include "pindesc.h"
#define UART_HARD   0
/* Software UART baudrate. */
#ifndef UART_BAUDRATE
#define UART_BAUDRATE   19200
#endif
#endif

#if defined(__AVR_ATmega328P__)
#define UART_TX_DESC    D,1
#define UART_RX_DESC    D,0
#endif
#if defined(__AVR_ATtiny84__)
#define UART_TX_DESC    A,0
#define UART_RX_DESC    A,1
#endif
#if defined(__AVR_ATtiny85__)
#define UART_TX_DESC    B,4
#define UART_RX_DESC    B,3
#endif

#if UART_HARD
static inline void uart_start(unsigned int baudrate)
{
    /*
     * UCSR0A = RXC0    TXC0    UDRE0  FE0   DOR0  UPE0   U2X0   MPCM0
     * UCSR0B = RXCIE0  TXCIE0  UDRIE0 RXEN0 TXEN0 UCSZ02 RXB80  TXB80
     * UCSR0C = UMSEL01 UMSEL00 UPM01  UPM00 USBS0 UCSZ01 UCSZ00 UCPOL0
     * UMSEL0[1:0] = 0: Asynchronous USART.
     * UCSZ0[2:0] = 3: 8bit.
     * UPM0[1:0] = 0: Parity mode disabled.
     * USBS0 = 0: 1 stop bit
     */
    PRR &= ~_BV(PRUSART0);
    UBRR0 = (F_CPU/16/baudrate -1);
    UCSR0B = _BV(RXEN0)|_BV(TXEN0);
    UCSR0C = _BV(UCSZ01)|_BV(UCSZ00);
}

static inline void uart_stop(void)
{
    PRR |= _BV(PRUSART0);
}

static inline bool uart_can_xmit()
{
    return (UCSR0A&_BV(UDRE0));
}

static inline void uart_xmit(uint8_t data)
{
    while (!uart_can_xmit())
        ;
    UCSR0A |= _BV(TXC0);
    UDR0 = data;
}

static inline void uart_wait_xmit(void)
{
    while (!uart_can_xmit() || !(UCSR0A&_BV(TXC0)))
        ;
}

static inline bool uart_can_recv()
{
    return (UCSR0A&_BV(RXC0));
}

static inline uint16_t uart_recv(void)
{
    if (!uart_can_recv())
        return 0x0100;
    return (uint16_t)(uint8_t)UDR0;
}
#else
static inline void uart_start(unsigned int baudrate)
{
    /* software uart baudrate is fixed to UART_BAUDRATE. */
    (void) baudrate;
    PINDESC_PULLUP_EN(UART_TX_DESC);
    PINDESC_SET_OUTPUT(UART_TX_DESC);
#if 0
    /* No RX support yet */
    PINDESC_PULLUP_EN(UART_RX_DESC);
    PINDESC_SET_INPUT(UART_RX_DESC);
#endif
}

static inline void uart_stop(void)
{
    PINDESC_SET_INPUT(UART_TX_DESC);
}

static inline uint8_t uart_can_xmit()
{
    return 1;
}

extern void uart_xmit(uint8_t data);

static inline void uart_wait_xmit(void)
{
}

static inline uint8_t uart_can_recv(void)
{
    return 1;
}

extern uint16_t uart_recv(void);
#endif

extern void uart_init_stdio(void);
extern void uart_puts(const char *str);
extern void uart_puts_P(const char *str);

#endif /* LIB_UART_H */
