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
main包含自定义参数，用于测试esp8266.c
*/

//连接端口号:8086,可自行修改为其他端口.
 extern u8 *port;		
//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
 u8* wifista_ssid="meeting";			//路由器SSID号
 u8* wifista_encryption="WPA_WAP2_PSK";	//wpa/wpa2 aes加密方式  wpawpa2_aes
 u8* wifista_password="1234567890"; 	//连接密码

 u8 work_mode= 1;//工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
 u8 connect_mode= 3;//连接方式：1:UDP, 2: TCP Client, 3: TCP Server
 u8* PC_IP= "192.168.0.110";				//远端设备IP  ->增加通过指令响应裁剪获取的函数
 u8 smartCnfigType= 3; //SmartConfig 模式： 1.ESP-TOUCH 2.Airkiss 3.混合
extern flag;
u8 RX_data[sizeof(USART3_RX_BUF)];
 int main(void)
 {		
	
	delay_init(50);	    	 									//延时函数初始化		 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 			//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 										//串口初始化为115200
	usart3_init(115200);	 									//串口初始化为115200
	LED_Init();				   									//初始化LED端口
	BEEP_Init();         										//初始化蜂鸣器端口
	
	esp8266_send_cmd("AT+RESTORE","OK",50);						//初始化,调试用,调试完成后disable
	 
	esp8266_set_mode(work_mode);								//设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
	while(esp8266_send_cmd("AT+RST","",50));					//防止接受乱码暂时不设置ack check
	
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);

	//atk_8266_init();		//AP模式初始设置，AP模式时enable
	 
	 //用户配置	 
	 
	set_wifi_sta(wifista_ssid, wifista_encryption, wifista_password); 		//STA设置wifi信息，非smartconfig的STA模式时enable
	
	//SmartConfig模式：1.ESP_Touch 2.Airkiss 3.Conbine...			//smartconfig模式中ESP8266必须为单STA模式
	//set_smartConfig(smartCnfigType);							//写个按键中断？  smartconfig得到AP数据存在变量里
	
	 atk_8266_ip_set(work_mode,PC_IP);							
	 if(work_mode==1)
	 {esp8266_sta_mode_config(connect_mode);}
	 else if(work_mode==2)
	 {esp8266_ap_mode_config(connect_mode);}
	 else if(work_mode==3)
	 {esp8266_mode_config(connect_mode);}
	 

	delay_ms(1000);
	 
	while(esp8266_send_cmd("AT+CIPSTATUS","3",50))
		//esp8266_start_trans();										//esp8266进行初始化,开始透传 
		delay_ms(1000);										//增加把收到数据写入file的功能
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
				
			GPIO_ResetBits(GPIOF,GPIO_Pin_9); // DS0拉低，亮   等同LED0=0;
			GPIO_ResetBits(GPIOF,GPIO_Pin_8); //BEEP引脚拉低， 等同BEEP=0;
			delay_ms(300);                    //延时300ms
			GPIO_SetBits(GPIOF,GPIO_Pin_9);   // DS0拉高，灭   等同LED0=1;
			GPIO_SetBits(GPIOF,GPIO_Pin_8);   //BEEP引脚拉高， 等同BEEP=1;
			delay_ms(300); 
				
			
			}
		}
			esp8266_quit_trans();
	}
 

 
 

