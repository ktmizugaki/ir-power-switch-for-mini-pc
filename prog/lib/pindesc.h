#ifndef LIB_PIN_DESC_H
#define LIB_PIN_DESC_H

/*
 usage:

#define PIN_DESC    A,0

DESC2PORT(PIN_DESC) |= DESC2BV(PIN_DESC));
if (DESC2PIN(PIN_DESC) & DESC2BV(PIN_DESC)) ...;

 */

/* port x data register */
#define __DESC2PORT(x, n)   PORT ## x
#define DESC2PORT(...)      __DESC2PORT(__VA_ARGS__)
/* port x data direction register */
#define __DESC2DDR(x, n)    DDR ## x
#define DESC2DDR(...)       __DESC2DDR(__VA_ARGS__)
/* port x input pins address */
#define __DESC2PIN(x, n)    PIN ## x
#define DESC2PIN(...)       __DESC2PIN(__VA_ARGS__)

#define __DESC2POS(x, n)    (n)
#define DESC2POS(...)       __DESC2POS(__VA_ARGS__)
#define DESC2BV(...)        _BV(DESC2POS(__VA_ARGS__))

#define PINDESC_SET_INPUT(...)      DESC2DDR(__VA_ARGS__) &= ~DESC2BV(__VA_ARGS__)
#define PINDESC_SET_OUTPUT(...)     DESC2DDR(__VA_ARGS__) |= DESC2BV(__VA_ARGS__)
#define PINDESC_SET_LOW(...)        DESC2PORT(__VA_ARGS__) &= ~DESC2BV(__VA_ARGS__)
#define PINDESC_SET_HIGH(...)       DESC2PORT(__VA_ARGS__) |= DESC2BV(__VA_ARGS__)
#define PINDESC_PULLUP_EN(...)      DESC2PORT(__VA_ARGS__) |= DESC2BV(__VA_ARGS__)
#define PINDESC_PULLUP_DIS(...)     DESC2PORT(__VA_ARGS__) &= ~DESC2BV(__VA_ARGS__)
#define PINDESC_TOGGLE(...)         DESC2PIN(__VA_ARGS__) |= DESC2BV(__VA_ARGS__)
#define PINDESC_GET(...)            (DESC2PIN(__VA_ARGS__) & DESC2BV(__VA_ARGS__))

#endif /* LIB_PIN_DESC_H */
