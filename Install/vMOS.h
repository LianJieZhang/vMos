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
	double initialBufferTime; /*初始缓冲时延，单位秒(s)*/
	double stallingRatio; /*卡顿时长占比，例如卡顿时长占比为10%，则输入0.1*/
	double videoPlayDuration; /*视频片源纯播放时长，单位秒(s)*/
	double videoBitrate; /*视频码率，单位（kbps）*/
	int videoResolution; /*视频分辨率，具体取值参见宏值*/
	int videoCode; /*视频编码算法，具体取值参见宏值*/
	int videoCodeProfile; /*视频编码层级，具体取值参见宏值*/
}VideoFullInfo;
	
typedef struct VideoLiteInfo{
	double initialBufferTime; /*初始缓冲时延，单位秒(s)*/
	double stallingRatio; /*卡顿时长占比，例如卡顿时长占比为10%，则输入0.1*/
	double videoPlayDuration; /*视频片源纯播放时长，单位秒(s)*/
	double videoBitrate; /*视频码率，单位（kbps）*/
	int videoOTTProvider; /*视频编码层级，具体取值参见宏值*/
}VideoLiteInfo;
	
typedef struct MobileMOS{
	double Score;/*移动视频MOS分*/
	double sQuality;/*移动视频片源质量分*/
	double sStalling;/*移动视频卡顿得分*/
	double sLoading;/*移动视频初始加载时延得分*/
}MobileMOS;

MobileMOS mCalcVMOSValue_Full(VideoFullInfo videoFullInfoArray[], int arrayLength);
typedef MobileMOS (*mCalcVMOSValue_Full_t)(VideoFullInfo*, int);   //声明函数指针类型

void print_versions_infomation();
typedef void (*print_versions_infomation_t)();  //声明函数指针类型

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif
/***__vMOS_H__***/

