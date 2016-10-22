ifneq ($(READIUM_SKIPARM),true)

APP_ABI := armeabi-v7a

ifneq ($(READIUM_SKIPX86),true)
APP_ABI += x86
endif

else

APP_ABI := x86

endif



APP_PLATFORM := android-24

ifeq ($(READIUM_CLANG),true)
NDK_TOOLCHAIN_VERSION := clang
APP_STL := c++_static
else
NDK_TOOLCHAIN_VERSION := 4.9
APP_STL := gnustl_static
endif