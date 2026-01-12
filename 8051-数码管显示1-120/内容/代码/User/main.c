/* 头文件声明区域 */
#include <STC15F2K60S2.H>
#include <Seg.h>

/* 用户变量声明区域 */
unsigned int Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[] = {10, 10, 10, 10, 10, 10, 10, 10};
unsigned char index;
unsigned int Timer1000ms;

/* 信息处理函数 */
void Seg_Proc() {
  if (Seg_Slow_Down)
    return;
  Seg_Slow_Down = 1;

  Seg_Buf[7] = index;
}

/* 定时器初始化 */
void Timer0Init(void) // 1毫秒@12.000MHz
{
  AUXR &= 0x7F; // 定时器时钟12T模式
  TMOD &= 0xF0; // 设置定时器模式
  TL0 = 0x18;   // 设置定时初值
  TH0 = 0xFC;   // 设置定时初值
  TF0 = 0;      // 清除TF0标志
  TR0 = 1;      // 定时器0开始计时
  ET0 = 1;
  EA = 1;
}

/* 中断服务函数*/
void Timer0Server() interrupt 1 {
  if (++Timer1000ms == 1000) {
    Timer1000ms = 0;
    if (++index == 10)
      index = 0;
  }
  if (++Seg_Slow_Down == 500)
    Seg_Slow_Down = 0;

  if (++Seg_Pos == 8)
    Seg_Pos = 0;
  Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos]);
}

void Delay750ms() //@12.000MHz
{
  unsigned char i, j, k;
  i = 35;
  j = 51;
  k = 182;
  do {
    do {
      while (--k)
        ;
    } while (--j);
  } while (--i);
}

/* 用户主函数 */
int main() {
  Timer0Init();

  System_Init();
  while (1) {
    Seg_Proc();
  }
}