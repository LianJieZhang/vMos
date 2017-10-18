#!/usr/local/python/bin/python
# coding=utf-8
from flask import Flask,render_template,request
import MySQLdb as mysql

con = mysql.connect(user='root',passwd='199211',host='localhost',db='test')
#con = mysql.connect(user='root',passwd='123456',host='192.168.1.195',db='test')
con.autocommit(True)
cur = con.cursor()
app = Flask(__name__)
import json

@app.route('/')
def index():
    return render_template('index.html')

tmp_time = 0
tmp_ip = "";
@app.route('/data')
def data():
	global tmp_time
	ip = request.args.get('ip')
	loop = request.args.get('loop')
	if (int(loop) == 0):
		tmp_time = 0;
	if tmp_time>0:
		sql = 'select * from memory where time>%s and id = "%s"' % (tmp_time/1000, ip)
		#sql = 'select * from memory where id = "%s"' %(ip)
	else:
		sql = 'select * from memory where id = "%s"' %(ip)
	print sql
	cur.execute(sql)
	arr = []
	for i in cur.fetchall():
		print i[1];
		arr.append([i[1]*1000,i[0]])
	if len(arr)>0:
		tmp_time = arr[-1][0]
	return json.dumps(arr)

@app.route('/id')
def getid():
	sql = 'select distinct id from memory'
	cur.execute(sql)
	id_arr = [];
	for i in cur.fetchall():
		id_arr.append(i)
	return json.dumps(id_arr)
if __name__=='__main__':
    app.run(host='0.0.0.0',port=9092,debug=True)
