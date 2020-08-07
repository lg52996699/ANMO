/*-------------------------------------------------------------------------------------
 * 项目名称:

     TM1668模块驱动程序v1.0

 * 版权信息:

     (c) 飞翼电子, 2016.

 * 历史版本:
     2016-5-11:
       - 初始版本 V1.0.0;

 * 备注:
     - 使用前需要在下面的接口设置部分设置通信接口
	 - 在串联个数设置区设置级联的个数，如果只一块则设置为1，以此类推

----------------------------------------------------------------------------------------*/

#ifndef _PLAT_H_
#define _PLAT_H_

/*---------------------------声明数组---------------------------------------------------*/

extern unsigned char code Seg_test[8];			//八段数码管的每一段
extern unsigned char code Dign[8];				//对位的译码数组
extern unsigned char code Number_arr[10];		//数字0-9到七段数码管的编码数组
extern unsigned char code Character_arr[53];	//数码管可显示字符到八段数码管的编码数组，
												//包括_,A,C,E,F,H,L,P,Q,U,-,b,c,d,h,l,n,o,u

/*---------------------------函数声明-----------------------------------------------------*/

void DisplayInterfaceInit(void);
void NoDecodeDigiDisplay(unsigned char start,unsigned char* arr,unsigned char len);
void NoDecodeDisplayOneDigi(unsigned char digi,unsigned char c);
void DisplayOneDigi(unsigned char digi,unsigned char c,unsigned char dot);
#endif