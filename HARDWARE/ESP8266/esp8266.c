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
 u8 *port = "8086";							//自定义
// u8 workmode; 							//工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
 u8 p[48];	
//p=mymalloc(SRAMIN,32);							//申请32字节内存
 u8* wifi_msg;

void atk_8266_init(void)
{						
//WIFI AP模式,模块对外的无线参数,可自行修改.
sprintf((char*)p,"AT+CWSAP=\"ESP8266-TEST\",\"12345678\",5,3");    //配置模块AP模式无线参数
while(esp8266_send_cmd(p,"OK",100));

}
//输入WIFI信息并加入
void set_wifi_sta(u8* ssid, u8* encryp, u8* pswd)    
{
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",ssid,pswd);
	while(esp8266_send_cmd(p,"OK",300));
}

//设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
u8 esp8266_set_mode(u8 mode)
{
	while(1)
	{
		if(esp8266_send_cmd("AT","OK",50)==1)
		{
		esp8266_quit_trans();//退出透传
		esp8266_send_cmd("AT+CIPMODE=0","OK",50);  //关闭透传模式
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

//AP:自己设定静态IP， STA：路由器DHCP分配IP
void atk_8266_ip_set(u8 mode,char* ip)  
{
	ipbuf=ip;	//写入远端IP地址
	if(mode==1||mode== 3)  //不能在这里AT+CIFSR,后面会ERROR！！！
	{


	}
	if(mode==2 || mode==3)
	{
		//printf("AP mode//IP adress of ESP8266 is:192.168.4.1");
		esp8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.4.1\",8086","OK",100);	//AP mode, TCP/IP设置，可更改
	}

}

//设置AP连接方式：1:UDP, 2: TCP Client, 3: TCP Server 
void esp8266_ap_mode_config(u8 mode)
{

	if(mode==1)   //UDP
	{
				sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)port);    //配置目标UDP服务器
				esp8266_send_cmd("AT+CIPMUX=0","OK",50);  //单链接模式
				while(esp8266_send_cmd(p,"OK",50));
	}
	else     //TCP
	{
		if(mode==2)     //TCP Client    透传模式测试
		{
			esp8266_send_cmd("AT+CIPMUX=0","OK",50);   //0：单连接，1：多连接
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)port);    //配置目标TCP服务器
			while(esp8266_send_cmd(p,"OK",100))
			esp8266_send_cmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传		
			}
		else					//TCP Server
			{
				
					esp8266_send_cmd("AT+CIPMUX=1","OK",50);   //0：单连接，1：多连接
					sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)port);
					esp8266_send_cmd(p,"OK",50);     //开启Server模式，端口号为8086
			}

}
	}

//设置STA连接方式：1:UDP, 2: TCP Client, 3: TCP Server //和AP几乎没有区别
void esp8266_sta_mode_config(u8 mode)
{
		if(mode==1)   //UDP
	{
				sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)port);    //配置目标UDP服务器
				delay_ms(200);
				esp8266_send_cmd("AT+CIPMUX=0","OK",50);  //单链接模式
				delay_ms(200);
				while(esp8266_send_cmd(p,"OK",50));
	}
	else     //TCP
	{
		if(mode==2)     //TCP Client    透传模式测试
		{
			esp8266_send_cmd("AT+CIPMUX=0","OK",50);   //0：单连接，1：多连接
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)port);    //配置目标TCP服务器
			while(esp8266_send_cmd(p,"OK",50))
			{
				printf("连接失败");		//自定义连接失败的处理
				delay_ms(500);
				break;
			}	
			esp8266_send_cmd("AT+CIPMODE=1","OK",50);      //传输模式为：透传			
		}
		else					//TCP Server
		{/////ERROR
				esp8266_send_cmd("AT","OK",20);
				esp8266_send_cmd("AT+CIPMUX=1","0K",50);   //0：单连接，1：多连接
				sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)port);    //开启Server模式(0，关闭；1，打开)，端口号为portnum
				esp8266_send_cmd(p,"OK",50);    
		}
} 	
	}
//设置STA/AP连接方式：1:UDP, 2: TCP Client, 3: TCP Server 
void esp8266_mode_config(u8 mode) 
{
			if(mode==1)   //STA UDP
	{
				sprintf((char*)p,"AT+CIPSTART=0,\"UDP\",\"%s\",%s",ipbuf,(u8*)port);    //配置目标UDP服务器,及ID号，STA模式下为0
				esp8266_send_cmd(p,"OK",50);
				esp8266_ap_mode_config(mode);     //AP模式网络模式配置			
	}
	else     //TCP
	{
		if(mode==2)     //STA TCP Client  
		{
			sprintf((char*)p,"AT+CIPSTART=0,\"TCP\",\"%s\",%s",ipbuf,(u8*)port);    //配置目标TCP服务器,及ID号，STA模式下为0
			while(esp8266_send_cmd(p,"OK",50))
			{
				printf("连接失败");		//自定义连接失败的处理
				delay_ms(500);
				break;
			}	
			esp8266_ap_mode_config(mode);     //AP模式网络模式配置	
		}
		else esp8266_ap_mode_config(mode);      //TCP SERVER不用配置
	}
	}
	

//ESP8266模块和PC进入透传模式
void esp8266_start_trans(void)
{	
	esp8266_send_cmd("AT+CIPMODE=1","OK",50);
	//透传模式下 开始发送数据的指令 这个指令之后就可以直接发数据了//AT+CIPSEND=<link ID>,<length>
	//esp8266_send_cmd("AT+CIPSENDEX=0,32","OK",50);
}

//ESP8266退出透传模式   返回值:0,退出成功;1,退出失败
//配置wifi模块，通过想wifi模块连续发送3个+（每个+号之间 超过10ms,这样认为是连续三次发送+）
u8 esp8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';      
	delay_ms(500);					//等待500ms
	return esp8266_send_cmd("AT","OK",20);//退出透传判断.
}


//向ESP8266发送命令
//cmd:发送的命令字符串;ack:期待的应答结果,如果为空,则表示不需要等待应答;waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果);1,发送失败
u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp8266_check_cmd(ack))
				{
					//printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)
			res=1; 
	}
	return res;
} 


//ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果;其他,期待应答结果的位置(str的位置)
u8* esp8266_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//向ESP8266发送数据
//cmd:发送的命令字符串;waittime:等待时间(单位:10ms)
//返回值:发送数据后，服务器的返回验证码
u8 esp8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	esp8266_send_cmd("AT+CIPSENDEX=0,32","OK",50);
	delay_ms(50);
	u3_printf("%s\r\n\\0",data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp8266_check_cmd(ack))break;//得到有效数据 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s",USART3_RX_BUF);	//发送到串口
		if(mode)USART3_RX_STA=0;
	} 
}
//STA接收远端数据
u8* esp8266_receive_data()
{
	static char wifi_msg[200];
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{
		//printf("!USART3_RX_BUF=%s\r\n",USART3_RX_BUF);//向串口一打印数据，验证我们收到的数据
		memcpy(wifi_msg, USART3_RX_BUF,sizeof(USART3_RX_BUF));
		printf("wifi_msg is:%s\r\n",wifi_msg);	//发送到串口 
		
		USART3_RX_STA=0;
		memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
		delay_ms(300);
	}
	return  wifi_msg;

}
