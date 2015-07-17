LOCAL_PATH := $(call my-dir)
LOCAL_CPP_EXTENSION := .cpp
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/ffmpeg \
#$(LOCAL_PATH)/libs
		 
LOCAL_LDLIBS := -llog -lz -L $(LOCAL_PATH)/libs -lffmpeg -lm -L$(SYSROOT)/usr/lib -llog -ljnigraphics
LOCAL_MODULE :=ffmpegclient
LOCAL_SRC_FILES := FFmpegClient.cpp FFmpeg.cpp FFmpeg.h

include $(BUILD_SHARED_LIBRARY)