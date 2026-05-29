#include "delay.h"

static u8  fac_us = 0; // us延时倍乘数			   
static u16 fac_ms = 0; // ms延时倍乘数

//初始化延迟函数
//当使用72Mhz的时钟时，SysTick时钟为9Mhz (HCLK/8)
void delay_init(void)
{
    // 选择外部时钟，即系统时钟的1/8（72MHz / 8 = 9MHz）
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    
    fac_us = SystemCoreClock / 8000000; // 72M/8M = 9。即每经过1微秒，SysTick需要数9下。
    fac_ms = (u16)fac_us * 1000;        // 毫秒的倍乘数
}

//延时 nus 微秒
//nus: 要延时的微秒数
void delay_us(u32 nus)
{		
    u32 temp;	    	 
    SysTick->LOAD = nus * fac_us; 					// 时间加载，设定位数	  		 
    SysTick->VAL = 0x00;        					// 清空当前计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;       // 开始倒数	  
    do
    {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));  // 等待时间到达（第16位被硬件置1说明数完了）
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      // 关闭计数器
    SysTick->VAL = 0X00;                            // 清空当前计数器	 
}

//延时 nms 毫秒
//nms: 要延时的毫秒数
void delay_ms(u16 nms)
{	 		  	  
    u32 temp;		   
    SysTick->LOAD = (u32)nms * fac_ms;				// 时间加载(SysTick->LOAD为24bit)
    SysTick->VAL = 0x00;							// 清空当前计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;       // 开始倒数  
    do
    {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));  // 等待时间到达
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      // 关闭计数器
    SysTick->VAL = 0X00;       						// 清空当前计数器	  	    
}