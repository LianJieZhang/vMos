//g++ -g  test.cpp  -L ./ -lvMOS_V301 -lev -o 123
//g++ -g  test.cpp  -L ./ -lvMOS_V301 -lev  `mysql_config --cflags --libs` -o 123
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ev.h>
#include <sys/shm.h>
#include "vMOS.h"
#include <mysql.h>
#include <sys/time.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SHM_SIZE 4 * 1024
#define SHM_MODE 0666 | IPC_CREAT
#define SampleDuration 60.0
VideoFullInfo info;
VideoFullInfo videoinfo[15];
MobileMOS RealTime;
MobileMOS Period;

char *buf = NULL;
char result[2][128];
char *Index = NULL;
double CurTime = 0.0;
double UseTime = 0.0;
double TempTime = 0.0;
int count = 0;
int i = 0;
int j = 0;
int TimeList = 0;
int shmid;
void *shmaddr = NULL;
key_t key;
MYSQL conn;
int ret = 0;
char sql[1024] = {'\0'};
struct timeval tv;
char ip[32] = {'\0'};

void handleTimeout(struct ev_loop *main_loop,ev_timer *timer_w,int e);
int GetConfigInfo(char *filepath, char *host, char *sqlname, char *dbpwd, char *dbname);
int main(int argc, char *argv[])
{
		int res;
		char host[32] = {'\0'};
		char sqlname[32] = {'\0'};
		char dbpwd[32] = {'\0'};
		char dbname[32] = {'\0'};
		if (GetConfigInfo((char *)"/usr/local/bin/vMos/vMos.conf", host, sqlname, dbpwd, dbname) < 0) {
				printf("open vMos.conf faild!\n");
				return -1;
		}

		mysql_init(&conn);
		if (!(mysql_real_connect(&conn, host, sqlname, dbpwd, dbname, 0, NULL, CLIENT_FOUND_ROWS))) {
				printf("connect mysql faild!\n");
				return -1;
		}

		//get localhost ip
		struct ifaddrs * ifAddrStruct=NULL;
		void * tmpAddrPtr=NULL;
		getifaddrs(&ifAddrStruct);
		while (ifAddrStruct!=NULL) {
				if (ifAddrStruct->ifa_addr->sa_family==AF_INET) { // check it is IP4
						// is a valid IP4 Address
						tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
						char addressBuffer[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
						if (ifAddrStruct->ifa_name[0] != 'l' && ifAddrStruct->ifa_name[1] != '0') {
								memcpy(ip,addressBuffer,strlen(addressBuffer));
								//printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer);
								break;
						}
				}
				ifAddrStruct=ifAddrStruct->ifa_next;
		}
		printf("get ip:%s\n",ip);


		printf("connection mysql success!\n");
		
		gettimeofday(&tv,NULL);
		TimeList = tv.tv_sec;
		char firstsql[256] = {'\0'};
		sprintf(firstsql,"insert into memory values(%f,%d,\"%s\")",0.0,TimeList,ip);
		ret=mysql_query(&conn,firstsql);
		
		ev_timer timer_w;
		struct ev_loop *main_loop;
		main_loop = ev_default_loop(0);
		if ((key = ftok("/dev/null",66)) < -1) {
				printf(" ftok faild\n");
				return -1;
		}	

		if ((shmid = shmget(key, SHM_SIZE,SHM_MODE)) < 0) {
				printf("shmget() faild\n");
				return -1;
		}
		if ((shmaddr = shmat(shmid, NULL, 0)) == (void *)-1) {
				printf("shmaddr faild\n");
				return -1;
		}
		buf = (char *)shmaddr;
		gettimeofday(&tv,NULL);
		TimeList = tv.tv_sec + 60;

		ev_timer_init(&timer_w,handleTimeout,0,1);
		//ev_timer_set(&timer_w,0,3);
		ev_timer_start(main_loop,&timer_w);
		ev_run(main_loop,0);
		return 0;
}

void handleTimeout(struct ev_loop *main_loop,ev_timer *timer_w,int e)
{
		Index = strstr(buf,",");
		if (Index == NULL) {
				printf("read share memory is null,please wait try agin\n");
				return ;
		}
		memcpy(result[0], buf, Index - buf);
		result[0][Index - buf] = '\0';
		memcpy(result[1], Index + 1, buf + strlen(buf) - Index - 1);
		result[1][buf + strlen(buf) - Index -1] = '\0';
		CurTime = atof(result[0]);	
		UseTime = atof(result[1]);
		//	printf("zlj--:%f,%f\n",CurTime,UseTime);	
		if (CurTime - TempTime > 1.5 || CurTime - TempTime < 0.5) {
				count++;
		}
		TempTime = CurTime;
		i++;
		if (i >=60) {
				i = 0;
				info.initialBufferTime = UseTime;
				info.stallingRatio = (double)count / 60.0;
				info.videoPlayDuration = 60.0;
				info.videoBitrate = 9000;
				info.videoResolution = VRESOLUTION_720P;
				info.videoCode = VCODEC_H264;
				info.videoCodeProfile = PROFILE_MAIN;
				if (j >= 15) {
						j = 0;
				}
				videoinfo[j] = info;
				j++;
				RealTime = mCalcVMOSValue_Full(videoinfo,j);
				Period = mCalcVMOSValue_Full(&info,1);
				printf("%f,%d,%d,%f\n",RealTime.Score,TimeList,count,Period.Score);
				sprintf(sql,"insert into memory values(%f,%d,\"%s\")",Period.Score,TimeList,ip);
				printf("sql:%s\n",sql);
				ret=mysql_query(&conn,sql);
				if (ret) 
						printf("insert error\n");
				else
						printf("insert success!\n");

				TimeList += 60;
				count = 0;
		}
}
int GetConfigInfo(char *filepath, char *host, char *sqlname, char *dbpwd, char *dbname) {
		FILE *fp = NULL;
		fp = fopen(filepath, "r");
		if (NULL == fp) {
				printf("open filepath!\n");
				return -1;
		}
		fgets(host,1000,fp);
		fgets(sqlname,1000,fp);
		fgets(dbpwd,1000,fp);
		fgets(dbname,1000,fp);

		host[strlen(host) - 1] = '\0';
		sqlname[strlen(sqlname) - 1] = '\0';
		dbpwd[strlen(dbpwd) - 1] = '\0';
		dbname[strlen(dbname) - 1] = '\0';
		printf("host:%s,sqlname:%s,dbpwd:%s,dbname:%s\n",host,sqlname,dbpwd,dbname);
		return 0;
}
