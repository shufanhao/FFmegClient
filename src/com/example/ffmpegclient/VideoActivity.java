package com.example.ffmpegclient;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
public class VideoActivity extends Activity{
	private VideoDisplay video;
	//static public Handler myHandler;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// 去除title
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_main);
		 //myHandler = new MyHandler();
		video=new VideoDisplay(this);//new了一个类的对象video
		setContentView(video);
		
	}
	public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
        	AlertDialog.Builder builder = new AlertDialog.Builder(this);  
    		builder.setTitle("提示");  
    		builder.setMessage("亲！您确定要退出程序吗？");  
    		builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {

    			@Override
    			public void onClick(DialogInterface dialog, int which) {
    				// TODO Auto-generated method stub
    	            System.exit(0);
    			}    		  
    		});  
    		builder.setNegativeButton("取消", null);  
    		builder.show();
        }
		return true;
    }
	/*@SuppressLint("HandlerLeak")
	class MyHandler extends Handler{
		public void handleMessage(Message msg) { //handler用handleMessage方法去接收消息，然后根据消息的不同执行不同的操作
			// TODO Auto-generated method stub   
			switch (msg.what) {                 //判断what值
			case 0:                   //what等于MSG_REFRESH时
				Log.d("handler", "msg.what");
	
				break;
			default:
				break;
			}
			super.handleMessage(msg);
		}
	}*/
}
