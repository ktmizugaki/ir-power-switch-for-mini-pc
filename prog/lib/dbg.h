#ifndef LIB_DBG_H
#define LIB_DBG_H

#if defined(NDEBUG)

#define dbg_init()                  ((void)0)
#define dbg_nl()                    ((void)0)
#define dbg_print_char(c)           ((void)0)
#define dbg_print_str(str)          ((void)0)
#define dbg_print_str_P(str)        ((void)0)
#define dbg_print_num(value, digits)  ((void)0)
#define dbg_print_hex(value, digits)  ((void)0)
#define dbg_print_bits(value, digits) ((void)0)

#else /* !NDEBUG */

extern void dbg_init(void);
extern void dbg_nl(void);
extern void dbg_print_char(unsigned char c);
extern void dbg_print_str(const char *str);
extern void dbg_print_str_P(const char *str);
extern void dbg_print_num(int value, unsigned char digits);
extern void dbg_print_num_ex(long value, unsigned char digits);
extern void dbg_print_hex(unsigned short value, unsigned char digits);
extern void dbg_print_bits(unsigned char value, unsigned char digits);

#endif

#endif /* LIB_DBG_H */
