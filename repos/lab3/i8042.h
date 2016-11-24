#ifndef I8042_H
#define I8042_H

#include "bitmanip.h"

#define KBD_IRQ_LINE 1
#define DELAY_US 20000

// Ports
#define KBC_STATUS_PORT	0x64
#define KBD_CMD_PORT	0x60
#define KBD_ARGS_PORT	0x60

#define KBC_OUT_PORT	0x60
#define KBC_CMD_PORT 	0x64
#define KBC_ARGS_PORT 	0x60
#define KBC_IN_BUF		0x60
#define KBC_OUT_BUF		0x60

// KBC Status Byte
#define OUT_BUF_FULL 	BIT(0)
#define IN_BUF_FULL 	BIT(1)
#define SYS_FLAG 		BIT(2)
#define CMD_DATA 		BIT(3)
#define KEYBD_INH 		BIT(4)
#define TRANS_TMOUT 	BIT(5)
#define RCV_TMOUT 		BIT(7)
#define PARITY_EVEN 	BIT(8)

// KBC Commands
#define KBC_READ_CMD_BYTE 	0x20
#define KBC_WRITE_CMD_BYTE  0x60
#define KBC_SELF_CHECK		0xAA
#define KBC_CHECK_KBD_ITF	0xAB
#define KBC_DISABLE_KBD		0xAD
#define KBC_ENABLE_KBD		0xAE
#define KBC_READ_IN_PORT	0xC0
#define KBC_READ_OUT_PORT	0xD0
#define KBC_WRITE_OUT_PORT 	0xD1
#define KBC_READ_TEST_IN	0xE0
#define KBC_SYS_RESET		0xFE

// KBC Command Byte
#define KBD_IRQ_ENABLE		BIT(0)
#define MOUSE_IRQ_ENABLE	BIT(1)
#define KBC_SYS_FLAG		BIT(2)
#define KBD_IGNORE_LOCK		BIT(3)
#define KBD_DISABLE			BIT(4)
#define MOUSE_DISABLE		BIT(5)
#define KBD_SET_TRANSLATION	BIT(6)
// BIT(7) not used

// KBD Commands
#define KBD_LED_CMD 0xED

// KBD responses
#define KBD_ACK 	0xFA
#define KBD_ERROR 	0xFC
#define KBD_NOT_ACK	0xFE

// Keys
#define KBD_KEY_BREAK 0x80
#define KBD_KEY_ESC 0x01
#define KBD_KEY_2BYTE 0xE0

// LEDs
#define KBD_LED_SCROLL 0
#define KBD_LED_NUM 1
#define KBD_LED_CAPS 2

#endif
