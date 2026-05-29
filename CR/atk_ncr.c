#include "atk_ncr.h"
#include "malloc.h"					   				
//内存设置函数
void alientek_ncr_memset(char *p,char c,unsigned long len) 
{
	mymemset((u8*)p,(u8)c,(u32)len);
}	 					  
//内存申请函数
void *alientek_ncr_malloc(unsigned int size) 
{
	return mymalloc(SRAMIN,size);
}
//内存清空函数
void alientek_ncr_free(void *ptr) 
{
	myfree(SRAMIN,ptr);
}		  
		  



















