#ifndef __vMOS_H__
#define __vMOS_H__
//
#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

//videoOTTProvider
#define YouTube 0
#define Youku 1
#define Sohu 2
#define Tencent 3

//videoResolution
#define VRESOLUTION_360P  0
#define VRESOLUTION_480P  1
#define VRESOLUTION_720P  2
#define VRESOLUTION_1080P 3
#define VRESOLUTION_2K    4
#define VRESOLUTION_4K   5

//videoCode
#define VCODEC_H264  0
#define VCODEC_H265  1
#define VCODEC_VP9   2

//videoCodeProfile
#define PROFILE_BASE  0
#define PROFILE_MAIN  1
#define PROFILE_HIGH  2

typedef struct VideoFullInfo{
	double initialBufferTime; /*��ʼ����ʱ�ӣ���λ��(s)*/
	double stallingRatio; /*����ʱ��ռ�ȣ����翨��ʱ��ռ��Ϊ10%��������0.1*/
	double videoPlayDuration; /*��ƵƬԴ������ʱ������λ��(s)*/
	double videoBitrate; /*��Ƶ���ʣ���λ��kbps��*/
	int videoResolution; /*��Ƶ�ֱ��ʣ�����ȡֵ�μ���ֵ*/
	int videoCode; /*��Ƶ�����㷨������ȡֵ�μ���ֵ*/
	int videoCodeProfile; /*��Ƶ����㼶������ȡֵ�μ���ֵ*/
}VideoFullInfo;
	
typedef struct VideoLiteInfo{
	double initialBufferTime; /*��ʼ����ʱ�ӣ���λ��(s)*/
	double stallingRatio; /*����ʱ��ռ�ȣ����翨��ʱ��ռ��Ϊ10%��������0.1*/
	double videoPlayDuration; /*��ƵƬԴ������ʱ������λ��(s)*/
	double videoBitrate; /*��Ƶ���ʣ���λ��kbps��*/
	int videoOTTProvider; /*��Ƶ����㼶������ȡֵ�μ���ֵ*/
}VideoLiteInfo;
	
typedef struct MobileMOS{
	double Score;/*�ƶ���ƵMOS��*/
	double sQuality;/*�ƶ���ƵƬԴ������*/
	double sStalling;/*�ƶ���Ƶ���ٵ÷�*/
	double sLoading;/*�ƶ���Ƶ��ʼ����ʱ�ӵ÷�*/
}MobileMOS;

MobileMOS mCalcVMOSValue_Full(VideoFullInfo videoFullInfoArray[], int arrayLength);
typedef MobileMOS (*mCalcVMOSValue_Full_t)(VideoFullInfo*, int);   //��������ָ������

void print_versions_infomation();
typedef void (*print_versions_infomation_t)();  //��������ָ������

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif
/***__vMOS_H__***/

