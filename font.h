/* $Id: font.h,v 1.1 2012/04/18 03:47:31 demon Exp $ */

#ifndef _FONT_H
#define _FONT_H

unsigned short atari_small[0x80][2] = {
	{ 0x0000, 0x0000 },
	{ 0x081c, 0x3e1c },
	{ 0x0055, 0xaa55 },
	{ 0x001f, 0x24ff },
	{ 0x1f05, 0xf928 },
	{ 0x0205, 0xf550 },
	{ 0x0f08, 0xf850 },
	{ 0x001e, 0x121e },
	{ 0x0024, 0x2e24 },
	{ 0x0f02, 0xf48f },
	{ 0x0708, 0x17f0 },
	{ 0x0808, 0x0f00 },
	{ 0x0808, 0xf800 },
	{ 0x0000, 0xf808 },
	{ 0x0000, 0x0f08 },
	{ 0x0808, 0xff08 },
	{ 0x0002, 0x0202 },
	{ 0x0004, 0x0404 },
	{ 0x0008, 0x0808 },
	{ 0x0010, 0x1010 },
	{ 0x0020, 0x2020 },
	{ 0x0000, 0xff08 },
	{ 0x0808, 0xff00 },
	{ 0x1010, 0x1f10 },
	{ 0x1010, 0xf010 },
	{ 0x0000, 0xff00 },
	{ 0x0048, 0x5462 },
	{ 0x0062, 0x5448 },
	{ 0x023e, 0x023e },
	{ 0x1434, 0x1c16 },
	{ 0x247e, 0x1531 },
	{ 0x0000, 0x0800 },
	{ 0x0000, 0x0000 },
	{ 0x005e, 0x0000 },
	{ 0x0600, 0x0600 },
	{ 0x7e24, 0x7e00 },
	{ 0x24cb, 0x3400 },
	{ 0x6218, 0x4600 },
	{ 0x3a65, 0x5a00 },
	{ 0x0006, 0x0000 },
	{ 0x003c, 0x4200 },
	{ 0x423c, 0x0000 },
	{ 0x2a1c, 0x2a00 },
	{ 0x083e, 0x0800 },
	{ 0x8060, 0x0000 },
	{ 0x0808, 0x0800 },
	{ 0x0060, 0x0000 },
	{ 0x6018, 0x0600 },
	{ 0x3c4a, 0x3c00 },
	{ 0x447e, 0x4000 },
	{ 0x6452, 0x4c00 },
	{ 0x224a, 0x3600 },
	{ 0x1814, 0x7e00 },
	{ 0x2e4a, 0x3200 },
	{ 0x3c4a, 0x3200 },
	{ 0x0272, 0x0e00 },
	{ 0x344a, 0x3400 },
	{ 0x4c52, 0x3c00 },
	{ 0x0024, 0x0000 },
	{ 0x4024, 0x0000 },
	{ 0x1028, 0x4400 },
	{ 0x1414, 0x1400 },
	{ 0x4428, 0x1000 },
	{ 0x0452, 0x0c00 },
	{ 0x3c42, 0x4c00 },
	{ 0x7c12, 0x7c00 },
	{ 0x7e4a, 0x3400 },
	{ 0x3c42, 0x2400 },
	{ 0x7e42, 0x3c00 },
	{ 0x7e4a, 0x4a00 },
	{ 0x7e0a, 0x0a00 },
	{ 0x3c42, 0x3400 },
	{ 0x7e08, 0x7e00 },
	{ 0x427e, 0x4200 },
	{ 0x2040, 0x3e00 },
	{ 0x7e08, 0x7600 },
	{ 0x7e40, 0x4000 },
	{ 0x7e04, 0x7e00 },
	{ 0x7c18, 0x3e00 },
	{ 0x3c42, 0x3c00 },
	{ 0x7e12, 0x0c00 },
	{ 0x3c62, 0x5c00 },
	{ 0x7e12, 0x6c00 },
	{ 0x244a, 0x3400 },
	{ 0x027e, 0x0200 },
	{ 0x7e40, 0x7e00 },
	{ 0x3e40, 0x3e00 },
	{ 0x7e20, 0x7e00 },
	{ 0x7608, 0x7600 },
	{ 0x0678, 0x0600 },
	{ 0x625a, 0x4600 },
	{ 0x007e, 0x4200 },
	{ 0x0618, 0x6000 },
	{ 0x427e, 0x0000 },
	{ 0x0402, 0x0400 },
	{ 0x8080, 0x8080 },
	{ 0x0204, 0x0000 },
	{ 0x2454, 0x7800 },
	{ 0x7e48, 0x3000 },
	{ 0x3844, 0x4400 },
	{ 0x3048, 0x7e00 },
	{ 0x3854, 0x5800 },
	{ 0x087c, 0x0a00 },
	{ 0x98a4, 0x7c00 },
	{ 0x7e08, 0x7000 },
	{ 0x487a, 0x4000 },
	{ 0x8088, 0x7a00 },
	{ 0x7e10, 0x6800 },
	{ 0x007e, 0x0000 },
	{ 0x7c08, 0x7c00 },
	{ 0x7c04, 0x7800 },
	{ 0x3844, 0x3800 },
	{ 0xfc24, 0x1800 },
	{ 0x1824, 0xfc00 },
	{ 0x7804, 0x0400 },
	{ 0x4854, 0x2400 },
	{ 0x087e, 0x0800 },
	{ 0x7c40, 0x7c00 },
	{ 0x3c40, 0x3c00 },
	{ 0x7c20, 0x7c00 },
	{ 0x6c10, 0x6c00 },
	{ 0x9ca0, 0x7c00 },
	{ 0x6454, 0x4c00 },
	{ 0x0836, 0x4100 },
	{ 0x0077, 0x0000 },
	{ 0x4136, 0x0800 },
	{ 0x040c, 0x0800 },
	{ 0x0000, 0x0000 },
};

#endif
