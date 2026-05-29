#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "sys.h"
#include "gt9147.h"	 // 保留 GT9147 芯片头文件

#define TP_PRES_DOWN 0x80  		// 触屏被按下标志位	  
#define TP_CATH_PRES 0x40  		// 发生过按键按下的标志位 
#define CT_MAX_TOUCH  5    		// GT9147 电容屏支持的点数, 固定为 5 点

// 电容触摸屏控制器结构体
typedef struct 
{
	u8 (*init)(void);			// 初始化触摸屏底层驱动
	u8 (*scan)(u8);				// 扫描触摸屏坐标
	
	u16 x[CT_MAX_TOUCH]; 		// 承接多点触控的 X 坐标数组
	u16 y[CT_MAX_TOUCH];		// 承接多点触控的 Y 坐标数组
	
	u8  sta;					// 笔的状态 
								// b7: 按下1 / 松开0; 
	                            // b6: 0,没有按键按下; 1,有按键按下. 
								// b5: 保留
								// b4~b0: 电容触摸屏按下的点数(0表示未按下，>0表示按下的手指数量)
	   
	u8 touchtype;               // 屏幕类型与方向标志
                                // b0: 0,竖屏; 1,横屏
                                // b7: 0,电阻屏; 1,电容屏 (本项目中该位将固定为 1)
}_m_tp_dev;

extern _m_tp_dev tp_dev;	 	// 触屏控制器实例化对象 (在 touch.c 里面定义)

#endif