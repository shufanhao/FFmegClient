/*
 * FFmpeg.cpp
 *
 *
 */

#include "FFmpeg.h"

FFmpeg::FFmpeg() {
	pCodecCtx = NULL;
	videoStream = -1;

}

FFmpeg::~FFmpeg() {
	sws_freeContext(pSwsCtx);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
}
//将一个解码器的环境搭建起来，下面使用这个搭建起来的解码器进行解码
int FFmpeg::initial(char * url, JNIEnv * e) {
	int err;
	env = e;
	rtspURL = url;
	AVCodec *pCodec;//编解码器实现的名称
	av_register_all();//1 注册所有容器格式
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();//给AVFormatContext分配内存
	//分配帧空间
	pFrame = avcodec_alloc_frame();//给AVFrame分配内存空间
	err = avformat_open_input(&pFormatCtx, rtspURL, NULL, NULL);//2 打开文件
	if (err < 0) {
		printf("Can not open this file");
		return -1;
	}
	if (av_find_stream_info(pFormatCtx) < 0) { //3 从文件中提取流信息
		printf("Unable to get stream info");
		return -1;
	}
	int i = 0;
	videoStream = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) { //nb_streams 流内的帧数目
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) { //4 穷举所有的流，查找视频帧
			videoStream = i;
			LOGI("nb_streams= %d",pFormatCtx->nb_streams);//这个nb_streams 是2
			LOGI("videostream= %d",videoStream);//videostream= 1
			break;
		}
	}
	if (videoStream == -1) {
		printf("Unable to find video stream");
		return -1;
	}
	//定义编码器类型
	pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	//http://blog.csdn.net/smking/article/details/7561688 这篇文章注释清楚，详细
	width = pCodecCtx->width;//这个width是448和在java文件中的一样的也就是是原始的视频的长度和宽度
	height = pCodecCtx->height;
	//Allocate RGB picture，所以在这里进行picture的alloc时会指定其宽度及高度
	avpicture_alloc(&picture, PIX_FMT_RGB24, pCodecCtx->width,
			pCodecCtx->height);
	//pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	//获取编码器
	pCodec = avcodec_find_decoder(CODEC_ID_H264);//这里只需要对h264格式进行解码，所以直接查找h264
	pSwsCtx = sws_getContext(width, height, PIX_FMT_YUV420P, width, height,
			PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);//将原来的420p转成RGB

	if (pCodec == NULL) {
		printf("Unsupported codec");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) { //判断是否支持h264
		printf("Unable to open codec");
		return -1;
	}
	return 0;
}
//参考http://blog.csdn.net/kaizi318/article/details/7353387
//将某帧数据rgbPicture放到java从c/c++传过来的bitmap中
void FFmpeg::fillPicture(AndroidBitmapInfo* info, void *pixels,
		AVPicture *rgbPicture) {
	uint8_t *frameLine;

	int yy;
	for (yy = 0; yy < info->height; yy++) {
		uint8_t* line = (uint8_t*) pixels;
		frameLine = (uint8_t *) rgbPicture->data[0] + (yy * rgbPicture->linesize[0]);

		int xx;
		for (xx = 0; xx < info->width; xx++) {
			int out_offset = xx * 4;
			int in_offset = xx * 3;

			line[out_offset] = frameLine[in_offset];
			line[out_offset + 1] = frameLine[in_offset + 1];
			line[out_offset + 2] = frameLine[in_offset + 2];
			line[out_offset + 3] = 0xff; //主要是A值
		}
		pixels = (char*) pixels + info->stride;
	}
}

int FFmpeg::h264Decodec(jobject & bitmap) {
	int frameFinished = 0;
	AndroidBitmapInfo info;
	void * pixels;
	int ret = -1;
	if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
		LOGE("AndroidBitmap_getInfo() failed ! error");
		//return -1;
	}
	//读取一帧数据并且放在packet结构体中，并且一直
	while (av_read_frame(pFormatCtx, &packet) >= 0) {//8 不停的从码流中提取出帧数据
		if (packet.stream_index == videoStream) {
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);//9 判断视频帧进行解码播放
			if (frameFinished) {
				LOGI("***************ffmpeg decodec*******************\n");
				//把解码后的数据放到picture，rs=the height of the output slice,也就是说可以从jni层进行尺度变换，将视频变成自己需要的尺寸
				int rs = sws_scale(pSwsCtx,
						(const uint8_t* const *) pFrame->data, pFrame->linesize,
						0, height, picture.data, picture.linesize);
				LOGI("rs= %d",rs);//rs=336

				if (rs == -1) {
					LOGE(
							"__________Can open to change to des imag_____________e\n");
					return -1;
				}
				if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels))
						< 0) {
					LOGE("AndroidBitmap_lockPixels() failed ! error");
					//return -1;
				}
				fillPicture(&info,pixels,&picture);//将picture帧中的数据放到bitmap中进行绘制
				AndroidBitmap_unlockPixels(env, bitmap);
			}
		}
	}
	return 1;
}
