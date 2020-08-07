/*------------------------------------------------------------------------------------
 * 项目名称:

     TM1638模块驱动程序v1.0
 
 * 版权信息:

     (c) 飞翼电子, 2014.

 * 历史版本:
     2014-11-8:
       - 初始版本 V1.0.0;

 * 备注:
     - 使用前需要在TM1638.h文件设置通信接口、级联个数和初始化数据
-------------------------------------------------------------------------------------*/

/************************包含头文件***************************************************/
#include	"STC15Fxxxx.H"
#include "TM1668.h"
#include "plat.h"
#include	"config.h"
/************************编码表定义***************************************************/

unsigned char code Seg_test[8]=      {0x20,0x01,0x02,0x04,0x08,0x10,0x40,0x80};						 //八段数码管显示的每一段
unsigned char code Number_arr[10]=   {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};			 //共阳极数码管数字0-9编码数组
unsigned char code Character_arr[53]={0x77/*A*/,0xff,0x39/*C*/,0xff,0x79/*E*/,0x71/*F*/,0xff,0x76/*H*/,
									  0xff,0xff,0xff,0x38/*L*/,0xff,0xff,0xff,				 				 //数码管可显示字符编码表
									  0x73/*P*/,0x67/*Q*/,0xff,0xff,0xff,0x3e/*U*/,
									  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
									  0x08/*_*/,0xff,0xff,0x7c/*b*/,			 							 //包括_,A,C,E,F,H,L,P,Q,U,-,b,c,d,h,l,n,o,u
									  0x58/*c*/,0x5e/*d*/,0xff,0xff,0xff,0x74/*h*/,0xff,0xff,0xff,0x30/*l*/,0xff,
									  0x54/*n*/,0x5c/*o*/,0xff,0xff,0x50/*r*/,0xff,0xff,0x1c/*u*/};
/*************************************************************************************
*函数名：void DisplayInterfaceInit(void)
*功  能：初始化显示接口
*说  明：
**************************************************************************************/
void DisplayInterfaceInit(void)
{
	Init1668Interface();
}
/*************************************************************************************
*函数名：void NoDecodeDigiDisplay(unsigned char start,unsigned char* arr,unsigned char len)
*功  能：显示一个数组的数据,无程序解码，数组的编码由用户定义，
*说  明：start——起始位，从1-8，arr——要显示的数组，len——需要显示几位
**************************************************************************************/
void NoDecodeDigiDisplay(unsigned char start,unsigned char* arr,unsigned char len)
{
	DisplayArr_TM1668(start,arr,len);
}
/*************************************************************************************
*函数名：void NoDecodeDigiDisplay(unsigned char start,unsigned char* arr,unsigned char len)
*功  能：显示一个数组的数据,无程序解码，数组的编码由用户定义，
*说  明：digi——要显示的位置，从1-8，c——要显示的字符
**************************************************************************************/
void NoDecodeDisplayOneDigi(unsigned char digi,unsigned char c)
{
	DisplayOneDigi_TM1668(digi,c);
}
/*************************************************************************************
*函数名：void DisplayOneDigi(unsigned char digi,unsigned char cha,unsigned char dot)
*功  能：在指定位显示一个数字或字符
*说  明：digi——要显示的位置，从1-8，c——要显示的字符，dot——是否显示小数点
**************************************************************************************/
void DisplayOneDigi(unsigned char digi,unsigned char c,unsigned char dot)
{
	unsigned char tmp;
	
	if((dot&0x01)==0x01)							//判断是否显示小数点，如果dot设置为1，则显示小数点
	{
		dot=0x80;
	}
	else
	{
		dot=0;
	}
	if(c>=0&&c<=9)									//判断是否为数字
	{
		tmp=Number_arr[c]|dot;
	}
	else if(c>=65&&c<=117)							//判断是否为字符从’A'到‘u’
	{
		tmp=Character_arr[(c-65)]|dot;
	}
	else if(c==45)									//判断是否为’-‘号
	{
		tmp=0x01|dot;
	}
	else if(c==32)									//显示空白
	{
		tmp=0x00|dot;
	} 
	else if(c==61)									//判断是否为‘=’号
	{
		tmp=0x09|dot;
	}
	else if(c==46)									//判断是否为‘.’号
	{
		tmp=0x80|dot;
	}
		
	DisplayOneDigi_TM1668(digi,tmp);				//显示内容
}
/***************************************END********************************************/
