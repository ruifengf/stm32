//单片机头文件
#include "stm32f4xx.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "led.h"
#include "delay.h"
#include "fan.h"
#include "humidifier.h"
#include "heater.h"
//C库
#include <string.h>
#include <stdio.h>


#define PROID		"424929"    //产品ID

#define AUTH_INFO	  "123abc"   //鉴权信息

#define DEVID		"716233397"  //设备ID

extern unsigned char esp8266_buf[128];

//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	printf("正在连接云服务器\r\n产品ID: %s,	鉴权信息: %s,设备ID:%s\r\n", PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:printf("云服务器连接成功\r\n");status = 0;break;
					
					case 1:printf("WARN:	连接失败：协议错误\r\n");break;
					case 2:printf("WARN:	连接失败：非法的clientid\r\n");break;
					case 3:printf("WARN:	连接失败：服务器失败\r\n");break;
					case 4:printf("WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:printf("WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:printf("ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		printf("WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
extern float value_light; 	    //检测到的光照度
extern float value_temperature; //检测到的温度
extern float value_humidity; 	  //检测到的湿度
extern float value_CO2; 	      //检测到的CO2浓度

u8 LED_Light;				//下发的小灯亮度
u8 SmartOrManual = 1;				//智能调光模式

u8 light_control = 0;				//下发的开关灯命令
u8 heater_control = 0;				//下发的开关加热器命令
u8 humidifier_control = 0;				//下发的开关加湿器命令
u8 fan_control = 0;				//下发的开关风扇命令
u8 intelligent_mode = 0;				//智能模式

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
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[256];
	
	short body_len = 0, i = 0;
	
//	printf("Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
//			printf("Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			printf("WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
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
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				//printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					//printf("Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				//printf("Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, ':');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
		if(strstr((char *)req_payload, "heater"))		//搜索"LED_Light"
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
