LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := mot_boot_mode.c

LOCAL_MODULE := mot_boot_mode
LOCAL_MODULE_TAGS := eng

LOCAL_STATIC_LIBRARIES := \
	libm \
	liblog \
	libstdc++ \
	libcutils \
	libc

LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)
