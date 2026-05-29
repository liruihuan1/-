#include "touch.h" 
#include "lcd.h"
#include "delay.h"
#include "gt9147.h"     

// 触摸屏设备驱动结构体实例化
_m_tp_dev tp_dev =
{
    TP_Init,            // 初始化函数
    GT9147_Scan         // 扫描函数
    // 其余的 x数组, y数组, sta, touchtype 会由编译器自动初始化为 0
};					
 	 			    					   
// 触摸屏初始化  		    
// 返回值: 0, 成功; 1, 失败 
u8 TP_Init(void)
{	
    // 直接调用 GT9147 的底层初始化
    if (GT9147_Init() == 0) 
    {
        tp_dev.touchtype |= 0X80;              // 最高位置1，代表这是一块电容屏 
        tp_dev.touchtype |= lcddev.dir & 0X01; // 绑定横竖屏方向状态
        return 0;                              // 初始化成功
    }
    return 1;                                  // 初始化失败
}