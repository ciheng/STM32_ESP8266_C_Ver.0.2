#ifndef __ESP8266_H
#define __ESP8266_H
#include "sys.h"
#include "stdio.h"	

//用户配置AT指令
extern char restart[];
extern char cwmode[];
extern char cwlap[];
extern char cwjap[];
extern char cifsr[];
extern char cipmux[];
extern char cipstart[];
extern char cipsend[];
extern char cipserver[];
extern char cwlif[];
extern char cipstatus[];
extern char cipsto[];
extern char cipmode[];
extern char test[];


//函数

void atk_8266_init(void);
u8 esp8266_set_mode(u8 mode);
void esp8266_ap_mode_config(u8 mode);
void esp8266_sta_mode_config(u8 mode);
void esp8266_mode_config(u8 mode);
void set_wifi_sta(u8* ssid, u8* encryp, u8* pswd);
void atk_8266_ip_set(u8 mode,char* ip);
void set_smartConfig(u8 smartType);


u8 esp8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 esp8266_send_data(u8 *data,u8 *ack,u16 waittime);
u8* esp8266_check_cmd(u8 *str);
void esp8266_start_trans(void);
u8 esp8266_quit_trans(void);	
u8 atk_8266_consta_check(void);
void atk_8266_at_response(u8 mode);
u8* esp8266_receive_data();

#endif
