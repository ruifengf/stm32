#include "dht11.h"

void Dht11_Init(void)
{
	GPIO_InitTypeDef  		GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	
	//PG9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//第9根引脚 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//引脚的速度最大为100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//没有使用内部上拉电阻
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
	
	//只要是有输出模式的引脚，就会有初始的电平状态，必须要看时序图
	DHT11_OUT_PIN=1;
}


int Dht11_Start(void)
{
	uint32_t t=0;
	GPIO_InitTypeDef  		GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//第0根引脚 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//引脚的速度最大为100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//没有使用内部上拉电阻
	GPIO_Init(GPIOE, &GPIO_InitStructure);		
	DHT11_OUT_PIN=1;	

	Delay_us(38);
	DHT11_OUT_PIN=0;
	
	Delay_ms(20);
	
	DHT11_OUT_PIN=1;	

	Delay_us(38);
	//DHT11_OUT_PIN=0;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//第9根引脚 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			//输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//引脚的速度最大为100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//没有使用内部上拉电阻
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	//等待低电平出现
	t=0;
	while(DHT11_IN_PIN)
	{
		Delay_us(10);
		
		t++;
		
		if(t>=100)
		{
			return -12;
		}
	}
	
	
	//超时检测电平的有效性
	t=0;
	while(DHT11_IN_PIN == 0)
	{
		Delay_us(10);
		
		t++;
		
		if(t>=10)
			return -2;

	}
	
	//超时检测电平的有效性
	t=0;
	while(DHT11_IN_PIN)
	{
		Delay_us(10);
		
		t++;
		
		if(t>=10)
			return -3;

	}	
	
	return 0;
}


int32_t Dht11_Read_Byte(uint8_t *pbuf)
{
	uint32_t t=0;
	
	uint8_t *p = pbuf;
	
	uint8_t  d=0;
	
	int32_t  i=0;
	
	for(i=7; i>=0; i--)
	{
	
		//等待低电平持续完毕
		t=0;
		while(DHT11_IN_PIN==0)
		{
			Delay_us(10);
			
			t++;
			
			if(t>=10)
				return -4;

		}
		
		//延时40us
		Delay_us(40);
		
		if(DHT11_IN_PIN)
		{
			//将d变量对应的比特位置1
			d|=1<<i;
			
			//超时检测电平的有效性
			t=0;
			while(DHT11_IN_PIN)
			{
				Delay_us(10);
				
				t++;
				
				if(t>=10)
					return -5;

			}				
		}	
	
	}

	*p=d;
	
	
	return 0;
}


int32_t Dht11_Read_Data(uint8_t *pbuf)
{
	int32_t rt;
	int32_t i=0;
	uint8_t *p = pbuf;
	uint8_t check_sum=0;
	
	rt = Dht11_Start();
	
	
	if(rt < 0)
	{
		return rt;
	}
		
	
	//连续读取5个字节
	for(i=0; i<5; i++)
	{

		rt = Dht11_Read_Byte(&p[i]);
		
		if(rt < 0)
			return rt;
		

	
	}
	
	//通讯的结束
	Delay_us(50);
	
	//计算校验和
	check_sum=(p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(check_sum != p[4])
		return -7;
	
	return 0;
	



}


