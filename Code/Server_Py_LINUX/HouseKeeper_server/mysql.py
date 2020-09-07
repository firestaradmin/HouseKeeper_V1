
# -*- coding: UTF-8 -*-

import pymysql
import time

host_addr = "127.0.0.1"
user_name = "root"
user_passwd = "3342949"
DB_name = "HouseKeeper"
table_name = "V1"
def mysql_insert(my_json_dict):
	db = pymysql.connect(host_addr, user_name, user_passwd, DB_name )
	# 使用cursor()方法获取操作游标 
	cursor = db.cursor()
	sql = """INSERT INTO %s (opreation, Date, Time, Note)
		 VALUES ('%s', '%s', '%s', '%s')""" % ( table_name, 
												my_json_dict['opreation'],
												time.strftime("%Y-%m-%d", time.localtime()),
												time.strftime("%H:%M:%S", time.localtime()),
												my_json_dict['Note']
												)
	try:
		# 执行sql语句
		cursor.execute(sql)
		# 提交到数据库执行
		db.commit()
		print("insert succeed!")
		db.close()
		return True
	except:
		# 如果发生错误则回滚
		db.rollback()
		print("insert failed!")
		# 关闭数据库连接
		db.close()
		return False
    

if  __name__ == "__main__":
    # 打开数据库连接
	db = pymysql.connect("localhost","root","111111","HouseKeeper" )

	# 使用cursor()方法获取操作游标 
	cursor = db.cursor()
   
    # SQL 查询语句
	sql = "SELECT * FROM test \
		WHERE id < %s" % (1000)
	try:
		# 执行SQL语句
		cursor.execute(sql)
		# 获取所有记录列表
		results = cursor.fetchall()
		for row in results:
			item_id = row[0]
			item_opreation = row[1]
			item_time = row[2]
			# 打印结果
			print ("item_id=%s \titem_opreation=%s\titem_time=%s" % \
				(item_id, item_opreation, item_time))
	except:
		print ("Error: unable to fetch data")

	sql = """INSERT INTO test (opreation, Time)
			VALUES ('test', '%s')""" % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
	try:
		# 执行sql语句
		cursor.execute(sql)
		# 提交到数据库执行
		db.commit()
	except:
		# 如果发生错误则回滚
		db.rollback()

	# 关闭数据库连接
	db.close()
