package com.example.ffmpegclient;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;

public class VideoDisplay extends SurfaceView implements Callback {
	private Bitmap bitmap;
	private Matrix matrix;
	private SurfaceHolder sfh;
	private int width = 0;
	private int height = 0;
	private  boolean isRun = true; //这个在出现屏幕不稳定的时候，这个地方出问题了。
	//这只是一个函数的声明，然后在下面函数调用的时候将rtspurl传到jni层去调用
	public native void initialWithUrl(String url);//主要是对ffmpeg的初始化，后去rtsp数据流的一些参数
	//public native void initialWithUrl(String url);
	//获取了图像的尺寸之后在C层代码调用JAVA层代码初始化bitmap对面。因为bitmap初始化需要知道他的尺寸
	public native void play( Bitmap bitmap);//个就是我们ffmpeg里面的一个循环读取数据解码的过程
	//ffmpeg数据处理一个线程，surfaceviewy画图一个线程
	public VideoDisplay(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		sfh = this.getHolder();
		sfh.addCallback(this);
		matrix=new Matrix();
		//初始化了一个Bitmap类
		bitmap = Bitmap.createBitmap(640, 480, Bitmap.Config.ARGB_8888);//创建一个位图bitmap并初始化长度、宽度是640*480
	}
	@Override
	public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) { //arg0 ,1 ,2, 3 这几个参数怎么没用用到呢
		width = arg2;//新的宽度,这个地方获得的是屏幕的分辨率
		height = arg3;//新的高度
	}
	/* surfaceCreated(SurfaceHolder holder)：当Surface第一次创建后会立即调用该函数。程序可以在该函数中做些和绘制界面相关的初始化工作，一般情况下都是在另外的线程来绘制界面，所以不要在这个函数中绘制Surface。 
	 surfaceChanged(SurfaceHolder holder, int format, int width,int height)：当Surface的状态（大小和格式）发生变化的时候会调用该函数，在surfaceCreated调用后该函数至少会被调用一次。 
	 surfaceDestroyed(SurfaceHolder holder)：当Surface被摧毁前会调用该函数，该函数被调用后就不能继续使用Surface了，一般在该函数中来清理使用的资源。*/
	@Override
	//两个线程：（1）初始化ffmpeg和不断解码视频流；（2）获得的bitmap去刷新界面显示
	public void surfaceCreated(SurfaceHolder arg0) {
		Log.i("SUr", "play before");
		new Thread(new Runnable() {

			@Override
			public void run() {
				Log.i("SUr", "play");
				initialWithUrl(MainActivity.RTSPURL);
				play(bitmap);//在FFmpeg里面循环读取解码过程
			}
		}).start();
		new Thread(new Runnable() {

			@Override
			public void run() {
				while (isRun) {
					if ((bitmap != null)) {
						//当服务器中断的时候，bitmap依然不是null
						//为了避免在画图的时候,UI线程也对surface进行操作，在画图前需要对surface加锁，这个
						//工作是有holder做。调用lockCanvas。然后进行绘画画完之后，会解锁unlockCanvasAndPost
						Canvas canvas = sfh.lockCanvas(null);
						//canvas是一块画布，具体的文本和位图如何显示这就是在paint类中定义的
						//用canvas对象画图。画图结束后调用holder.unlockCanvasAndPost()就把图画在窗口中了。
						Paint paint = new Paint();
						paint.setAntiAlias(true);//给Paint加上抗锯齿标志。然后将Paint对象作为参数传给canvas的绘制方法。
						//paint.setStyle(Style.FILL);//fill是填充形状，stroke是绘制形状的轮廓
						paint.setStyle(Style.FILL);
						int mWidth = bitmap.getWidth();
						int mHeight = bitmap.getHeight();
						//通过矩阵来对bitmap进行尺度变换，目的是填充整个屏幕
						//matrix提供了setTranslate(),setScale,setRotate分别进行平移、缩放、旋转
						matrix.reset();//reset()这个方法初始化矩阵
						matrix.setScale((float) width / mWidth, (float) height/ mHeight); //这个width和height是手机的分辨率，而mWidth和mHeight是要播放的视频的分辨率
						System.out.println("width"+ width);//width得到的是960,这个width是手机的分辨率
						System.out.println("mwidth"+ mWidth);//mwidth是448 mWidth应该就是要播放视频的分别率：448*336
						System.out.println("Scale"+ (float)width/mWidth);
						canvas.drawBitmap(bitmap, matrix, paint);
						sfh.unlockCanvasAndPost(canvas);//解锁
					}
					/*else{
						isRun = false;
						VideoActivity.myHandler.sendEmptyMessage(0);
						Log.d("handler", "else");
					}*/
				}
			}
		}).start();
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		//是销毁线程
		isRun = false;
	}

	public void setBitmapSize(int width, int height) {//这个是放在c里面去调用的
		Log.i("Sur", "setsize");
		bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
	}
	static {
		System.loadLibrary("ffmpeg");//一开始没有加载这个
		System.loadLibrary("ffmpegclient");
	}
	
}


