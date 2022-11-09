#include <assert.h>

#define ROUNDS128 18

#define byteSize 300*1024*1024
#define blockSize 16

#define W0_4   0xf0000000
#define W4_8   0x0f000000
#define W8_12  0x00f00000
#define W12_16 0x000f0000
#define W16_20 0x0000f000
#define W20_24 0x00000f00
#define W24_28 0x000000f0
#define W28_32 0x0000000f
#define W0_8   0xff000000
#define W8_16  0x00ff0000
#define W16_24 0x0000ff00
#define W24_32 0x000000ff

#define W0_6   0xfe000000
#define W0_24  0xffffff80
#define W7_31  0x01ffffff
#define W25_31 0x0000007f

#define B0_4   0xf0
#define B4_8   0x0f
