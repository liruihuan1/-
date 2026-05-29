#ifndef __ATK_NCR_H
#define __ATK_NCR_H
#define ATK_NCR_TRACEBUF1_SIZE	500*4  	//定义第一个tracebuf大小(单位为字节)
#define ATK_NCR_TRACEBUF2_SIZE	250*4	//定义第二个tracebuf大小(单位为字节)
//输入轨迹坐标类型
__packed typedef struct _atk_ncr_point
{
	short x; 	//x轴坐标
	short y;	//y轴坐标
}atk_ncr_point;		 

//外部调用函数
//初始化识别器
//返回值:0,初始化成功
//       1,初始化失败
unsigned char alientek_ncr_init(void);
//停止识别器
void alientek_ncr_stop(void);	  
//识别器识别
//track:输入点阵集合 
//potnum:输入点阵的点数,就是track的大小
//charnum:期望输出的结果数,就是你希望输出多少个匹配结果
//mode:识别模式
//1,仅识别数字
//2,仅识别大写字母
//3,仅识别小写字母
//4,混合识别(全部识别)
//result:结果缓存区(至少为:charnum+1个字节)	
void alientek_ncr(atk_ncr_point * track,int potnum,int charnum,unsigned char mode,char*result);
//内存设置函数 
void alientek_ncr_memset(char *p,char c,unsigned long len);	
//动态申请内存,当使用ATKNCR_M_Vx.x.lib时,必须实现.
void *alientek_ncr_malloc(unsigned int size);			 
//动态释放内存,当使用ATKNCR_M_Vx.x.lib时,必须实现.
void alientek_ncr_free(void *ptr);						 	    
#endif



