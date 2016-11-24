#ifndef DS12887_H
#define DS12887_H

#include "bitmanip.h"

#define RTC_IRQ_LINE	8

// Ports
#define RTC_ADDR_REG	0x70
#define RTC_DATA_REG	0x71

// Address Locations
#define RTC_SECONDS 		0
#define RTC_SECONDS_ALARM 	1
#define RTC_MINUTES			2
#define RTC_MINUTES_ALARM	3
#define RTC_HOURS			4
#define RTC_HOURS_ALARM		5
#define RTC_DAY_OF_WEEK		6
#define RTC_DAY_OF_MONTH	7
#define RTC_MONTH			8
#define RTC_YEAR			9
#define RTC_REGISTER_A		10
#define RTC_REGISTER_B		11
#define RTC_REGISTER_C		12
#define RTC_REGISTER_D		13

// Register A
#define RTC_A_RS	BIT(0) | BIT(1) | BIT(2) | BIT(3)
#define RTC_A_DV	BIT(4) | BIT(5) | BIT(6)
#define RTC_A_UIP	BIT(7)

// Register B
#define RTC_B_DSE		BIT(0)
#define RTC_B_24_12		BIT(1)
#define RTC_B_DM		BIT(2)
#define RTC_B_SQWE		BIT(3)
#define RTC_B_UIE		BIT(4)
#define RTC_B_AIE		BIT(5)
#define RTC_B_PIE		BIT(6)
#define RTC_B_SET		BIT(7)

// Register C
#define RTC_C_UF		BIT(4)
#define RTC_C_AF		BIT(5)
#define RTC_C_PF		BIT(6)
#define RTC_C_IRQF		BIT(7)
// Other bits set to 0

// Register D
#define RTC_D_VRT		BIT(7)
// Other bits set to 0

#endif
