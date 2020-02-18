#include "reg52.h"			 //此文件中定义了单片机的一些特殊功能寄存器
#include<intrins.h>		

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

//定义数码管段选和位选控制管脚
#define SMG_PORT P0
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;



//定义电机控制管脚
sbit dian1=P2^0;//左电机	 
sbit dian2=P2^1;
sbit dian3=P2^5;//右电机	 
sbit dian4=P2^6;

//定义超声模块控制管脚
//TRIG 为控制端
sbit TRIG=P1^6;//超声波的TRIG端接在了P1.6口
//ECHO接收端
sbit ECHO=P1^7;//超声波的ECHO端接在了P1.7口

u16 k=1;//判断是否起动超声波
//定义数码管段码数据的储存空间
u8 DisplayData[4];
u8 code smgduan[17]={
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
//0、1、2、3、4、5、6、7、8、9、A、b、C、d、E、F、H的显示码

/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/
void delay(u16 us)
{
	while(us--);	
}


/*******************************************************************************
* 函数名         :DigDisplay()
* 函数功能		 :数码管显示函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void DigDisplay()
{
	u8 i;
	for(i=0;i<4;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSA=0;LSB=0;LSC=0;break;//显示第0位
			case(1):
				LSA=1;LSB=0;LSC=0;break;//显示第1位
			case(2):
				LSA=0;LSB=1;LSC=0;break;//显示第2位	
			case(3):
				LSA=1;LSB=1;LSC=0;break;//显示第3位	
		}
		SMG_PORT=DisplayData[i];//发送数据
		delay(100); //间隔一段时间扫描	
		SMG_PORT=0x00;//消隐
	}		
}

void Car_ForwardRun(void)//前进
{
	 dian1=1;		 //左电机	  
		dian2=0;		//左电机 
    delay(2000);
		dian1=0;
	  dian2=0;	
    delay(2000);			
		dian3=1;		 //右电机  
		dian4=0;		 //右电机   
		delay(2000);
		dian3=0;
	  dian4=0;
		delay(3000);
}

void Car_BackwardRun(void)//后退
{
	 dian1=0;		 //左电机	   
		dian2=1;		 //左电机
		delay(2000);
		dian1=0;
	  dian2=0;
		delay(100);
		dian3=0;		 //右电机    
		dian4=1;		 //右电机    
		delay(2000);
		dian3=0;
	  dian4=0;
		delay(6000);	
}

void Car_LelfRun(void)//左转
{
	dian1=1;		 //左电机	·
		dian2=0;		 //左电机  
		dian3=0;		 //右电机    
		dian4=0;	   //右电机  
		delay(20);
		dian1=0;
	  dian2=0;
		dian3=0;
	  dian4=0;
		delay(80);	
}

void Car_RightRun(void)//右转
{
	 dian1=0;		 //左电机	
		dian2=0;		 //左电机  
		dian3=1;		 //右电机    
		dian4=0;	   //右电机  
		delay(20);
		dian1=0;
	  dian2=0;
		dian3=0;
	  dian4=0;
		delay(80);	
}

void Car_StopRun(void)//停止
{
	    dian1=0;
	    dian2=0;
	    dian3=0;
	    dian4=0;
}

u8 overflow_flag=0;//表志定时器是否溢出

void delay_20us(void)//延时20us秒
{
 _nop_();_nop_();_nop_();_nop_();_nop_();
 _nop_();_nop_();_nop_();_nop_();_nop_();
 _nop_();_nop_();_nop_();_nop_();_nop_();
 _nop_();_nop_();_nop_();_nop_();_nop_();
}

void Timer0Init(void)
{
	TMOD=0x01;//选择为定时器0模式，工作方式为1，仅用TRO打开起动
	TH0=0;//给定时器赋初值
	TL0=0;
	ET0=1;//打开定时器1，中断允许
	EA=1;//打开总中断
	TR0=0;
}
void Timer0() interrupt 1
{
	overflow_flag=1;//溢出标址位置
}
 
u16 Get_Distanc(void)//测距范围为0-4000cm，单位cm
{
	u16 time_data,out_THO,out_TLO;
	
	TRIG=0;//先给控制端初始化为0
	
	Timer0Init();//初始化定时器
	overflow_flag=0;//置溢出标志位为0
	//控制口发一个10us以上的高电平
	if(k==1)
	{
	TRIG=1;
	}
	delay_20us();
	TRIG=0;
	//等待接收端出现高电平
	while(!ECHO);
	TR0=1;//启动定时器，开始计时
	while(ECHO);//等待高电平过去
	TR0=0;//关闭低电平
	
	out_THO=TH0;//取定时器的值
	out_TLO=TL0;
	time_data=out_THO*256+out_TLO;
	time_data=time_data*0.017;
	if(overflow_flag==1)
  {
	time_data=4000;
	}
	
	return time_data;//返回
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


