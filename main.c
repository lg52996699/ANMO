
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
/*************	����˵��	**************

˫����ȫ˫���жϷ�ʽ�շ�ͨѶ����

ͨ��PC��MCU��������, MCU�յ���ͨ�����ڰ��յ�������ԭ������.

******************************************/

/*************I/O�ڶ���	**************/
//(IN)��������
sbit key_1 = P3^4;      //��������
sbit key_2 = P3^5;    //��:short preee :time+1/long press :time+10
sbit key_3 = P3^6;    //��:short preee :time-1/long press :time-10
sbit key_4 = P5^5;      //����ģʽ
sbit key_5 = P5^4;    //����ģʽ
//(OUT)LED�ƶ���
sbit LED_I = P3^2;    //����ģʽ��ʾLED
sbit LED_O = P3^3;    //����ģʽ��ʾLED
sbit LED_R = P2^4;    //��ɫLED�ƹ�
sbit LED_G = P2^3;    //��ɫLED�ƹ�
//(OUT)�̵�������
sbit Beng = P2^5;     //���ã��̵��������� 
sbit Change = P2^6;   //�������л��̵���
//(OUT)PWM
sbit Cover = P1^7;    //���ӿ���


/* sbit Buzzer = P2^2;  //������*/


/************* ���ر�������	**************/
static unsigned char second,minute;
unsigned char key_press_num1=0,key_press_num2=0,key_press_num3=0;  //�����������
unsigned char Set_minute,tcount,Change_flag=0;                         //����ʱ�����
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
volatile bit FlagTest = 1;		//��ʱ���Ա�־��ÿ0.5����λ��������0
volatile bit timer_start=0; //Ĭ��Ϊ����ģʽ
/************* ���غ�������	**************/
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

/************* ���ر������� *****************/
#define READ_TIMES 15 //��ȡ����
#define LOST_VAL 5	  //����ֵ

/*****��ȡADֵ�������Ż�����***********/
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
	for(i=0;i<READ_TIMES;i++)            //������ȡʮ���,�Ѷ�ȡʮ��ε����ݴ��� 
	{
		if(flag)
	  {
			Temp1=Get_ADC10bitResult(CHx);	   
			if(Temp1 > (current_level + add))  //�޷��˲�
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
/*******    �������� ********/
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	
/**********  ***********/ 
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];  //ȥ����ͺ����LOST_VAL���������
	temp=sum/(READ_TIMES-2*LOST_VAL);                      //ȡƽ��ֵ
	return temp;                                           //���ض�ȡ��ƽ��ֵ 
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
	init_eeprom();  //��ʼ��ʼ�����������
	LED_R=0;  //RED-LED ��
	Delay(10);
	LED_I=0;
	Delay(10);
	LED_O=0;
	DisplayInterfaceInit();						  //��ʼ���ӿ�
   //������λ���ж�ȫ��
 	for(i=0;i<8;i++)
 	{
 		NoDecodeDisplayOneDigi(i+1,0xFF);		  //��ָ����ַ��ʾָ������
 	}
 	Delay(1000);	
 	LED_I=1;
 	Delay(10);
 	LED_O=1;
 	
 	Delay(10);	
 	LED_R=1;  //RED-LED ��
 	Delay(10);
 	//������λ���ж�ȫ��
 	for(i=0;i<8;i++)
 	{
 		NoDecodeDisplayOneDigi(i+1,0x00);		  //��ָ����ַ��ʾָ������
 	}
	Delay(1000);
	Delay(10);	
	LED_R=1;  //RED-LED ��
	Delay(10);
	LED_G=1;
	Delay(10);
	Delay(50);
	if(Change_flag==0)//����ģʽ��ʾҺ����Ϣ
	{	
		Change = 1;
		delay_ms(10);
		LED_I = 0;
	    delay_ms(10);
        LED_O = 1;
    }
	else //����ģʽҺ������ʾ
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
	DisplayOneDigi(4,Set_minute/100,0);		     //��ָ����ַ��ʾָ������
	DisplayOneDigi(5,Set_minute%100/10,0);		 //��ָ����ַ��ʾָ������
	DisplayOneDigi(6,Set_minute%10,0);		     //��ָ����ַ��ʾָ������
	current_level = AD_Read(0,0);
	last_level = current_level;
	//TX1_write2buff(0x0a);	//�յ�������ԭ������
	TX1_write2buff(0x0a);	//�յ�������ԭ������
	TX1_write2buff(base_level>>8);	//�յ�������ԭ������
	TX1_write2buff(base_level&0xff);	//�յ�������ԭ������
	while (1)
	{
		
		KeyPress();
		if(RX_FinishFlag)
		{
		  RX_FinishFlag=0;
		  CheckUart();
		}
		
	    if(Change_flag==0)//����ģʽ��ʾҺ����Ϣ
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
					if(last_remain != now_remain) //��ʾ�����˲�
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
					DisplayOneDigi(1,Remain/100,0);		  //��ָ����ַ��ʾָ������
					DisplayOneDigi(2,Remain%100/10,0);    //��ָ����ַ��ʾָ������
					DisplayOneDigi(3,Remain%10,0);		  //��ָ����ַ��ʾָ������}	
				}
				else
				{
	  				//DisplayOneDigi(1,Remain/100,0);	  //��ָ����ַ��ʾָ������
	  				NoDecodeDisplayOneDigi(1,0x00);		  //��ָ����ַ��ʾָ������
	  				DisplayOneDigi(2,Remain/10,0);		  //��ָ����ַ��ʾָ������
	  				DisplayOneDigi(3,Remain%10,0);		  //��ָ����ַ��ʾָ������}	
				}
				
				/*
				if(Remain == 0)
				{
					if(timer_start == 1)
					{
							minute=0;
							second=0;
							timer_start=0;    //��ʱ���ر�,��ƺ��̵ƶ�ͬʱ�رգ�����ֹͣ����ֹͣ.
							LED_R=1;  
							LED_G=1;
							Cover =1;
							Beng =1;
							start_flag=0;
							TX1_write2buff(0x0a);	//�յ�������ԭ������
							TX1_write2buff(0x00);	//�յ�������ԭ������
							TX1_write2buff(Remain);	
							TX1_write2buff(0x00);	//�û��ж�
							TX1_write2buff(0xff);	//�յ�������ԭ������
							FinishSecond=0;//ϵͳ������ɱ�־
							finish_flag=1; 
					}
				}*/
			 }
		 }
		else //����ģʽҺ������ʾ
		{
			NoDecodeDisplayOneDigi(1,0x00);	
			NoDecodeDisplayOneDigi(2,0x00);	
			NoDecodeDisplayOneDigi(3,0x00);	
		}
		
		
	}
}
/********************* Timer0�жϺ���************************/
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
						if(minute==1)//һ���Ӻ������̵�������ʼ����
						{
							
							LED_R=1;
							LED_G=0;
							Beng =0;
						}
					 if(minute==Set_minute+1)//ʱ�䵽����ֹͣ����
					 {
							minute=0;
							second=0;
							LED_R=1;  
							LED_G=1;
							Beng =1;
						  
							timer_start=0;    //��ʱ���ر�,��ƺ��̵ƶ�ͬʱ�رգ�����ֹͣ����ֹͣ.
							start_flag=0;
							TX1_write2buff(0x0a);	//�յ�������ԭ������
							TX1_write2buff(0x02);	//�յ�������ԭ������
							TX1_write2buff(Remain);	//�յ�������ԭ������
							TX1_write2buff(0x00);	//�Զ�ֹͣ
							TX1_write2buff(0xff);	//�յ�������ԭ������
						 
              FinishSecond=0;//ϵͳ������ɱ�־
							finish_flag=1; 
					 }
					}
					
				}
			if(finish_flag)//���ƶ����ʱ�رա�
			{
				FinishSecond++;
			  if(FinishSecond ==5)
				{
					CR = 1;//�رո���
				  set_2=5;
				}
				else if(FinishSecond >= 8)
				{	
					FinishSecond=0;
					finish_flag=0;
				  CR = 0;//�رն��
				}	
			}
				
	  }
	
}

/*************************************************************************************
*��������Delay(unsigned char time)
*��  �ܣ���ʱ����
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
	GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����
	GPIO_InitStructure.Pin  = GPIO_Pin_2|GPIO_Pin_3;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//��ʼ��
}
//���ڼ��
void CheckUart()
{
    if(((RX1_Buffer[0]) == 0x0a)&&((RX1_Buffer[4]) == 0xff) )
    {
			switch(RX1_Buffer[2])
			{
				case 0x00 :  //�жϳ�������	
				minute=0;
				second=0;
				timer_start=0;    //��ʱ���ر�,��ƺ��̵ƶ�ͬʱ�رգ�����ֹͣ����ֹͣ.
				LED_R=1;  
				LED_G=1;
				Cover =1;
				Beng =1;
				start_flag=0;
				TX1_write2buff(0x0a);	//�յ�������ԭ������
				TX1_write2buff(0x00);	//�յ�������ԭ������
				TX1_write2buff(Remain);	
				TX1_write2buff(0x00);	//�û��ж�
				TX1_write2buff(0xff);	//�յ�������ԭ������
				FinishSecond=0;//ϵͳ������ɱ�־
				finish_flag=1; 
			break;				
				case 0x01 ://�㶯������ϵͳ����		
				start_flag=1;
				timer_start=1; 
				Cover=0;
				LED_R=0;  //RED-LED ��
				delay_ms(20);
				//LED_G=0;
				TX1_write2buff(0x0a);	//�յ�������ԭ������
				TX1_write2buff(0x01);	//�յ�������ԭ������
				TX1_write2buff(Remain);		//�յ�������ԭ������
				TX1_write2buff(Set_minute);	//�յ�������ԭ������
				TX1_write2buff(0xff);	//�յ�������ԭ������
				FinishSecond=0;//ϵͳ������ɱ�־
				finish_flag=0; 
				CR = 1;//�򿪸���
				set_2=17;
				delay_ms(200);
			  delay_ms(200);
			  delay_ms(200);
				delay_ms(200);
			  CR = 0;//�رն��
				
			break;
			//	default:
			}
			
	 }
}	

//�������
void KeyPress()
{
	if(key_1==0)   //��������
	{
		delay_ms(15);
		if(key_1==0)
		{
			
			while(!key_1)
			{
				delay_ms(10);
				key_press_num1++;
				if(key_press_num1>=100)//����������ϵͳֹͣ����
				{
					key_press_num1=0;
					minute=0;
					second=0;
					timer_start=0;    //��ʱ���ر�,��ƺ��̵ƶ�ͬʱ�رգ�����ֹͣ����ֹͣ.
					LED_R=1;  
					LED_G=1;
					Cover =1;
					Beng =1;
					start_flag=0;
					TX1_write2buff(0x0a);	//�յ�������ԭ������
					TX1_write2buff(0x00);	//�յ�������ԭ������
					TX1_write2buff(Remain);	
					TX1_write2buff(0x00);	//�û��ж�
					TX1_write2buff(0xff);	//�յ�������ԭ������
          FinishSecond=0;//ϵͳ������ɱ�־
					finish_flag=1; 
					while(!key_1);
					delay_ms(20);
					key_flag=1; 
				}
			}
			if((key_press_num1!=0)&&(key_flag==0))//�㶯������ϵͳ����
			{
				if(start_flag==0)
	      {  
					key_press_num1=0;
					start_flag=1;
					timer_start=1; 
					Cover=0;
					LED_R=0;  //RED-LED ��
					delay_ms(20);
					//LED_G=0;
					TX1_write2buff(0x0a);	//�յ�������ԭ������
					TX1_write2buff(0x01);	//�յ�������ԭ������
					TX1_write2buff(Remain);		//�յ�������ԭ������
					TX1_write2buff(Set_minute);	//�յ�������ԭ������
					TX1_write2buff(0xff);	//�յ�������ԭ������
					FinishSecond=0;//ϵͳ������ɱ�־
					finish_flag=0; 
					CR = 1;//�򿪸���
					set_2=17;
					delay_ms(200);
					delay_ms(200);
					delay_ms(200);
					delay_ms(200);
					CR = 0;//�رն��
				}
			}
			key_press_num1 = 0;
			key_flag=0;
		}
	}
	if(start_flag==0)
	{
	 if(key_2==0)	   //��:short preee :time+1/long press :time+10
	 {
		delay_ms(5);
		if(key_2==0)
		{
			if(key_3 == 0)  
      {  
        base_level = AD_Read(0,0); 
				writeBase_eeprom();  //��������ͬʱ����ִ�к���      
        while(key_3 == 0); //���ּ��
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
							//��ʾ
							DisplayOneDigi(4,Set_minute/100,0);		      //��ָ����ַ��ʾָ������
							DisplayOneDigi(5,Set_minute%100/10,0);		  //��ָ����ַ��ʾָ������
							DisplayOneDigi(6,Set_minute%10,0);		      //��ָ����ַ��ʾָ������*/	
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
					//��ʾ
					DisplayOneDigi(4,Set_minute/100,0);		  //��ָ����ַ��ʾָ������
					DisplayOneDigi(5,Set_minute%100/10,0);		  //��ָ����ַ��ʾָ������
					DisplayOneDigi(6,Set_minute%10,0);		  //��ָ����ַ��ʾָ������*/
				}
				write_eeprom();
			}	
		}
	}
	if(key_3==0)  //��:short preee :time-1/long press :time-10
	{
		delay_ms(5);
		if(key_3==0)
		{
			if(key_2 == 0)  
      {  
        base_level = AD_Read(0,0);
				writeBase_eeprom();  //��������ͬʱ����ִ�к���    
        while(key_2 == 0); //���ּ��    
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
							if(Set_minute<=1||Set_minute>240)//��ֹ�������
							Set_minute=1;	
							//��ʾ
							DisplayOneDigi(4,Set_minute/100,0);		  //��ָ����ַ��ʾָ������
							DisplayOneDigi(5,Set_minute%100/10,0);		  //��ָ����ַ��ʾָ������
							DisplayOneDigi(6,Set_minute%10,0);		  //��ָ����ַ��ʾָ������*/
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
					//��ʾ
					DisplayOneDigi(4,Set_minute/100,0);		  //��ָ����ַ��ʾָ������
					DisplayOneDigi(5,Set_minute%100/10,0);		  //��ָ����ַ��ʾָ������
					DisplayOneDigi(6,Set_minute%10,0);		  //��ָ����ַ��ʾָ������*/
				}
				write_eeprom();	
			}	
	 }
 }
	 if(key_4==0)	   //����
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
	if(key_5==0)	   //����
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
/******************�����ݱ��浽��Ƭ���ڲ�eeprom��******************/
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
/******************�����ݴӵ�Ƭ���ڲ�eeprom�ж�����*****************/
void read_eeprom()
{
	Set_minute  = byte_read(0x1000);
	Change_flag = byte_read(0x1030);
	a_a   = byte_read(0x1060);
	base_level   = byte_read(0xE00);
}
/**************�����Լ�eeprom��ʼ��*****************/
void init_eeprom() 
{
	read_eeprom();	//�ȶ�
	if(a_a != 1)		//�µĵ�Ƭ����ʼ��Ƭ������eeprom
	{
		Set_minute=1;
		Change_flag=0;
		a_a =1;
		base_level = AD_Read(0,0);
		write_eeprom();//��������	
		writeBase_eeprom();
	}	
}
/*************  ����1��ʼ������ *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ʹ�ò�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 9600ul;			//������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//�ж����ȼ�, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(����ʹ���ڲ�ʱ��)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//��ʼ������1 USART1,USART2

	//PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1����һ���ַ���
}
/************************ ��ʱ������ ****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//�ṹ����
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 0xa600;//65536UL - (MAIN_Fosc/(50*12));		//��ֵ,
	TIM_InitStructure.TIM_Run       = DISABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2
		
}
void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;				//�ṹ����
	ADC_InitStructure.ADC_Px        = ADC_P10;	//����Ҫ��ADC��IO,	ADC_P10 ~ ADC_P17(�����),ADC_P1_All
	ADC_InitStructure.ADC_Speed     = ADC_360T;			//ADC�ٶ�			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;			//ADC��������/�ر�	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;		//ADC�������,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;		//���ȼ�����	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//�ж�����		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//��ʼ��
	ADC_PowerControl(ENABLE);							//������ADC��Դ��������, ENABLE��DISABLE
}
void	PCA_config(void)
{
	PCA_InitTypeDef		PCA_InitStructure;

	PCA_InitStructure.PCA_Clock    = PCA_Clock_12T;		//PCA_Clock_1T, PCA_Clock_2T, PCA_Clock_4T, PCA_Clock_6T, PCA_Clock_8T, PCA_Clock_12T, PCA_Clock_Timer0_OF, PCA_Clock_ECI
	PCA_InitStructure.PCA_IoUse    = PCA_P24_P25_P26_P27;	//PCA_P12_P11_P10_P37, PCA_P34_P35_P36_P37, PCA_P24_P25_P26_P27
	PCA_InitStructure.PCA_Interrupt_Mode = DISABLE;		//ENABLE, DISABLE
	PCA_InitStructure.PCA_Polity   = PolityHigh;		//���ȼ�����	PolityHigh,PolityLow
	PCA_InitStructure.PCA_RUN      = DISABLE;			//ENABLE, DISABLE
	//PCA_Init(PCA0,&PCA_InitStructure);

	PCA_InitStructure.PCA_Mode     = PCA_Mode_SoftTimer;	//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	PCA_InitStructure.PCA_PWM_Wide = 0;					//PCA_PWM_8bit, PCA_PWM_7bit, PCA_PWM_6bit
	PCA_InitStructure.PCA_Interrupt_Mode = ENABLE;		//PCA_Rise_Active, PCA_Fall_Active, ENABLE, DISABLE
	PCA_InitStructure.PCA_Value    = 92;				//���������ʱ, Ϊƥ��Ƚ�ֵ
	PCA_Init(PCA0,&PCA_InitStructure);
	
	CR = 1;
}