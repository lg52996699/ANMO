
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

#include	"config.h"
#include	"USART1.h"
#include	"delay.h"
#include "eeprom.h"
#include "plat.h"
#include	"timer.h"
#include	"adc.h"
#include	"GPIO.h"
#include	"PCA.h"
/*************	功能说明	**************

双串口全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

******************************************/

/*************I/O口定义	**************/
//(IN)按键定义
sbit key_1 = P3^4;      //启动按键
sbit key_2 = P3^5;    //加:short preee :time+1/long press :time+10
sbit key_3 = P3^6;    //减:short preee :time-1/long press :time-10
sbit key_4 = P5^5;      //内置模式
sbit key_5 = P5^4;    //外置模式
//(OUT)LED灯定义
sbit LED_I = P3^2;    //内置模式显示LED
sbit LED_O = P3^3;    //外置模式显示LED
sbit LED_R = P2^4;    //红色LED灯光
sbit LED_G = P2^3;    //绿色LED灯光
//(OUT)继电器控制
sbit Beng = P2^5;     //气泵（继电器）喷雾 
sbit Change = P2^6;   //内外置切换继电器
//(OUT)PWM
sbit Cover = P1^7;    //盖子开关


/* sbit Buzzer = P2^2;  //蜂鸣器*/


/************* 本地变量声明	**************/
static unsigned char second,minute;
unsigned char key_press_num1=0,key_press_num2=0,key_press_num3=0;  //按键设置相关
unsigned char Set_minute,tcount,Change_flag=0;                         //设置时间相关
unsigned char a_a;
unsigned char FinishSecond=0;
//u16 Remain;
unsigned char Remain,last_remain,now_remain;
u16 last_level,current_level,base_level;

bit water_flag = 0;
bit finish_flag=0; 
bit start_flag=0;
bit key_flag=0;
//u16	j;
volatile bit FlagTest = 1;		//定时测试标志，每0.5秒置位，测完清0
volatile bit timer_start=0; //默认为内置模式
/************* 本地函数声明	**************/
void Delay(unsigned char time);
void GPIO_config(void);
void CheckUart();
void KeyPress();
void write_eeprom();
void writeBase_eeprom();
void read_eeprom();
void init_eeprom();
void	UART_config(void);
void	Timer_config(void);
void	ADC_config(void);
void	PCA_config(void);

/************* 本地变量声明 *****************/
#define READ_TIMES 15 //读取次数
#define LOST_VAL 5	  //丢弃值

/*****读取AD值并进行优化处理***********/
u16 AD_Read(u8 CHx,u8 flag)
{
	u8  i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp,Temp1;
	static u8 add;
	if(timer_start == 1)
	{
		add = 10;
	}	
	else
		add = 20;
	for(i=0;i<READ_TIMES;i++)            //连续读取十五次,把读取十五次的数据存入 
	{
		if(flag)
	  {
			Temp1=Get_ADC10bitResult(CHx);	   
			if(Temp1 > (current_level + add))  //限幅滤波
			{
					 buf[i] = current_level + add;
			}
			else if(Temp1 < (current_level - add))
			{
					 buf[i] = current_level - add;
			}
			else
			{
					 buf[i] = current_level;
			}
		}
		else
		{
			buf[i] = Get_ADC10bitResult(CHx);	
		}
	  Delay(2);
  }				     
/*******    升序排列 ********/
	for(i=0;i<READ_TIMES-1; i++)//排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	
/**********  ***********/ 
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];  //去掉最低和最高LOST_VAL个数并求和
	temp=sum/(READ_TIMES-2*LOST_VAL);                      //取平均值
	return temp;                                           //返回读取的平均值 
} 
/**********************************************/
void main(void)
{
	unsigned char i;
	unsigned char remain_counter;
	Delay(100);
	Change_flag=0;
	Set_minute=1;
	GPIO_config();
	UART_config();
	Timer_config();
	ADC_config();
	PCA_config();
	EA = 1;
	init_eeprom();  //开始初始化保存的数据
	LED_R=0;  //RED-LED 亮
	Delay(10);
	LED_I=0;
	Delay(10);
	LED_O=0;
	DisplayInterfaceInit();						  //初始化接口
   //让所有位所有段全亮
 	for(i=0;i<8;i++)
 	{
 		NoDecodeDisplayOneDigi(i+1,0xFF);		  //在指定地址显示指定内容
 	}
 	Delay(1000);	
 	LED_I=1;
 	Delay(10);
 	LED_O=1;
 	
 	Delay(10);	
 	LED_R=1;  //RED-LED 亮
 	Delay(10);
 	//让所有位所有段全灭
 	for(i=0;i<8;i++)
 	{
 		NoDecodeDisplayOneDigi(i+1,0x00);		  //在指定地址显示指定内容
 	}
	Delay(1000);
	Delay(10);	
	LED_R=1;  //RED-LED 亮
	Delay(10);
	LED_G=1;
	Delay(10);
	Delay(50);
	if(Change_flag==0)//内置模式显示液量信息
	{	
		Change = 1;
		delay_ms(10);
		LED_I = 0;
	    delay_ms(10);
        LED_O = 1;
    }
	else //外置模式液量不显示
	{
		Change = 0;
		delay_ms(10);
		LED_I = 1;
	  delay_ms(10);
    LED_O = 0;
		NoDecodeDisplayOneDigi(1,0x00);	
		NoDecodeDisplayOneDigi(2,0x00);	
		NoDecodeDisplayOneDigi(3,0x00);	
	}
	CR = 0;
	TR0=1;
	DisplayOneDigi(4,Set_minute/100,0);		     //在指定地址显示指定内容
	DisplayOneDigi(5,Set_minute%100/10,0);		 //在指定地址显示指定内容
	DisplayOneDigi(6,Set_minute%10,0);		     //在指定地址显示指定内容
	current_level = AD_Read(0,0);
	last_level = current_level;
	//TX1_write2buff(0x0a);	//收到的数据原样返回
	TX1_write2buff(0x0a);	//收到的数据原样返回
	TX1_write2buff(base_level>>8);	//收到的数据原样返回
	TX1_write2buff(base_level&0xff);	//收到的数据原样返回
	while (1)
	{
		
		KeyPress();
		if(RX_FinishFlag)
		{
		  RX_FinishFlag=0;
		  CheckUart();
		}
		
	    if(Change_flag==0)//内置模式显示液量信息
		{	
			if(FlagTest)
			{
				FlagTest=0;			
				
				if (timer_start == 1)
				{
					current_level = AD_Read(0,1);
				  if(current_level > (last_level+5))
				  {
				     current_level = last_level +5;
				  }
				  else if(current_level < (last_level-5))
				  {
				     current_level = last_level-5;
				  }
			  }
				else
				{
				 current_level = AD_Read(0,0);
				}	
				if(current_level < base_level)
				{ 
				  now_remain = 0;
				}
				else if(current_level < (base_level+13))
				{
          now_remain = 0;
				}	
				else if(current_level < (base_level+20))
				{
          now_remain = 10;
				}	
				else if(current_level <= (base_level+220))
				{
						now_remain=(unsigned char)((current_level-base_level-19)/23*10+10);
				}
				else if(current_level > (base_level+223))
				{
				  now_remain = 100;
				}
				if (timer_start == 1)
				{
					if(last_remain != now_remain) //显示消抖滤波
					{
						remain_counter++;
						if(remain_counter>2)
						{
						    remain_counter = 0;
						    Remain = now_remain;
						}	
						else
						{
						    Remain = last_remain;
						}
					}
					else
					{
						remain_counter = 0;
						Remain = last_remain;
					}
				}
				else
				{
				   Remain = now_remain;
				}
			    last_level = current_level;	
			    last_remain = Remain;
				if(Remain>=100)
				{
					Remain=100;
					DisplayOneDigi(1,Remain/100,0);		  //在指定地址显示指定内容
					DisplayOneDigi(2,Remain%100/10,0);    //在指定地址显示指定内容
					DisplayOneDigi(3,Remain%10,0);		  //在指定地址显示指定内容}	
				}
				else
				{
	  				//DisplayOneDigi(1,Remain/100,0);	  //在指定地址显示指定内容
	  				NoDecodeDisplayOneDigi(1,0x00);		  //在指定地址显示指定内容
	  				DisplayOneDigi(2,Remain/10,0);		  //在指定地址显示指定内容
	  				DisplayOneDigi(3,Remain%10,0);		  //在指定地址显示指定内容}	
				}
				
				/*
				if(Remain == 0)
				{
					if(timer_start == 1)
					{
							minute=0;
							second=0;
							timer_start=0;    //定时器关闭,红灯和绿灯都同时关闭，喷雾停止，泵停止.
							LED_R=1;  
							LED_G=1;
							Cover =1;
							Beng =1;
							start_flag=0;
							TX1_write2buff(0x0a);	//收到的数据原样返回
							TX1_write2buff(0x00);	//收到的数据原样返回
							TX1_write2buff(Remain);	
							TX1_write2buff(0x00);	//用户中断
							TX1_write2buff(0xff);	//收到的数据原样返回
							FinishSecond=0;//系统操作完成标志
							finish_flag=1; 
					}
				}*/
			 }
		 }
		else //外置模式液量不显示
		{
			NoDecodeDisplayOneDigi(1,0x00);	
			NoDecodeDisplayOneDigi(2,0x00);	
			NoDecodeDisplayOneDigi(3,0x00);	
		}
		
		
	}
}
/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
 	   tcount++;
    if(tcount==40)
	  {
			tcount=0;
			FlagTest = 1;
			//LED_R=!LED_R;  
			//LED_G=!LED_G;
			if(timer_start)
				{	second++;
				 if(second==60)
					{
						second=0;
						minute++;
						if(minute==1)//一分钟后红灯灭，绿灯亮，开始喷雾
						{
							
							LED_R=1;
							LED_G=0;
							Beng =0;
						}
					 if(minute==Set_minute+1)//时间到，则停止动作
					 {
							minute=0;
							second=0;
							LED_R=1;  
							LED_G=1;
							Beng =1;
						  
							timer_start=0;    //定时器关闭,红灯和绿灯都同时关闭，喷雾停止，泵停止.
							start_flag=0;
							TX1_write2buff(0x0a);	//收到的数据原样返回
							TX1_write2buff(0x02);	//收到的数据原样返回
							TX1_write2buff(Remain);	//收到的数据原样返回
							TX1_write2buff(0x00);	//自动停止
							TX1_write2buff(0xff);	//收到的数据原样返回
						 
              FinishSecond=0;//系统操作完成标志
							finish_flag=1; 
					 }
					}
					
				}
			if(finish_flag)//控制舵机延时关闭。
			{
				FinishSecond++;
			  if(FinishSecond ==5)
				{
					CR = 1;//关闭盖子
				  set_2=5;
				}
				else if(FinishSecond >= 8)
				{	
					FinishSecond=0;
					finish_flag=0;
				  CR = 0;//关闭舵机
				}	
			}
				
	  }
	
}

/*************************************************************************************
*函数名：Delay(unsigned char time)
*功  能：延时函数
**************************************************************************************/
void Delay(unsigned char time)
{
	int i,j,k;
	for(i=0;i<time;i++)
		for(j=0;j<50;j++)
			for(k=0;k<90;k++)
			{
				;
			}
}
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
	GPIO_InitStructure.Pin  = GPIO_Pin_2|GPIO_Pin_3;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//初始化
}
//串口检测
void CheckUart()
{
    if(((RX1_Buffer[0]) == 0x0a)&&((RX1_Buffer[4]) == 0xff) )
    {
			switch(RX1_Buffer[2])
			{
				case 0x00 :  //中断程序运行	
				minute=0;
				second=0;
				timer_start=0;    //定时器关闭,红灯和绿灯都同时关闭，喷雾停止，泵停止.
				LED_R=1;  
				LED_G=1;
				Cover =1;
				Beng =1;
				start_flag=0;
				TX1_write2buff(0x0a);	//收到的数据原样返回
				TX1_write2buff(0x00);	//收到的数据原样返回
				TX1_write2buff(Remain);	
				TX1_write2buff(0x00);	//用户中断
				TX1_write2buff(0xff);	//收到的数据原样返回
				FinishSecond=0;//系统操作完成标志
				finish_flag=1; 
			break;				
				case 0x01 ://点动启动，系统运行		
				start_flag=1;
				timer_start=1; 
				Cover=0;
				LED_R=0;  //RED-LED 亮
				delay_ms(20);
				//LED_G=0;
				TX1_write2buff(0x0a);	//收到的数据原样返回
				TX1_write2buff(0x01);	//收到的数据原样返回
				TX1_write2buff(Remain);		//收到的数据原样返回
				TX1_write2buff(Set_minute);	//收到的数据原样返回
				TX1_write2buff(0xff);	//收到的数据原样返回
				FinishSecond=0;//系统操作完成标志
				finish_flag=0; 
				CR = 1;//打开盖子
				set_2=17;
				delay_ms(200);
			  delay_ms(200);
			  delay_ms(200);
				delay_ms(200);
			  CR = 0;//关闭舵机
				
			break;
			//	default:
			}
			
	 }
}	

//按键检测
void KeyPress()
{
	if(key_1==0)   //启动按键
	{
		delay_ms(15);
		if(key_1==0)
		{
			
			while(!key_1)
			{
				delay_ms(10);
				key_press_num1++;
				if(key_press_num1>=100)//长按启动，系统停止运行
				{
					key_press_num1=0;
					minute=0;
					second=0;
					timer_start=0;    //定时器关闭,红灯和绿灯都同时关闭，喷雾停止，泵停止.
					LED_R=1;  
					LED_G=1;
					Cover =1;
					Beng =1;
					start_flag=0;
					TX1_write2buff(0x0a);	//收到的数据原样返回
					TX1_write2buff(0x00);	//收到的数据原样返回
					TX1_write2buff(Remain);	
					TX1_write2buff(0x00);	//用户中断
					TX1_write2buff(0xff);	//收到的数据原样返回
          FinishSecond=0;//系统操作完成标志
					finish_flag=1; 
					while(!key_1);
					delay_ms(20);
					key_flag=1; 
				}
			}
			if((key_press_num1!=0)&&(key_flag==0))//点动启动，系统运行
			{
				if(start_flag==0)
	      {  
					key_press_num1=0;
					start_flag=1;
					timer_start=1; 
					Cover=0;
					LED_R=0;  //RED-LED 亮
					delay_ms(20);
					//LED_G=0;
					TX1_write2buff(0x0a);	//收到的数据原样返回
					TX1_write2buff(0x01);	//收到的数据原样返回
					TX1_write2buff(Remain);		//收到的数据原样返回
					TX1_write2buff(Set_minute);	//收到的数据原样返回
					TX1_write2buff(0xff);	//收到的数据原样返回
					FinishSecond=0;//系统操作完成标志
					finish_flag=0; 
					CR = 1;//打开盖子
					set_2=17;
					delay_ms(200);
					delay_ms(200);
					delay_ms(200);
					delay_ms(200);
					CR = 0;//关闭舵机
				}
			}
			key_press_num1 = 0;
			key_flag=0;
		}
	}
	if(start_flag==0)
	{
	 if(key_2==0)	   //加:short preee :time+1/long press :time+10
	 {
		delay_ms(5);
		if(key_2==0)
		{
			if(key_3 == 0)  
      {  
        base_level = AD_Read(0,0); 
				writeBase_eeprom();  //两个按键同时按下执行函数      
        while(key_3 == 0); //松手检测
			}
			else    
      {      
				while(!key_2)
				{
					key_press_num2++;
				 if(key_press_num2>=100)
					{
							key_press_num2=0;
							if(Set_minute<240)
							Set_minute+=10;
							if(Set_minute>=240)
							Set_minute=240;	
							//显示
							DisplayOneDigi(4,Set_minute/100,0);		      //在指定地址显示指定内容
							DisplayOneDigi(5,Set_minute%100/10,0);		  //在指定地址显示指定内容
							DisplayOneDigi(6,Set_minute%10,0);		      //在指定地址显示指定内容*/	
					}
					delay_ms(10);
				}	
				if(key_press_num2!=0)
				{
					key_press_num2=0;
					if(Set_minute<240)
					Set_minute+=1;
					if(Set_minute>=240)
					Set_minute=240;
					//显示
					DisplayOneDigi(4,Set_minute/100,0);		  //在指定地址显示指定内容
					DisplayOneDigi(5,Set_minute%100/10,0);		  //在指定地址显示指定内容
					DisplayOneDigi(6,Set_minute%10,0);		  //在指定地址显示指定内容*/
				}
				write_eeprom();
			}	
		}
	}
	if(key_3==0)  //减:short preee :time-1/long press :time-10
	{
		delay_ms(5);
		if(key_3==0)
		{
			if(key_2 == 0)  
      {  
        base_level = AD_Read(0,0);
				writeBase_eeprom();  //两个按键同时按下执行函数    
        while(key_2 == 0); //松手检测    
      }
			else
			{	
				while(!key_3)
				{
					key_press_num3++;
				 if(key_press_num3>=100)
					{
							key_press_num3=0;
							if(Set_minute>1)
							Set_minute-=10;   
							if(Set_minute<=1||Set_minute>240)//防止数据溢出
							Set_minute=1;	
							//显示
							DisplayOneDigi(4,Set_minute/100,0);		  //在指定地址显示指定内容
							DisplayOneDigi(5,Set_minute%100/10,0);		  //在指定地址显示指定内容
							DisplayOneDigi(6,Set_minute%10,0);		  //在指定地址显示指定内容*/
					}
					delay_ms(10);
				}	
				if(key_press_num3!=0)
				{
					key_press_num3=0;
					if(Set_minute>1)
					Set_minute-=1;
					if(Set_minute<=1)
					Set_minute=1;
					//显示
					DisplayOneDigi(4,Set_minute/100,0);		  //在指定地址显示指定内容
					DisplayOneDigi(5,Set_minute%100/10,0);		  //在指定地址显示指定内容
					DisplayOneDigi(6,Set_minute%10,0);		  //在指定地址显示指定内容*/
				}
				write_eeprom();	
			}	
	 }
 }
	 if(key_4==0)	   //内置
	{
		delay_ms(5);
		if(key_4==0)
		{
			Change_flag=0;
			Change = 1;
			
			delay_ms(10);
			LED_I = 0;
		  delay_ms(10);
      LED_O = 1;
			write_eeprom();
    }	
	}
	if(key_5==0)	   //外置
	{
		delay_ms(5);
		if(key_5==0)
		{
			Change_flag=1;
			Change = 0;
			delay_ms(10);
			LED_I = 1;
			delay_ms(10);
      LED_O = 0;
		  write_eeprom();
			
			NoDecodeDisplayOneDigi(1,0x00);	
			NoDecodeDisplayOneDigi(2,0x00);	
			NoDecodeDisplayOneDigi(3,0x00);	
    }	
	}
 }	
}
/**
/******************把数据保存到单片机内部eeprom中******************/
void write_eeprom()
{
		SectorErase(0x1000);
	  byte_write(0x1000,Set_minute);
	  byte_write(0x1030,Change_flag);
	  byte_write(0x1060,a_a);	
}
void writeBase_eeprom()
{
	SectorErase(0xE00);
	byte_write(0xE00,base_level);
}
/******************把数据从单片机内部eeprom中读出来*****************/
void read_eeprom()
{
	Set_minute  = byte_read(0x1000);
	Change_flag = byte_read(0x1030);
	a_a   = byte_read(0x1060);
	base_level   = byte_read(0xE00);
}
/**************开机自检eeprom初始化*****************/
void init_eeprom() 
{
	read_eeprom();	//先读
	if(a_a != 1)		//新的单片机初始单片机内问eeprom
	{
		Set_minute=1;
		Change_flag=0;
		a_a =1;
		base_level = AD_Read(0,0);
		write_eeprom();//保存数据	
		writeBase_eeprom();
	}	
}
/*************  串口1初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 9600ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2

	//PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1发送一个字符串
}
/************************ 定时器配置 ****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 0xa600;//65536UL - (MAIN_Fosc/(50*12));		//初值,
	TIM_InitStructure.TIM_Run       = DISABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
		
}
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;				//结构定义
	ADC_InitStructure.ADC_Px        = ADC_P10;	//设置要做ADC的IO,	ADC_P10 ~ ADC_P17(或操作),ADC_P1_All
	ADC_InitStructure.ADC_Speed     = ADC_360T;			//ADC速度			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;			//ADC功率允许/关闭	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;		//ADC结果调整,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;		//优先级设置	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//中断允许		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//初始化
	ADC_PowerControl(ENABLE);							//单独的ADC电源操作函数, ENABLE或DISABLE
}
void	PCA_config(void)
{
	PCA_InitTypeDef		PCA_InitStructure;

	PCA_InitStructure.PCA_Clock    = PCA_Clock_12T;		//PCA_Clock_1T, PCA_Clock_2T, PCA_Clock_4T, PCA_Clock_6T, PCA_Clock_8T, PCA_Clock_12T, PCA_Clock_Timer0_OF, PCA_Clock_ECI
	PCA_InitStructure.PCA_IoUse    = PCA_P24_P25_P26_P27;	//PCA_P12_P11_P10_P37, PCA_P34_P35_P36_P37, PCA_P24_P25_P26_P27
	PCA_InitStructure.PCA_Interrupt_Mode = DISABLE;		//ENABLE, DISABLE
	PCA_InitStructure.PCA_Polity   = PolityHigh;		//优先级设置	PolityHigh,PolityLow
	PCA_InitStructure.PCA_RUN      = DISABLE;			//ENABLE, DISABLE
	//PCA_Init(PCA0,&PCA_InitStructure);

	PCA_InitStructure.PCA_Mode     = PCA_Mode_SoftTimer;	//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	PCA_InitStructure.PCA_PWM_Wide = 0;					//PCA_PWM_8bit, PCA_PWM_7bit, PCA_PWM_6bit
	PCA_InitStructure.PCA_Interrupt_Mode = ENABLE;		//PCA_Rise_Active, PCA_Fall_Active, ENABLE, DISABLE
	PCA_InitStructure.PCA_Value    = 92;				//对于软件定时, 为匹配比较值
	PCA_Init(PCA0,&PCA_InitStructure);
	
	CR = 1;
}