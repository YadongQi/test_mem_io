LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := gen-file
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := gen_file.c
LOCAL_STATIC_LIBRARIES :=
LOCAL_MODULE_STEM_64 := $(LOCAL_MODULE)64

include $(BUILD_EXECUTABLE)