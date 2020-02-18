#include "reg52.h"			 //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#include<intrins.h>		

typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;

//��������ܶ�ѡ��λѡ���ƹܽ�
#define SMG_PORT P0
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;



//���������ƹܽ�
sbit dian1=P2^0;//����	 
sbit dian2=P2^1;
sbit dian3=P2^5;//�ҵ��	 
sbit dian4=P2^6;

//���峬��ģ����ƹܽ�
//TRIG Ϊ���ƶ�
sbit TRIG=P1^6;//��������TRIG�˽�����P1.6��
//ECHO���ն�
sbit ECHO=P1^7;//��������ECHO�˽�����P1.7��

u16 k=1;//�ж��Ƿ��𶯳�����
//��������ܶ������ݵĴ���ռ�
u8 DisplayData[4];
u8 code smgduan[17]={
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
//0��1��2��3��4��5��6��7��8��9��A��b��C��d��E��F��H����ʾ��

/*******************************************************************************
* �� �� ��         : delay
* ��������		   : ��ʱ������i=1ʱ����Լ��ʱ10us
*******************************************************************************/
void delay(u16 us)
{
	while(us--);	
}


/*******************************************************************************
* ������         :DigDisplay()
* ��������		 :�������ʾ����
* ����           : ��
* ���         	 : ��
*******************************************************************************/
void DigDisplay()
{
	u8 i;
	for(i=0;i<4;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LSA=0;LSB=0;LSC=0;break;//��ʾ��0λ
			case(1):
				LSA=1;LSB=0;LSC=0;break;//��ʾ��1λ
			case(2):
				LSA=0;LSB=1;LSC=0;break;//��ʾ��2λ	
			case(3):
				LSA=1;LSB=1;LSC=0;break;//��ʾ��3λ	
		}
		SMG_PORT=DisplayData[i];//��������
		delay(100); //���һ��ʱ��ɨ��	
		SMG_PORT=0x00;//����
	}		
}

void Car_ForwardRun(void)//ǰ��
{
	 dian1=1;		 //����	  
		dian2=0;		//���� 
    delay(2000);
		dian1=0;
	  dian2=0;	
    delay(2000);			
		dian3=1;		 //�ҵ��  
		dian4=0;		 //�ҵ��   
		delay(2000);
		dian3=0;
	  dian4=0;
		delay(3000);
}

void Car_BackwardRun(void)//����
{
	 dian1=0;		 //����	   
		dian2=1;		 //����
		delay(2000);
		dian1=0;
	  dian2=0;
		delay(100);
		dian3=0;		 //�ҵ��    
		dian4=1;		 //�ҵ��    
		delay(2000);
		dian3=0;
	  dian4=0;
		delay(6000);	
}

void Car_LelfRun(void)//��ת
{
	dian1=1;		 //����	��
		dian2=0;		 //����  
		dian3=0;		 //�ҵ��    
		dian4=0;	   //�ҵ��  
		delay(20);
		dian1=0;
	  dian2=0;
		dian3=0;
	  dian4=0;
		delay(80);	
}

void Car_RightRun(void)//��ת
{
	 dian1=0;		 //����	
		dian2=0;		 //����  
		dian3=1;		 //�ҵ��    
		dian4=0;	   //�ҵ��  
		delay(20);
		dian1=0;
	  dian2=0;
		dian3=0;
	  dian4=0;
		delay(80);	
}

void Car_StopRun(void)//ֹͣ
{
	    dian1=0;
	    dian2=0;
	    dian3=0;
	    dian4=0;
}

u8 overflow_flag=0;//��־��ʱ���Ƿ����

void delay_20us(void)//��ʱ20us��
{
 _nop_();_nop_();_nop_();_nop_();_nop_();
 _nop_();_nop_();_nop_();_nop_();_nop_();
 _nop_();_nop_();_nop_();_nop_();_nop_();
 _nop_();_nop_();_nop_();_nop_();_nop_();
}

void Timer0Init(void)
{
	TMOD=0x01;//ѡ��Ϊ��ʱ��0ģʽ��������ʽΪ1������TRO����
	TH0=0;//����ʱ������ֵ
	TL0=0;
	ET0=1;//�򿪶�ʱ��1���ж�����
	EA=1;//�����ж�
	TR0=0;
}
void Timer0() interrupt 1
{
	overflow_flag=1;//�����ַλ��
}
 
u16 Get_Distanc(void)//��෶ΧΪ0-4000cm����λcm
{
	u16 time_data,out_THO,out_TLO;
	
	TRIG=0;//�ȸ����ƶ˳�ʼ��Ϊ0
	
	Timer0Init();//��ʼ����ʱ��
	overflow_flag=0;//�������־λΪ0
	//���ƿڷ�һ��10us���ϵĸߵ�ƽ
	if(k==1)
	{
	TRIG=1;
	}
	delay_20us();
	TRIG=0;
	//�ȴ����ն˳��ָߵ�ƽ
	while(!ECHO);
	TR0=1;//������ʱ������ʼ��ʱ
	while(ECHO);//�ȴ��ߵ�ƽ��ȥ
	TR0=0;//�رյ͵�ƽ
	
	out_THO=TH0;//ȡ��ʱ����ֵ
	out_TLO=TL0;
	time_data=out_THO*256+out_TLO;
	time_data=time_data*0.017;
	if(overflow_flag==1)
  {
	time_data=4000;
	}
	
	return time_data;//����
}

void Distance_Datapros(u16 value)
{
	  DisplayData[0] = smgduan[value%10];
		DisplayData[1] = smgduan[value%100/10];
		DisplayData[2] = smgduan[value%1000/100];
	  DisplayData[3] = smgduan[value/1000];
}
void main()
{	
	u16 distane=0;
	Timer0Init();
	while(1)
	{
		
		distane=Get_Distanc();
		Distance_Datapros(distane);
		DigDisplay();
		 if(distane<50)
		{
			Car_BackwardRun();
		}
	   if(distane>50)
		{
			Car_ForwardRun();
		}
		 if(distane==50)
		{
			 Car_StopRun();
		}
	}
}


