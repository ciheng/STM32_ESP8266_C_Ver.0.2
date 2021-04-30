#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart3.h"
#include "esp8266.h"
#include "string.h"
#include "timer.h"
#include "beep.h"
#include "led.h"
 
/*
main�����Զ�����������ڲ���esp8266.c
*/

//���Ӷ˿ں�:8086,�������޸�Ϊ�����˿�.
 extern u8 *port;		
//WIFI STAģʽ,����Ҫȥ���ӵ�·�������߲���,��������Լ���·��������,�����޸�.
 u8* wifista_ssid="meeting";			//·����SSID��
 u8* wifista_encryption="WPA_WAP2_PSK";	//wpa/wpa2 aes���ܷ�ʽ  wpawpa2_aes
 u8* wifista_password="1234567890"; 	//��������

 u8 work_mode= 1;//����ģʽ 1��stationģʽ   2��APģʽ  3������ AP+stationģʽ
 u8 connect_mode= 3;//���ӷ�ʽ��1:UDP, 2: TCP Client, 3: TCP Server
 u8* PC_IP= "192.168.0.110";				//Զ���豸IP  ->����ͨ��ָ����Ӧ�ü���ȡ�ĺ���
 u8 smartCnfigType= 3; //SmartConfig ģʽ�� 1.ESP-TOUCH 2.Airkiss 3.���
extern flag;
u8 RX_data[sizeof(USART3_RX_BUF)];
 int main(void)
 {		
	
	delay_init(50);	    	 									//��ʱ������ʼ��		 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 			//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 										//���ڳ�ʼ��Ϊ115200
	usart3_init(115200);	 									//���ڳ�ʼ��Ϊ115200
	LED_Init();				   									//��ʼ��LED�˿�
	BEEP_Init();         										//��ʼ���������˿�
	
	esp8266_send_cmd("AT+RESTORE","OK",50);						//��ʼ��,������,������ɺ�disable
	 
	esp8266_set_mode(work_mode);								//���ù���ģʽ 1��stationģʽ   2��APģʽ  3������ AP+stationģʽ
	while(esp8266_send_cmd("AT+RST","",50));					//��ֹ����������ʱ������ack check
	
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);

	//atk_8266_init();		//APģʽ��ʼ���ã�APģʽʱenable
	 
	 //�û�����	 
	 
	set_wifi_sta(wifista_ssid, wifista_encryption, wifista_password); 		//STA����wifi��Ϣ����smartconfig��STAģʽʱenable
	
	//SmartConfigģʽ��1.ESP_Touch 2.Airkiss 3.Conbine...			//smartconfigģʽ��ESP8266����Ϊ��STAģʽ
	//set_smartConfig(smartCnfigType);							//д�������жϣ�  smartconfig�õ�AP���ݴ��ڱ�����
	
	 atk_8266_ip_set(work_mode,PC_IP);							
	 if(work_mode==1)
	 {esp8266_sta_mode_config(connect_mode);}
	 else if(work_mode==2)
	 {esp8266_ap_mode_config(connect_mode);}
	 else if(work_mode==3)
	 {esp8266_mode_config(connect_mode);}
	 

	delay_ms(1000);
	 
	while(esp8266_send_cmd("AT+CIPSTATUS","3",50))
		//esp8266_start_trans();										//esp8266���г�ʼ��,��ʼ͸�� 
		delay_ms(1000);										//���Ӱ��յ�����д��file�Ĺ���
		esp8266_send_data("this is ESP8266 speaking","",50);
		delay_ms(1000);

		while(1)
		{	
			u8* p;
			p = esp8266_receive_data();
			printf("return value is :%s\r\n",p);
			delay_ms(1000);
			if(strstr((char*)p,"on")) 			
			{
				
			GPIO_ResetBits(GPIOF,GPIO_Pin_9); // DS0���ͣ���   ��ͬLED0=0;
			GPIO_ResetBits(GPIOF,GPIO_Pin_8); //BEEP�������ͣ� ��ͬBEEP=0;
			delay_ms(300);                    //��ʱ300ms
			GPIO_SetBits(GPIOF,GPIO_Pin_9);   // DS0���ߣ���   ��ͬLED0=1;
			GPIO_SetBits(GPIOF,GPIO_Pin_8);   //BEEP�������ߣ� ��ͬBEEP=1;
			delay_ms(300); 
				
			
			}
		}
			esp8266_quit_trans();
	}
 

 
 

