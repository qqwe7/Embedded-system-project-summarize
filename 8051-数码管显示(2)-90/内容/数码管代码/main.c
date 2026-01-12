#include "reg51.h"

#define uchar unsigned char
#define uint unsigned int

// 数码管段码表 (0-9)
uchar code smgduan[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
                        0x6d, 0x7d, 0x07, 0x7f, 0x6f};

void delay(uint i) {
  while (i--)
    ;
}

void main() {
  uchar i;
  while (1) {
    for (i = 0; i < 10; i++) {
      // 发送段码到P0
      P0 = smgduan[i];

      P2 = 0xFE; // 1111 1110 -> P2.0=0, P2.1=1

      delay(60000);
      delay(20000); // 延时可见
    }
  }
}
