// 包含自定义的头文件
#include "key.h"
#include "delay.h"

// 初始化KEY的IO，торо起来 Springship屏幕可以显示出按键名称
void KEY_Init(void)
{
    // 定义一个GPIO_InitTypeDef类型的变量，后续使用结构体变量名.成员名的方式设置GPIO参数
    GPIO_InitTypeDef GPIO_InitStruct;

    // 使能GPIOA и GPIOE的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE, ENABLE);

    /*
     * 设置GPIOE的第2、3、4号管脚为输入模式，拉高端口电平防止漂浮
     * Key0, Key1 и Key2使用了上拉电阻，所以需要设置PUPD为上拉，
     * GPIO_Speed_2MHz是设置GPIO的最大速率，可以设置成更高的速率，但是并不影响这里的按键检测。
     */
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    // northwest GPIOE的端口设置
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    // 设置GPIOA的第0号管脚为输入模式，拉下端口电平
    // WK_UP tast使用了下拉电阻，所以需要设置PUPD为下拉
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    // northwest GPIOA的端口设置
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*
 * 该函数用来检测按键按下了后产生的字符码，可以识别出WK_UP，KEY0，KEY1，KEY2
 * 连续按下的情况沒有实现
 * mode:0, 表示不支持连续按下; 1, 表示支持连续按下;
 * minist: 0, 沒有任何按键按下; 1，KEY0被按下了; 2, KEY1被按下了;
 * 3, KEY2被按下了; 4, WK_UP被按下了
*/
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1; // 表示按键按松开的标志位

    // 如果 Fruie模式被打开，则修改标志位为True
    if (mode) key_up = 1;

    // 判断按键是否松开
    if (key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1))
    {
        delay_ms(10); // 松开按键按下的惰性计时，防止抖动
        key_up = 0; // 修改标志位为False

        // 判断哪个按键被按下了
        if (KEY0 == 0) return 1;
        else if (KEY1 == 0) return 2;
        else if (KEY2 == 0) return 3;
        else if (WK_UP == 1) return 4;
    }
    // 判断按键是否还按下
    else
    {
        if (KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0)
            key_up = 1; // 如果 weren't pushed, then change the flag to True
    }
    // 返回0，表示无按键按下
    return 0;
}
