//��Ƭ��ͷ�ļ�
#include "stm32f4xx.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
#include "usart.h"
#include "led.h"
#include "delay.h"
#include "fan.h"
#include "humidifier.h"
#include "heater.h"
//C��
#include <string.h>
#include <stdio.h>


#define PROID		"424929"    //��ƷID

#define AUTH_INFO	  "123abc"   //��Ȩ��Ϣ

#define DEVID		"716233397"  //�豸ID

extern unsigned char esp8266_buf[128];

//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	printf("���������Ʒ�����\r\n��ƷID: %s,	��Ȩ��Ϣ: %s,�豸ID:%s\r\n", PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:printf("�Ʒ��������ӳɹ�\r\n");status = 0;break;
					
					case 1:printf("WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:printf("WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:printf("WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:printf("WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:printf("WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
					
					default:printf("ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		printf("WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
extern float value_light; 	    //��⵽�Ĺ��ն�
extern float value_temperature; //��⵽���¶�
extern float value_humidity; 	  //��⵽��ʪ��
extern float value_CO2; 	      //��⵽��CO2Ũ��

u8 LED_Light;				//�·���С������
u8 SmartOrManual = 1;				//���ܵ���ģʽ

u8 light_control = 0;				//�·��Ŀ��ص�����
u8 heater_control = 0;				//�·��Ŀ��ؼ���������
u8 humidifier_control = 0;				//�·��Ŀ��ؼ�ʪ������
u8 fan_control = 0;				//�·��Ŀ��ط�������
u8 intelligent_mode = 0;				//����ģʽ

u8 loop = 0;

unsigned char OneNet_FillBuf(char *buf)
{
	char text[32];
	
	if (loop == 2)
	{
		 value_CO2 = value_CO2 + 22.0;
		 value_light = value_light + 3.0;
	   value_temperature = value_temperature + 1.0;
	   value_humidity = value_humidity + 1.0;
	}
	
	if (loop == 5)
	{
		 value_CO2 = value_CO2 + 15.0;
		 value_light = value_light + 2.0;
	   value_temperature = value_temperature + 1.0;
	   value_humidity = value_humidity + 1.0;
	}
	
	if (loop == 4)
	{
		 value_CO2 = value_CO2 - 17.0;
		 value_light = value_light - 2.0;
	   value_temperature = value_temperature - 1.0;
	   value_humidity = value_humidity - 1.0;
	}
	
  if (loop == 7)
	{
		 value_CO2 = value_CO2 - 21.0;
		 value_light = value_light - 3.0;
	   value_temperature = value_temperature - 1.0;
	   value_humidity = value_humidity - 1.0;
		loop = 0;
	}
	loop++;
	memset(text, 0, sizeof(text));
	
	strcpy(buf, ",;");
		
	memset(text, 0, sizeof(text));
	sprintf(text, "temperature,%lf;", value_temperature);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "humidity,%lf;", value_humidity);
	strcat(buf, text);
		
	memset(text, 0, sizeof(text));
	sprintf(text, "CO2,%.1f;", value_CO2);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "light,%.1f;", value_light);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "light_control,%d;", light_control);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "heater_control,%d;", heater_control);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "humidifier_control,%d;", humidifier_control);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "fan_control,%d;", fan_control);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "intelligent_mode,%d;", intelligent_mode);
	strcat(buf, text);
	
	return strlen(buf);
}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[256];
	
	short body_len = 0, i = 0;
	
//	printf("Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//���
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
//			printf("Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
		}
		else
			printf("WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				//printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
				{
					//printf("Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ظ�����
					MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				//printf("Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, ':');					//����'}'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
		
		if(strstr((char *)req_payload, "heater"))		//����"LED_Light"
		{
			  heater_control = num;
				heater_switch(heater_control);
		}
		else if(strstr((char *)req_payload, "humidifier"))
		{
			  humidifier_control = num;
				humidifier_switch(humidifier_control);
		}
		else if(strstr((char *)req_payload, "fan"))
		{
				fan_control = num;
				fan_switch(fan_control);
		}
		else if(strstr((char *)req_payload, "light"))
		{
			  light_control = num;
			  light_switch(light_control);
		}
		else if(strstr((char *)req_payload, "intelligent_mode"))
		{
			  intelligent_mode = num;
				printf("intelligent_mode = %d\r\n", num);
		}
		//delay_ms(4000);
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
