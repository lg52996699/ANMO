/*------------------------------------------------------------------------------------
 * 项目名称:

     TM1668芯片驱动程序v1.0

 * 版权信息:

     (c) 飞翼电子, 2016.

 * 历史版本:
     2016-5-11:
       - 初始版本 V1.0.0;

 * 备注:
     - 使用前需要在TM1668.h文件设置通信接口、初始化数据
-------------------------------------------------------------------------------------*/

/************************包含头文件***************************************************/
#include	"STC15Fxxxx.H"
#include "TM1668.h"
#include	"config.h"

/*************************************************************************************
*函数名：void Init1668Interface(void)
*功  能：初始化1668接口电平
*说  明：DIO、CLK、STB都初始化为高电平
**************************************************************************************/
void Init1668Interface(void)
{
	STB=1;		  //先将STB置高，DIO和CLK就无效了
	DIO=1;
	CLK=1;
}
/*************************************************************************************
*函数名：void SendOneByte_TM1668(unsigned char byt)
*功  能：1668发送一个字节的数据
*说  明：低位先发送
**************************************************************************************/
void SendOneByte_TM1668(unsigned char byt)
{
	unsigned char tmp;
	unsigned char i=0;

	tmp=byt;

	for(i=0;i<8;i++)
	{
		if(tmp&0x01==1)
		{
			DIO=1;
		}
		else
		{
			DIO=0;
		}
	
		CLK=0;
		CLK=1;
		
		tmp=tmp>>1;
	}
}
/*************************************************************************************
*函数名：void SendCmd_TM1668(unsigned char cmd)
*功  能：1668发送一个命令
*说  明：低位先发送
**************************************************************************************/
void SendCmd_TM1668(unsigned char cmd)
{

	STB=1;		  //先产生一个STB的下降沿
	STB=0;

	SendOneByte_TM1668(cmd);

	STB=1;
}
/*************************************************************************************
*函数名：void SendGroupData_TM1668(unsigned char startAddr,unsigned char* arr,unsigned char len)
*功  能：1668显示地址自增模式发送一组显示数据
*说  明：低位先发送
*参  数：startAddr-显示的起始地址，arr-待显示数据的数组地址，len-要发送的数据的字节数
**************************************************************************************/
void SendGroupData_TM1668(unsigned char startAddr,unsigned char* arr,unsigned char len)
{
	unsigned char i;

	STB=1;		  							//先产生一个STB的下降沿
	STB=0;

	if((len+(0x0f&startAddr))>16)			//因为最多发十六个数据，所以先判断len是否超范围
	{
		len=16-(0x0f&startAddr);
	}

	SendOneByte_TM1668(startAddr);			//发送起始地址
		
	for(i=0;i<(len>>1);i++)
	{
		SendOneByte_TM1668(arr[i]);			//因为偶数地址对应数码管的低8段，奇数地址对应高两段
		SendOneByte_TM1668(0x00);			//一般也不会有10段数码管，所以把高两段的数据全发0
	}

	STB=1;
}

/*************************************************************************************
*函数名：void SendOneData_TM1668(unsigned char Addr,unsigned char cha)
*功  能：1668向指定地址发送一字节显示数据
*说  明：低位先发送
*参  数：Addr-显示地址，cha-待显示数据
**************************************************************************************/
void SendOneData_TM1668(unsigned char addr,unsigned char cha)
{

	STB=1;		  							//先产生一个STB的下降沿
	STB=0;

	if(addr>0x0f)							//如果地址超范围
	{
		addr=0xC0;							//如果所给的地址超范围的话，则把地址置成0，并显示‘E’
		cha=0xf9;
	}
	else									//因为1668要求显示地址的前四位必须为1100
	{
		addr=0xC0|addr;
	}

	SendOneByte_TM1668(addr);				//发送起始地址
	SendOneByte_TM1668(cha);				//发送数据

	STB=1;
}
/*************************************************************************************
*函数名：void DisplayArr_TM1668(unsigned char startAddr,unsigned char* arr,unsigned char len)
*功  能：地址自增模式按序显示一个数组的数据
*说  明：低地址字节先发送
*参  数：start-显示的起始位数，从1-8（从左到右），arr-待显示数据的数组地址，len-显示的位数
**************************************************************************************/
void DisplayArr_TM1668(unsigned char start,unsigned char* arr,unsigned char len)
{
	 unsigned char startAddr;

	 startAddr=0xFE&(0xC0|(0x0F&(start-1)<<1));	 //先计算出起始地址
	 											 //0xC0|:因为1668要求显示地址前四位必须为1100
												 //0xFE&：因为八位数码管的显示地址都是2字节对齐的
	 len*=2;									 //正常来讲，每一位对应两个字节
	 
	 SET_DISPLAY_MODE;							 //设置段、位模式
	 ADDRESS_MODE_INC;							 //发送地址模式命令
	 SendGroupData_TM1668(startAddr,arr,len);	 //发送起始地址和连续的数据
	 DISPLAY_ON(LightLevel);					 //打开显示

}
/*************************************************************************************
*函数名：void DisplayOneDigi_TM1668(unsigned char addr,unsigned char cha)
*功  能：固定地址模式在指定位显示一个数据
*说  明：
*参  数：digi-显示的位，从1-8（从左到右），cha-待显示数据
**************************************************************************************/
void DisplayOneDigi_TM1668(unsigned char digi,unsigned char cha)
{
	 unsigned char addr=0;

	 addr=0x0f&((digi-1)<<1);				//先计算出起始地址
	 
	 SET_DISPLAY_MODE;						//设置段、位模块
	 ADDRESS_MODE_FIX;						//发送地址模式命令
	 SendOneData_TM1668(addr,cha);	 		//发送起始地址和连续的数据
	 DISPLAY_ON(LightLevel);				//打开显示

}
/***************************************END********************************************/
