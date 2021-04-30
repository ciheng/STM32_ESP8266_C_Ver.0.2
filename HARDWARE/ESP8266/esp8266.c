#include "esp8266.h"
#include "string.h"
#include "usart.h"
#include "usart3.h"
#include "stm32f4xx.h"
#include "sys.h" 
#include "delay.h"
#include "stdlib.h"
#include "malloc.h"

 char *ipbuf;
 u8 *port = "8086";							//�Զ���
// u8 workmode; 							//����ģʽ 1��stationģʽ   2��APģʽ  3������ AP+stationģʽ
 u8 p[48];	
//p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
 u8* wifi_msg;

void atk_8266_init(void)
{						
//WIFI APģʽ,ģ���������߲���,�������޸�.
sprintf((char*)p,"AT+CWSAP=\"ESP8266-TEST\",\"12345678\",5,3");    //����ģ��APģʽ���߲���
while(esp8266_send_cmd(p,"OK",100));

}
//����WIFI��Ϣ������
void set_wifi_sta(u8* ssid, u8* encryp, u8* pswd)    
{
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",ssid,pswd);
	while(esp8266_send_cmd(p,"OK",300));
}

//���ù���ģʽ 1��stationģʽ   2��APģʽ  3������ AP+stationģʽ
u8 esp8266_set_mode(u8 mode)
{
	while(1)
	{
		if(esp8266_send_cmd("AT","OK",50)==1)
		{
		esp8266_quit_trans();//�˳�͸��
		esp8266_send_cmd("AT+CIPMODE=0","OK",50);  //�ر�͸��ģʽ
		delay_ms(800);
		}else{	break; }
	}
	//workmode=mode;
	sprintf((char*)p,"AT+CWMODE=%d",mode);
	esp8266_send_cmd(p,"OK",50);
	return mode;
}

void set_smartConfig(u8 smartType)
{
	esp8266_send_cmd("AT+CWSTARTSMART","OK",50);
		while(1)
	{
		delay_ms(20);
		if(esp8266_send_cmd("AT+CIPSTATUS","2",50)==0)
		{
			esp8266_send_cmd("AT+CWSTOPSMART","OK",50);
			break;
		}
	}
	
	
}

//AP:�Լ��趨��̬IP�� STA��·����DHCP����IP
void atk_8266_ip_set(u8 mode,char* ip)  
{
	ipbuf=ip;	//д��Զ��IP��ַ
	if(mode==1||mode== 3)  //����������AT+CIFSR,�����ERROR������
	{


	}
	if(mode==2 || mode==3)
	{
		//printf("AP mode//IP adress of ESP8266 is:192.168.4.1");
		esp8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.4.1\",8086","OK",100);	//AP mode, TCP/IP���ã��ɸ���
	}

}

//����AP���ӷ�ʽ��1:UDP, 2: TCP Client, 3: TCP Server 
void esp8266_ap_mode_config(u8 mode)
{

	if(mode==1)   //UDP
	{
				sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)port);    //����Ŀ��UDP������
				esp8266_send_cmd("AT+CIPMUX=0","OK",50);  //������ģʽ
				while(esp8266_send_cmd(p,"OK",50));
	}
	else     //TCP
	{
		if(mode==2)     //TCP Client    ͸��ģʽ����
		{
			esp8266_send_cmd("AT+CIPMUX=0","OK",50);   //0�������ӣ�1��������
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)port);    //����Ŀ��TCP������
			while(esp8266_send_cmd(p,"OK",100))
			esp8266_send_cmd("AT+CIPMODE=1","OK",100);      //����ģʽΪ��͸��		
			}
		else					//TCP Server
			{
				
					esp8266_send_cmd("AT+CIPMUX=1","OK",50);   //0�������ӣ�1��������
					sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)port);
					esp8266_send_cmd(p,"OK",50);     //����Serverģʽ���˿ں�Ϊ8086
			}

}
	}

//����STA���ӷ�ʽ��1:UDP, 2: TCP Client, 3: TCP Server //��AP����û������
void esp8266_sta_mode_config(u8 mode)
{
		if(mode==1)   //UDP
	{
				sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)port);    //����Ŀ��UDP������
				delay_ms(200);
				esp8266_send_cmd("AT+CIPMUX=0","OK",50);  //������ģʽ
				delay_ms(200);
				while(esp8266_send_cmd(p,"OK",50));
	}
	else     //TCP
	{
		if(mode==2)     //TCP Client    ͸��ģʽ����
		{
			esp8266_send_cmd("AT+CIPMUX=0","OK",50);   //0�������ӣ�1��������
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)port);    //����Ŀ��TCP������
			while(esp8266_send_cmd(p,"OK",50))
			{
				printf("����ʧ��");		//�Զ�������ʧ�ܵĴ���
				delay_ms(500);
				break;
			}	
			esp8266_send_cmd("AT+CIPMODE=1","OK",50);      //����ģʽΪ��͸��			
		}
		else					//TCP Server
		{/////ERROR
				esp8266_send_cmd("AT","OK",20);
				esp8266_send_cmd("AT+CIPMUX=1","0K",50);   //0�������ӣ�1��������
				sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)port);    //����Serverģʽ(0���رգ�1����)���˿ں�Ϊportnum
				esp8266_send_cmd(p,"OK",50);    
		}
} 	
	}
//����STA/AP���ӷ�ʽ��1:UDP, 2: TCP Client, 3: TCP Server 
void esp8266_mode_config(u8 mode) 
{
			if(mode==1)   //STA UDP
	{
				sprintf((char*)p,"AT+CIPSTART=0,\"UDP\",\"%s\",%s",ipbuf,(u8*)port);    //����Ŀ��UDP������,��ID�ţ�STAģʽ��Ϊ0
				esp8266_send_cmd(p,"OK",50);
				esp8266_ap_mode_config(mode);     //APģʽ����ģʽ����			
	}
	else     //TCP
	{
		if(mode==2)     //STA TCP Client  
		{
			sprintf((char*)p,"AT+CIPSTART=0,\"TCP\",\"%s\",%s",ipbuf,(u8*)port);    //����Ŀ��TCP������,��ID�ţ�STAģʽ��Ϊ0
			while(esp8266_send_cmd(p,"OK",50))
			{
				printf("����ʧ��");		//�Զ�������ʧ�ܵĴ���
				delay_ms(500);
				break;
			}	
			esp8266_ap_mode_config(mode);     //APģʽ����ģʽ����	
		}
		else esp8266_ap_mode_config(mode);      //TCP SERVER��������
	}
	}
	

//ESP8266ģ���PC����͸��ģʽ
void esp8266_start_trans(void)
{	
	esp8266_send_cmd("AT+CIPMODE=1","OK",50);
	//͸��ģʽ�� ��ʼ�������ݵ�ָ�� ���ָ��֮��Ϳ���ֱ�ӷ�������//AT+CIPSEND=<link ID>,<length>
	//esp8266_send_cmd("AT+CIPSENDEX=0,32","OK",50);
}

//ESP8266�˳�͸��ģʽ   ����ֵ:0,�˳��ɹ�;1,�˳�ʧ��
//����wifiģ�飬ͨ����wifiģ����������3��+��ÿ��+��֮�� ����10ms,������Ϊ���������η���+��
u8 esp8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0);	//�ȴ����Ϳ�
	USART3->DR='+';      
	delay_ms(500);					//�ȴ�500ms
	return esp8266_send_cmd("AT","OK",20);//�˳�͸���ж�.
}


//��ESP8266��������
//cmd:���͵������ַ���;ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��;waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����);1,����ʧ��
u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(esp8266_check_cmd(ack))
				{
					//printf("ack:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)
			res=1; 
	}
	return res;
} 


//ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����;����,�ڴ�Ӧ������λ��(str��λ��)
u8* esp8266_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//��ESP8266��������
//cmd:���͵������ַ���;waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:�������ݺ󣬷������ķ�����֤��
u8 esp8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	esp8266_send_cmd("AT+CIPSENDEX=0,32","OK",50);
	delay_ms(50);
	u3_printf("%s\r\n\\0",data);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(esp8266_check_cmd(ack))break;//�õ���Ч���� 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		printf("%s",USART3_RX_BUF);	//���͵�����
		if(mode)USART3_RX_STA=0;
	} 
}
//STA����Զ������
u8* esp8266_receive_data()
{
	static char wifi_msg[200];
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{
		//printf("!USART3_RX_BUF=%s\r\n",USART3_RX_BUF);//�򴮿�һ��ӡ���ݣ���֤�����յ�������
		memcpy(wifi_msg, USART3_RX_BUF,sizeof(USART3_RX_BUF));
		printf("wifi_msg is:%s\r\n",wifi_msg);	//���͵����� 
		
		USART3_RX_STA=0;
		memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
		delay_ms(300);
	}
	return  wifi_msg;

}
