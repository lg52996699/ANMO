/*------------------------------------------------------------------------------------
 * ��Ŀ����:

     TM1638ģ����������v1.0
 
 * ��Ȩ��Ϣ:

     (c) �������, 2014.

 * ��ʷ�汾:
     2014-11-8:
       - ��ʼ�汾 V1.0.0;

 * ��ע:
     - ʹ��ǰ��Ҫ��TM1638.h�ļ�����ͨ�Žӿڡ����������ͳ�ʼ������
-------------------------------------------------------------------------------------*/

/************************����ͷ�ļ�***************************************************/
#include	"STC15Fxxxx.H"
#include "TM1668.h"
#include "plat.h"
#include	"config.h"
/************************�������***************************************************/

unsigned char code Seg_test[8]=      {0x20,0x01,0x02,0x04,0x08,0x10,0x40,0x80};						 //�˶��������ʾ��ÿһ��
unsigned char code Number_arr[10]=   {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};			 //���������������0-9��������
unsigned char code Character_arr[53]={0x77/*A*/,0xff,0x39/*C*/,0xff,0x79/*E*/,0x71/*F*/,0xff,0x76/*H*/,
									  0xff,0xff,0xff,0x38/*L*/,0xff,0xff,0xff,				 				 //����ܿ���ʾ�ַ������
									  0x73/*P*/,0x67/*Q*/,0xff,0xff,0xff,0x3e/*U*/,
									  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
									  0x08/*_*/,0xff,0xff,0x7c/*b*/,			 							 //����_,A,C,E,F,H,L,P,Q,U,-,b,c,d,h,l,n,o,u
									  0x58/*c*/,0x5e/*d*/,0xff,0xff,0xff,0x74/*h*/,0xff,0xff,0xff,0x30/*l*/,0xff,
									  0x54/*n*/,0x5c/*o*/,0xff,0xff,0x50/*r*/,0xff,0xff,0x1c/*u*/};
/*************************************************************************************
*��������void DisplayInterfaceInit(void)
*��  �ܣ���ʼ����ʾ�ӿ�
*˵  ����
**************************************************************************************/
void DisplayInterfaceInit(void)
{
	Init1668Interface();
}
/*************************************************************************************
*��������void NoDecodeDigiDisplay(unsigned char start,unsigned char* arr,unsigned char len)
*��  �ܣ���ʾһ�����������,�޳�����룬����ı������û����壬
*˵  ����start������ʼλ����1-8��arr����Ҫ��ʾ�����飬len������Ҫ��ʾ��λ
**************************************************************************************/
void NoDecodeDigiDisplay(unsigned char start,unsigned char* arr,unsigned char len)
{
	DisplayArr_TM1668(start,arr,len);
}
/*************************************************************************************
*��������void NoDecodeDigiDisplay(unsigned char start,unsigned char* arr,unsigned char len)
*��  �ܣ���ʾһ�����������,�޳�����룬����ı������û����壬
*˵  ����digi����Ҫ��ʾ��λ�ã���1-8��c����Ҫ��ʾ���ַ�
**************************************************************************************/
void NoDecodeDisplayOneDigi(unsigned char digi,unsigned char c)
{
	DisplayOneDigi_TM1668(digi,c);
}
/*************************************************************************************
*��������void DisplayOneDigi(unsigned char digi,unsigned char cha,unsigned char dot)
*��  �ܣ���ָ��λ��ʾһ�����ֻ��ַ�
*˵  ����digi����Ҫ��ʾ��λ�ã���1-8��c����Ҫ��ʾ���ַ���dot�����Ƿ���ʾС����
**************************************************************************************/
void DisplayOneDigi(unsigned char digi,unsigned char c,unsigned char dot)
{
	unsigned char tmp;
	
	if((dot&0x01)==0x01)							//�ж��Ƿ���ʾС���㣬���dot����Ϊ1������ʾС����
	{
		dot=0x80;
	}
	else
	{
		dot=0;
	}
	if(c>=0&&c<=9)									//�ж��Ƿ�Ϊ����
	{
		tmp=Number_arr[c]|dot;
	}
	else if(c>=65&&c<=117)							//�ж��Ƿ�Ϊ�ַ��ӡ�A'����u��
	{
		tmp=Character_arr[(c-65)]|dot;
	}
	else if(c==45)									//�ж��Ƿ�Ϊ��-����
	{
		tmp=0x01|dot;
	}
	else if(c==32)									//��ʾ�հ�
	{
		tmp=0x00|dot;
	} 
	else if(c==61)									//�ж��Ƿ�Ϊ��=����
	{
		tmp=0x09|dot;
	}
	else if(c==46)									//�ж��Ƿ�Ϊ��.����
	{
		tmp=0x80|dot;
	}
		
	DisplayOneDigi_TM1668(digi,tmp);				//��ʾ����
}
/***************************************END********************************************/
