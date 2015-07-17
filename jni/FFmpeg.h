/*
 * FFmpeg.h
 *
 *定义了ffmpeg类的成员变量和成员方法
 */

#ifndef FFMPEG_H_
#define FFMPEG_H_

#include <jni.h>
#include <android/bitmap.h>

#include <android/log.h>
#define  LOG_TAG    "jniTest"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
}
class FFmpeg {
public:
	FFmpeg();
	int initial(char * url, JNIEnv *e);
	int h264Decodec(jobject & bitmap);
	void fillPicture(AndroidBitmapInfo* info, void *pixels, AVPicture *rgbPicture);
	virtual ~FFmpeg();
	friend class Video;
	int width;
	int height;
private:
	//结构体解释：http://blog.sina.com.cn/s/blog_73799d980101sxwd.html
	AVFormatContext *pFormatCtx; //此结构体描述了一个媒体文件和媒体流的构成和基本信息，包括音频真视频帧和字幕等
	AVCodecContext *pCodecCtx;//关于解码和编码的上下文数据结构，包含了众多编解码器需要的参数信息
	AVFrame *pFrame;//描述了解码数据
	AVPacket packet;//使用AVPacket来暂存解复用之后、解码之前的媒体数据
	AVPicture picture;//放解码后的数据
	SwsContext * pSwsCtx;
	int videoStream;

	char * rtspURL;
	JNIEnv *env;
};

#endif /* FFMPEG_H_ */
