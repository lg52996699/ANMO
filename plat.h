/*-------------------------------------------------------------------------------------
 * ��Ŀ����:

     TM1668ģ����������v1.0

 * ��Ȩ��Ϣ:

     (c) �������, 2016.

 * ��ʷ�汾:
     2016-5-11:
       - ��ʼ�汾 V1.0.0;

 * ��ע:
     - ʹ��ǰ��Ҫ������Ľӿ����ò�������ͨ�Žӿ�
	 - �ڴ����������������ü����ĸ��������ֻһ��������Ϊ1���Դ�����

----------------------------------------------------------------------------------------*/

#ifndef _PLAT_H_
#define _PLAT_H_

/*---------------------------��������---------------------------------------------------*/

extern unsigned char code Seg_test[8];			//�˶�����ܵ�ÿһ��
extern unsigned char code Dign[8];				//��λ����������
extern unsigned char code Number_arr[10];		//����0-9���߶�����ܵı�������
extern unsigned char code Character_arr[53];	//����ܿ���ʾ�ַ����˶�����ܵı������飬
												//����_,A,C,E,F,H,L,P,Q,U,-,b,c,d,h,l,n,o,u

/*---------------------------��������-----------------------------------------------------*/

void DisplayInterfaceInit(void);
void NoDecodeDigiDisplay(unsigned char start,unsigned char* arr,unsigned char len);
void NoDecodeDisplayOneDigi(unsigned char digi,unsigned char c);
void DisplayOneDigi(unsigned char digi,unsigned char c,unsigned char dot);
#endif