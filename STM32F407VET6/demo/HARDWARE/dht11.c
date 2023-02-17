#include "dht11.h"

void Dht11_Init(void)
{
	GPIO_InitTypeDef  		GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	
	//PG9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//��9������ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�����������ǿ�������������ŵ������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//���ŵ��ٶ����Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//û��ʹ���ڲ���������
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
	
	//ֻҪ�������ģʽ�����ţ��ͻ��г�ʼ�ĵ�ƽ״̬������Ҫ��ʱ��ͼ
	DHT11_OUT_PIN=1;
}


int Dht11_Start(void)
{
	uint32_t t=0;
	GPIO_InitTypeDef  		GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//��0������ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�����������ǿ�������������ŵ������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//���ŵ��ٶ����Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//û��ʹ���ڲ���������
	GPIO_Init(GPIOE, &GPIO_InitStructure);		
	DHT11_OUT_PIN=1;	

	Delay_us(38);
	DHT11_OUT_PIN=0;
	
	Delay_ms(20);
	
	DHT11_OUT_PIN=1;	

	Delay_us(38);
	//DHT11_OUT_PIN=0;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//��9������ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			//����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�����������ǿ�������������ŵ������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//���ŵ��ٶ����Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//û��ʹ���ڲ���������
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	//�ȴ��͵�ƽ����
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
	
	
	//��ʱ����ƽ����Ч��
	t=0;
	while(DHT11_IN_PIN == 0)
	{
		Delay_us(10);
		
		t++;
		
		if(t>=10)
			return -2;

	}
	
	//��ʱ����ƽ����Ч��
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
	
		//�ȴ��͵�ƽ�������
		t=0;
		while(DHT11_IN_PIN==0)
		{
			Delay_us(10);
			
			t++;
			
			if(t>=10)
				return -4;

		}
		
		//��ʱ40us
		Delay_us(40);
		
		if(DHT11_IN_PIN)
		{
			//��d������Ӧ�ı���λ��1
			d|=1<<i;
			
			//��ʱ����ƽ����Ч��
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
		
	
	//������ȡ5���ֽ�
	for(i=0; i<5; i++)
	{

		rt = Dht11_Read_Byte(&p[i]);
		
		if(rt < 0)
			return rt;
		

	
	}
	
	//ͨѶ�Ľ���
	Delay_us(50);
	
	//����У���
	check_sum=(p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(check_sum != p[4])
		return -7;
	
	return 0;
	



}


