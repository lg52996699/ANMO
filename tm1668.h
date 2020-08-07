/*-------------------------------------------------------------------------------------
 * 项目名称:

     TM1668芯片驱动程序v1.0

 * 版权信息:

     (c) 飞翼电子, 2016.

 * 历史版本:
     2016-5-11:
       - 初始版本 V1.0.0;

 * 备注:
     - 使用前需要在下面的接口设置部分设置通信接口

----------------------------------------------------------------------------------------*/

#ifndef _TM1668_H_
#define _TM1668_H_

/*---------------------------接口设置---------------------------------------------------*/

sbit DIO = P2^1;			   //数据输入输出引脚  
sbit CLK = P2^0; 			   //时钟信号引脚  
sbit STB = P3^7;			   //锁存信号引脚

/*---------------------------宏定义-----------------------------------------------------*/
#define LightLevel						0x04						//设置显示亮度，设置值从0x03到0x07
#define DISPLAY_ON(LightLevel)	 		SendCmd_TM1668(0x88|(0x07&LightLevel))	 //打开显示
#define DISPLAY_OFF	 					SendCmd_TM1668(0x80)					 //关闭显示
#define SET_DISPLAY_MODE				SendCmd_TM1668(0x03)		//设置显示模式，0x02:7位10段
#define ADDRESS_MODE_INC 				SendCmd_TM1668(0x40)		//设置显示模式，0x40:普通模式、写数据到显示
																	//寄存器，地址自增模式
#define ADDRESS_MODE_FIX				SendCmd_TM1668(0x44)		//设置显示模式，0x44：普通模式，写数据到显示
																	//寄存器，固定地址模式
#define SET_LIGHT_LEVEL(LightLevel)		SendCmd_TM1668(0x88|(0x07&LtghtLevel))
																	//设置亮度等级，打开显示

							   
/*---------------------------函数声明---------------------------------------------------*/
void Init1668Interface(void);
void SendCmd_TM1668(unsigned char cmd);
void DisplayArr_TM1668(unsigned char start,unsigned char* arr,unsigned char len);
void DisplayOneDigi_TM1668(unsigned char digi,unsigned char cha);
#endif