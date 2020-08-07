/*-------------------------------------------------------------------------------------
 * ��Ŀ����:

     TM1668оƬ��������v1.0

 * ��Ȩ��Ϣ:

     (c) �������, 2016.

 * ��ʷ�汾:
     2016-5-11:
       - ��ʼ�汾 V1.0.0;

 * ��ע:
     - ʹ��ǰ��Ҫ������Ľӿ����ò�������ͨ�Žӿ�

----------------------------------------------------------------------------------------*/

#ifndef _TM1668_H_
#define _TM1668_H_

/*---------------------------�ӿ�����---------------------------------------------------*/

sbit DIO = P2^1;			   //���������������  
sbit CLK = P2^0; 			   //ʱ���ź�����  
sbit STB = P3^7;			   //�����ź�����

/*---------------------------�궨��-----------------------------------------------------*/
#define LightLevel						0x04						//������ʾ���ȣ�����ֵ��0x03��0x07
#define DISPLAY_ON(LightLevel)	 		SendCmd_TM1668(0x88|(0x07&LightLevel))	 //����ʾ
#define DISPLAY_OFF	 					SendCmd_TM1668(0x80)					 //�ر���ʾ
#define SET_DISPLAY_MODE				SendCmd_TM1668(0x03)		//������ʾģʽ��0x02:7λ10��
#define ADDRESS_MODE_INC 				SendCmd_TM1668(0x40)		//������ʾģʽ��0x40:��ͨģʽ��д���ݵ���ʾ
																	//�Ĵ�������ַ����ģʽ
#define ADDRESS_MODE_FIX				SendCmd_TM1668(0x44)		//������ʾģʽ��0x44����ͨģʽ��д���ݵ���ʾ
																	//�Ĵ������̶���ַģʽ
#define SET_LIGHT_LEVEL(LightLevel)		SendCmd_TM1668(0x88|(0x07&LtghtLevel))
																	//�������ȵȼ�������ʾ

							   
/*---------------------------��������---------------------------------------------------*/
void Init1668Interface(void);
void SendCmd_TM1668(unsigned char cmd);
void DisplayArr_TM1668(unsigned char start,unsigned char* arr,unsigned char len);
void DisplayOneDigi_TM1668(unsigned char digi,unsigned char cha);
#endif