#ifndef __ESP8266EX_H_
#define __ESP8266EX_H_
#include "sys.h"

#define ESP8266_REC_LEN  			200  	//定义最大接收字节数 200



#define ESP8266_AT_SET_STATION_MODE		(u8 *)"AT+CWMODE=1\r\n"
#define ESP8266_AT_RST					(u8 *)"AT+RST\r\n"
#define ESP8266_AT_CONNECT_WIFI			(u8 *)"AT+CWJAP=\"iu\",\"lizhienya\"\r\n"
#define ESP8266_AT_QUERY_IP				(u8 *)"AT+CIFSR\r\n"
#define ESP8266_AT_CONNECT_TCP			(u8 *)"AT+CIPSTART=\"TCP\",\"120.55.45.151\",8888 \r\n"

#define ESP8266_AT_OPEN_SERIAL_MODE		(u8 *)"AT+CIPMODE=1\r\n"
#define ESP8266_AT_BEGIN_TRANS			(u8 *)"AT+CIPSEND\r\n"
#define ESP8266_AT_OVER_TRANS			(u8 *)"+++"
#define ESP8266_AT_CLOSE_TCP			(u8 *)"AT+CIPCLOSE\r\n"

#define ESP8266_AT_TEST_TCP				(u8 *)"\"{\"opreation\":\"close\",\"Note\":\"帅气庚庚\"}\""			//成功返回[OK]


extern u8 ESP8266_RX_BUF[ESP8266_REC_LEN];	
extern u16 ESP8266_RX_STA ;       //接收状态标记	
extern u16 ESP8266_RX_BUF_P;		//RXBUF 光标
extern u8 ESP8266_RX_timeout_10ms_counter;	//接受完成超时
extern u8 ESP8266_AT_timeout_10ms_counter;
extern u8 ESP8266_AT_wait_flag ;

void ESP8266EX_EN(u8 EN);
void ESP8266EX_RST(void);
void ESP8266EX_Init(void);
void ESP8266_SEND_AT(u8 *str);
void usart3_init(u32 bound);
void ESP8266_wait_AT_RX(void);
void ESP8266_AT_CHOICE(u8 index);



ErrorStatus ESP8266_IF_WIFI_CONNECT(void);
ErrorStatus ESP8266_IF_OK(u8* s, u8 len);
void ESP8266_AT_edit_json_item(char* buf, char *opreation, char *Note);
ErrorStatus ESP8266_AT_AT(void);
ErrorStatus ESP8266_try_send_item(void);
void ESP8266_SHOW_AT(void);
#endif


/*
波特率 9600
1） 设置 wifi 模式：
AT+CWMODE=3 //设置为 softAP+station 共存模式
2） 重启生效
AT+RST
响应：OK
6 6 /  13  Espressif Systems  June 27, 2014
3） 连接路由
AT+CWJAP="ssid","password" // 传入路由的ssid 和 password
响应：OK
4） 查询设备IP
AT+CIFSR
响应：192.168.3.106 //返回设备的 IP 地址
6） 设备连接服务器
AT+CIPSTART="TCP","120.55.45.151",8888 //传入协议、服务器 IP、端口号
7） 开启透传模式
AT+CIPMODE=1
响应：OK
8） 开始透传
AT+CIPSEND
响应： > // 从此时开始，串口输入的字符会透传到服务器端
9)
"{"opreation":"close","Note":"帅气庚庚"}"
响应：[OK]
10） 结束透传
+++
11) 断开TCP
AT+CIPCLOSE
return：
	OK
	Unlink


*/


