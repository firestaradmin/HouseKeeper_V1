# -*- coding: UTF-8 -*-

import pymysql
import time
import socket
import json

import mysql

ip_addr = ""
port = 8888

encoding_name = "UTF-8"




"""
json_str_exmaple = 
{
	"opreation":"close",
	"Note":"firestaradmin"
}
"""
def myjson_fun(json_str):
	try:
		dict_data = json.loads(json_str)
		return dict_data
	except:
		return False



if __name__ == "__main__":
	# 创建 socket 对象
	serversocket = socket.socket(
				socket.AF_INET, socket.SOCK_STREAM) 
	# 绑定端口号
	serversocket.bind((ip_addr, port))
	# 设置最大连接数，超过后排队
	serversocket.listen(5)

	while True:
		print("服务器启动，监听客户端链接")
		client, client_addr = serversocket.accept() 
		print("链接的客户端ip:", client_addr)
		while True:
			try:
				data = client.recv(1000) # 读取已链接客户的发送的消息
			except Exception:
				print("断开的客户端", client_addr)
				break

			if not data:
				break
			
			try:
				str_receive = data.decode(encoding = encoding_name)
				print("客户端发送的内容:\r\n",str_receive)
			except:
				print("[error]", encoding_name, "解码失败\r\n")
				client.send("[error]faile to decode\r\n".encode(encoding=encoding_name)) #发送消息给已链接客户端
			else:
				dict_json = myjson_fun(str_receive[1:len(str_receive)-1])

				#成功解析json后
				if dict_json:
					try:
						print("插入数据库的信息为：",dict_json['opreation'],dict_json['Note'])
						if mysql.mysql_insert(dict_json):
							client.send("[OK]\r\n".encode(encoding=encoding_name)) #发送消息给已链接客户端
						else:
							client.send("[error]insert failed\r\n".encode(encoding=encoding_name)) #发送消息给已链接客户端
					except:
						print("[error]数据类型不完整")
				else:
					print("[error]json解析失败")

			
		client.close() #关闭客户端链接
	serversocket.close()


