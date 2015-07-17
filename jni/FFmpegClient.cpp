#include <jni.h>
#include <android/log.h>
#define  LOG_TAG    "jniTest"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C" {

#include "ffmpeg/libavcodec/avcodec.h"
#include "ffmpeg/libavformat/avformat.h"
#include "ffmpeg/libswscale/swscale.h"
#include "FFmpeg.h"

}
const char * rtspURL;
FFmpeg * ffmpeg;
extern "C" {

void Java_com_example_ffmpegclient_VideoDisplay_initialWithUrl(JNIEnv *env,
		jobject thisz, jstring url) {
	//jni提供了jstring来引用java的string类型变量，通过env指针就可以对java端代码进行操作，env是java和c++沟通的一个桥梁
	rtspURL = env->GetStringUTFChars(url, NULL);//返回的 rtspURL是一个地址
	LOGI("%s", rtspURL);
	ffmpeg = new FFmpeg(); //
	ffmpeg->initial((char *) rtspURL, env);//(char *)将rtsp强制类型转换 ,char* 是指向字符的指针类型
	//ffmpeg->initial((char *) rtspURL, env);
	//调用java的方法，设置bitmap的wdith和height
	//jni层调用java函数 ，目的是给java层传递消息 分为三步：
	//参考:http://blog.csdn.net/stefzeus/article/details/6721346
	//http://www.cnblogs.com/likwo/archive/2012/05/21/2512400.html
	//1 获取实例的类定义
	//2 获取方法定义，获取代表方法的jmethodID来代表java端的属性和方法，才能进行java方法调用
	//3 调用方法
	jclass cls = env->GetObjectClass(thisz); //根据对象来获取该对象的类cls
	//调用Java端的方法时，需要取得代表方法的jmethodID才能进行Java方法调用
	jmethodID mid = env->GetMethodID(cls, "setBitmapSize", "(II)V"); // (II)表示有两个参数，V表示是void型,(II)V是数字签名
	env->CallVoidMethod(thisz, mid, (int) ffmpeg->width, (int) ffmpeg->height);//调用java代码初始化bitmap
	//http://my.oschina.net/zhiweiofli/blog/114064
}

void Java_com_example_ffmpegclient_VideoDisplay_play(JNIEnv *env, jobject thisz,
		jobject bitmap) {

	ffmpeg->h264Decodec(bitmap);
	}
}

