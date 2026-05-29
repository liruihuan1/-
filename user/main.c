#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "lcd.h"  
#include "key.h"      
#include "malloc.h"
#include "atk_ncr.h"
#include "touch.h"	 

//最大记录的轨迹点数
atk_ncr_point READ_BUF[200];					 

//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}

//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  

//两个数之差的绝对值 
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  

//画一条粗线
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

int main(void)
{	 
	u8 i=0;	    	
	u8 tcnt=0;    
	u8 res[10];
	u8 key;		    
	u16 pcnt=0;
	u8 mode=4;					//默认是混合模式	    	    
	u16 lastpos[2];				//最后一次的数据 
	delay_init();	    	    //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2
	uart_init(115200);	 	    //串口初始化为115200
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD     
	tp_dev.init();				//初始化触摸屏
	my_mem_init(SRAMIN);		//初始化内部内存池
	alientek_ncr_init();		//初始化手写识别算法库
	
	POINT_COLOR=RED;       

	// 重写为纯英文界面，直接调用 lcd.c 自带的英文显示函数
	LCD_ShowString(30, 10, 200, 16, 16, (u8*)"STM32 Board"); 				    	 
	LCD_ShowString(30, 30, 200, 16, 16, (u8*)"Handwriting Recog"); 			    	 
	LCD_ShowString(30, 50, 200, 16, 16, (u8*)"CR"); 				    	 
	LCD_ShowString(30, 70, 200, 16, 16, (u8*)"KEY0:MODE WKUP:Adj"); 			    
	LCD_ShowString(30, 90, 200, 16, 16, (u8*)"Result:"); 		
	
	LCD_DrawRectangle(19, 114, lcddev.width-20, lcddev.height-5);
	POINT_COLOR=BLUE;      
	LCD_ShowString(96, 207, 200, 16, 16, (u8*)"Draw Area"); 
	
	tcnt=100;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)	
		{
			LCD_Fill(20,115,219,314,WHITE);//清除当前显示
			mode++;
			if(mode>4)mode=1;
			switch(mode)
			{
				case 1:
					LCD_ShowString(64, 207, 200, 16, 16, (u8*)"Numbers Only   "); 
					break;	 	    
				case 2:
					LCD_ShowString(64, 207, 200, 16, 16, (u8*)"Uppercase Only "); 	
					break;	 	    
				case 3:
					LCD_ShowString(64, 207, 200, 16, 16, (u8*)"Lowercase Only "); 	
					break;	 	    
				case 4:
					LCD_ShowString(64, 207, 200, 16, 16, (u8*)"All Characters "); 
					break;	 
			}
			tcnt=100;
		}		   
 		tp_dev.scan(0);//扫描
 		if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
		{				  
			delay_ms(3);//必要的延时,否则老认为有按键按下.
 			tcnt=0;//松开时的计数器清空 	 		    
			if((tp_dev.x[0]<(lcddev.width-20-2)&&tp_dev.x[0]>=(20+2))&&(tp_dev.y[0]<(lcddev.height-5-2)&&tp_dev.y[0]>=(115+2)))
			{			 
				if(lastpos[0]==0XFFFF)
				{
					lastpos[0]=tp_dev.x[0];
					lastpos[1]=tp_dev.y[0];
				}
				lcd_draw_bline(lastpos[0],lastpos[1],tp_dev.x[0],tp_dev.y[0],2,BLUE);//画线
				lastpos[0]=tp_dev.x[0];
				lastpos[1]=tp_dev.y[0];
				if(pcnt<200)//总点数少于200
				{
					if(pcnt)
					{
						if((READ_BUF[pcnt-1].y!=tp_dev.y[0])&&(READ_BUF[pcnt-1].x!=tp_dev.x[0]))//x,y不相等
						{
							READ_BUF[pcnt].x=tp_dev.x[0];
							READ_BUF[pcnt].y=tp_dev.y[0]; 
							pcnt++;
						}	
					}else 
					{
						READ_BUF[pcnt].x=tp_dev.x[0];
						READ_BUF[pcnt].y=tp_dev.y[0]; 
						pcnt++;
					}		  
				}   						  				  
			}    
		}else //按键松开了 
		{
			lastpos[0]=0XFFFF;
			tcnt++;
			delay_ms(10);	  
			//延时识别
			i++;	 	    
			if(tcnt==40)
			{
				if(pcnt)//有有效的输入		 
				{
					printf("Points:%d\r\n",pcnt);
					alientek_ncr(READ_BUF,pcnt,6,mode,(char*)res);
					printf("Result:%s\r\n",res);
					pcnt=0;	 			    			   
	  				POINT_COLOR=BLUE;//设置画笔蓝色
		 			LCD_ShowString(30+56,90,200,16,16,res);	// 调整了X坐标，适应英文"Result:"的长度
				}
				LCD_Fill(20,115,lcddev.width-20-1,lcddev.height-5-1,WHITE);
			} 
		}  
		if(i==30)
		{
			i=0;
			
		}		   
	}       										    			    
}