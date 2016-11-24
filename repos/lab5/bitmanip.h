#ifndef BITMANIP_H
#define BITMANIP_H

#define BIT(n) (0x01 << n)
#define CLEAR_BIT(src, n) ((src) & ~BIT(n))
#define TOGGLE_BIT(src, bit) ((src) ^ BIT(bit))
#define SET_BYTE(n) 			(0xFF << (8 * (n)))
#endif // BITMANIP_H
