# Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without modification, 
#   are permitted provided that the following conditions are met:
#   1. Redistributions of source code must retain the above copyright notice, this 
#   list of conditions and the following disclaimer.
#   2. Redistributions in binary form must reproduce the above copyright notice, 
#   this list of conditions and the following disclaimer in the documentation and/or 
#   other materials provided with the distribution.
#   3. Neither the name of the organization nor the names of its contributors may be 
#   used to endorse or promote products derived from this software without specific 
#   prior written permission.
#   
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
#   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
#   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
#   IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
#   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
#   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
#   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
#   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
#   OF THE POSSIBILITY OF SUCH DAMAGE.

LOCAL_PATH := $(call my-dir)
EPUB3_PATH := $(LOCAL_PATH)/../../../ePub3
THIRD_PARTY_PATH := $(EPUB3_PATH)/ThirdParty

###########################################################
# Prebuilt libraries for ICU

#ICU_LIB_PATH := $(THIRD_PARTY_PATH)/icu4c/lib/$(TARGET_ARCH_ABI)
#ICU_INCLUDE_PATH := $(THIRD_PARTY_PATH)/icu4c/include

# Unicode

#include $(CLEAR_VARS)
#LOCAL_MODULE := icuuc
#LOCAL_SRC_FILES := $(ICU_LIB_PATH)/libicuuc.a
#LOCAL_EXPORT_C_INCLUDES := $(ICU_INCLUDE_PATH)
#include $(PREBUILT_STATIC_LIBRARY)

# Internationalization

#include $(CLEAR_VARS)
#LOCAL_MODULE := icui18n
#LOCAL_SRC_FILES := $(ICU_LIB_PATH)/libicui18n.a
#LOCAL_EXPORT_C_INCLUDES := $(ICU_INCLUDE_PATH)
#include $(PREBUILT_STATIC_LIBRARY)

# Input/Output

#include $(CLEAR_VARS)
#LOCAL_MODULE := icuio
#LOCAL_SRC_FILES := $(ICU_LIB_PATH)/libicuio.a
#LOCAL_EXPORT_C_INCLUDES := $(ICU_INCLUDE_PATH)
#include $(PREBUILT_STATIC_LIBRARY)

# Data

#include $(CLEAR_VARS)
#LOCAL_MODULE := icudata
#LOCAL_SRC_FILES := $(ICU_LIB_PATH)/libicudata.a
#LOCAL_EXPORT_C_INCLUDES := $(ICU_INCLUDE_PATH)
#include $(PREBUILT_STATIC_LIBRARY)

###########################################################
# libxml2

include $(CLEAR_VARS)
LOCAL_MODULE := xml2

ifeq ($(READIUM_CLANG),true)
LOCAL_CPPFLAGS := -std=c++11 -fpermissive -DBUILDING_EPUB3 -D_LIBCPP_INLINE_VISIBILITY_EXCEPT_GCC49=_LIBCPP_INLINE_VISIBILITY
LOCAL_CXXFLAGS := -std=c++11 -fpermissive -DBUILDING_EPUB3 -D_LIBCPP_INLINE_VISIBILITY_EXCEPT_GCC49=_LIBCPP_INLINE_VISIBILITY
LOCAL_CFLAGS := -std=c11 -DBUILDING_EPUB3 -D_LIBCPP_INLINE_VISIBILITY_EXCEPT_GCC49=_LIBCPP_INLINE_VISIBILITY
else
LOCAL_CPPFLAGS := -std=gnu++11 -fpermissive -DBUILDING_EPUB3
LOCAL_CXXFLAGS := -std=gnu++11 -fpermissive -DBUILDING_EPUB3
LOCAL_CFLAGS := -std=gnu11 -DBUILDING_EPUB3
endif

LOCAL_CPP_FEATURES += exceptions rtti

ifeq ($(TARGET_ARCH_ABI),x86)
    LOCAL_CFLAGS += -mtune=atom -mssse3 -mfpmath=sse
endif

LOCAL_SRC_FILES := \
        $(THIRD_PARTY_PATH)/libxml2-android/SAX.c \
        $(THIRD_PARTY_PATH)/libxml2-android/entities.c \
        $(THIRD_PARTY_PATH)/libxml2-android/encoding.c \
        $(THIRD_PARTY_PATH)/libxml2-android/error.c \
        $(THIRD_PARTY_PATH)/libxml2-android/parserInternals.c \
        $(THIRD_PARTY_PATH)/libxml2-android/parser.c \
        $(THIRD_PARTY_PATH)/libxml2-android/tree.c \
        $(THIRD_PARTY_PATH)/libxml2-android/hash.c \
        $(THIRD_PARTY_PATH)/libxml2-android/list.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlIO.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlmemory.c \
        $(THIRD_PARTY_PATH)/libxml2-android/uri.c \
        $(THIRD_PARTY_PATH)/libxml2-android/valid.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xlink.c \
        $(THIRD_PARTY_PATH)/libxml2-android/HTMLparser.c \
        $(THIRD_PARTY_PATH)/libxml2-android/HTMLtree.c \
        $(THIRD_PARTY_PATH)/libxml2-android/debugXML.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xpath.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xpointer.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xinclude.c \
        $(THIRD_PARTY_PATH)/libxml2-android/nanohttp.c \
        $(THIRD_PARTY_PATH)/libxml2-android/nanoftp.c \
        $(THIRD_PARTY_PATH)/libxml2-android/DOCBparser.c \
        $(THIRD_PARTY_PATH)/libxml2-android/catalog.c \
        $(THIRD_PARTY_PATH)/libxml2-android/globals.c \
        $(THIRD_PARTY_PATH)/libxml2-android/threads.c \
        $(THIRD_PARTY_PATH)/libxml2-android/c14n.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlstring.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlregexp.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlschemas.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlschemastypes.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlunicode.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlreader.c \
        $(THIRD_PARTY_PATH)/libxml2-android/relaxng.c \
        $(THIRD_PARTY_PATH)/libxml2-android/dict.c \
        $(THIRD_PARTY_PATH)/libxml2-android/SAX2.c \
        $(THIRD_PARTY_PATH)/libxml2-android/legacy.c \
        $(THIRD_PARTY_PATH)/libxml2-android/chvalid.c \
        $(THIRD_PARTY_PATH)/libxml2-android/pattern.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlsave.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlmodule.c \
        $(THIRD_PARTY_PATH)/libxml2-android/xmlwriter.c \
        $(THIRD_PARTY_PATH)/libxml2-android/schematron.c
LOCAL_C_INCLUDES += \
        $(LOCAL_PATH)/include
        # $(READIUM_NDK_INCLUDE_DIR)

# ifeq ($(TARGET_ARCH_ABI),x86)
# LOCAL_C_INCLUDES += \
#         $(READIUM_NDK_INCLUDE_DIR)/x86_64-linux-android
# endif

# ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
# LOCAL_C_INCLUDES += \
#         $(READIUM_NDK_INCLUDE_DIR)/arm-linux-android
# endif

# ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
# LOCAL_C_INCLUDES += \
#         $(READIUM_NDK_INCLUDE_DIR)/aarch64-linux-android
# endif

include $(BUILD_STATIC_LIBRARY)

###########################################################
# epub3

include $(CLEAR_VARS)
LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true
LOCAL_MODULE := epub3

ifeq ($(READIUM_CLANG),true)
LOCAL_CPPFLAGS := -std=c++11 -fpermissive -DBUILDING_EPUB3 -D_LIBCPP_INLINE_VISIBILITY_EXCEPT_GCC49=_LIBCPP_INLINE_VISIBILITY
LOCAL_CXXFLAGS := -std=c++11 -fpermissive -DBUILDING_EPUB3 -D_LIBCPP_INLINE_VISIBILITY_EXCEPT_GCC49=_LIBCPP_INLINE_VISIBILITY
LOCAL_CFLAGS := -std=c11 -DBUILDING_EPUB3 -D_LIBCPP_INLINE_VISIBILITY_EXCEPT_GCC49=_LIBCPP_INLINE_VISIBILITY
else
LOCAL_CPPFLAGS := -std=gnu++11 -fpermissive -DBUILDING_EPUB3
LOCAL_CXXFLAGS := -std=gnu++11 -fpermissive -DBUILDING_EPUB3
LOCAL_CFLAGS := -std=gnu11 -DBUILDING_EPUB3
endif

LOCAL_CPP_FEATURES += exceptions rtti

ifneq ($(TARGET_ARCH_ABI),x86)
    LOCAL_CPPFLAGS += -J6
    LOCAL_CXXFLAGS += -J6
    LOCAL_CFLAGS += -J6
endif

ifeq ($(TARGET_ARCH_ABI),x86)
    LOCAL_CFLAGS += -mtune=atom -mssse3 -mfpmath=sse
endif

LOCAL_STATIC_LIBRARIES := xml2
LOCAL_LDLIBS := -lz -landroid -llog
LOCAL_C_INCLUDES += \
        $(LOCAL_PATH)/include \
        $(LOCAL_PATH)/include/ePub3 \
        $(LOCAL_PATH)/include/ePub3/utilities
        # $(READIUM_NDK_INCLUDE_DIR)
        
# ifeq ($(TARGET_ARCH_ABI),x86)
# LOCAL_C_INCLUDES += \
#         $(READIUM_NDK_INCLUDE_DIR)/x86_64-linux-android
# endif

# ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
# LOCAL_C_INCLUDES += \
#         $(READIUM_NDK_INCLUDE_DIR)/arm-linux-android
# endif

# ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
# LOCAL_C_INCLUDES += \
#         $(READIUM_NDK_INCLUDE_DIR)/aarch64-linux-android
# endif

LOCAL_SRC_FILES := \
    $(THIRD_PARTY_PATH)/sha1/sha1.cpp \
    $(THIRD_PARTY_PATH)/libzip/mkstemp.c \
    $(THIRD_PARTY_PATH)/libzip/zip_add.c \
    $(THIRD_PARTY_PATH)/libzip/zip_add_dir.c \
    $(THIRD_PARTY_PATH)/libzip/zip_close.c \
    $(THIRD_PARTY_PATH)/libzip/zip_delete.c \
    $(THIRD_PARTY_PATH)/libzip/zip_dirent.c \
    $(THIRD_PARTY_PATH)/libzip/zip_entry_free.c \
    $(THIRD_PARTY_PATH)/libzip/zip_entry_new.c \
    $(THIRD_PARTY_PATH)/libzip/zip_err_str.c \
    $(THIRD_PARTY_PATH)/libzip/zip_error.c \
    $(THIRD_PARTY_PATH)/libzip/zip_error_clear.c \
    $(THIRD_PARTY_PATH)/libzip/zip_error_get.c \
    $(THIRD_PARTY_PATH)/libzip/zip_error_get_sys_type.c \
    $(THIRD_PARTY_PATH)/libzip/zip_error_strerror.c \
    $(THIRD_PARTY_PATH)/libzip/zip_error_to_str.c \
    $(THIRD_PARTY_PATH)/libzip/zip_fclose.c \
    $(THIRD_PARTY_PATH)/libzip/zip_file_error_clear.c \
    $(THIRD_PARTY_PATH)/libzip/zip_file_error_get.c \
    $(THIRD_PARTY_PATH)/libzip/zip_file_get_offset.c \
    $(THIRD_PARTY_PATH)/libzip/zip_file_strerror.c \
    $(THIRD_PARTY_PATH)/libzip/zip_filerange_crc.c \
    $(THIRD_PARTY_PATH)/libzip/zip_fopen.c \
    $(THIRD_PARTY_PATH)/libzip/zip_fopen_index.c \
    $(THIRD_PARTY_PATH)/libzip/zip_fread.c \
    $(THIRD_PARTY_PATH)/libzip/zip_free.c \
    $(THIRD_PARTY_PATH)/libzip/zip_fseek.c \
    $(THIRD_PARTY_PATH)/libzip/zip_ftell.c \
    $(THIRD_PARTY_PATH)/libzip/zip_get_archive_comment.c \
    $(THIRD_PARTY_PATH)/libzip/zip_get_archive_flag.c \
    $(THIRD_PARTY_PATH)/libzip/zip_get_file_comment.c \
    $(THIRD_PARTY_PATH)/libzip/zip_get_name.c \
    $(THIRD_PARTY_PATH)/libzip/zip_get_num_files.c \
    $(THIRD_PARTY_PATH)/libzip/zip_memdup.c \
    $(THIRD_PARTY_PATH)/libzip/zip_name_locate.c \
    $(THIRD_PARTY_PATH)/libzip/zip_new.c \
    $(THIRD_PARTY_PATH)/libzip/zip_open.c \
    $(THIRD_PARTY_PATH)/libzip/zip_rename.c \
    $(THIRD_PARTY_PATH)/libzip/zip_replace.c \
    $(THIRD_PARTY_PATH)/libzip/zip_set_archive_comment.c \
    $(THIRD_PARTY_PATH)/libzip/zip_set_archive_flag.c \
    $(THIRD_PARTY_PATH)/libzip/zip_set_file_comment.c \
    $(THIRD_PARTY_PATH)/libzip/zip_set_name.c \
    $(THIRD_PARTY_PATH)/libzip/zip_source_buffer.c \
    $(THIRD_PARTY_PATH)/libzip/zip_source_file.c \
    $(THIRD_PARTY_PATH)/libzip/zip_source_filep.c \
    $(THIRD_PARTY_PATH)/libzip/zip_source_free.c \
    $(THIRD_PARTY_PATH)/libzip/zip_source_function.c \
    $(THIRD_PARTY_PATH)/libzip/zip_source_zip.c \
    $(THIRD_PARTY_PATH)/libzip/zip_stat.c \
    $(THIRD_PARTY_PATH)/libzip/zip_stat_index.c \
    $(THIRD_PARTY_PATH)/libzip/zip_stat_init.c \
    $(THIRD_PARTY_PATH)/libzip/zip_strerror.c \
    $(THIRD_PARTY_PATH)/libzip/zip_unchange.c \
    $(THIRD_PARTY_PATH)/libzip/zip_unchange_all.c \
    $(THIRD_PARTY_PATH)/libzip/zip_unchange_archive.c \
    $(THIRD_PARTY_PATH)/libzip/zip_unchange_data.c \
    $(THIRD_PARTY_PATH)/google-url/base/string16.cc \
    $(THIRD_PARTY_PATH)/google-url/src/gurl.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_etc.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_filesystemurl.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_fileurl.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_host.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_internal.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_ip.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_mailtourl.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_path.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_pathurl.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_query.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_relative.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_stdurl.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_parse.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_parse_file.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_util.cc \
    $(THIRD_PARTY_PATH)/google-url/src/url_canon_cpp11.cc \
    $(EPUB3_PATH)/xml/utilities/base.cpp \
    $(EPUB3_PATH)/xml/utilities/io.cpp \
    $(EPUB3_PATH)/xml/validation/schema.cpp \
    $(EPUB3_PATH)/xml/validation/ns.cpp \
    $(EPUB3_PATH)/xml/validation/c14n.cpp \
    $(EPUB3_PATH)/xml/tree/document.cpp \
    $(EPUB3_PATH)/xml/tree/element.cpp \
    $(EPUB3_PATH)/xml/tree/node.cpp \
    $(EPUB3_PATH)/xml/tree/xpath.cpp \
    $(EPUB3_PATH)/utilities/byte_buffer.cpp \
    $(EPUB3_PATH)/utilities/byte_stream.cpp \
    $(EPUB3_PATH)/utilities/epub_locale.cpp \
    $(EPUB3_PATH)/utilities/error_handler.cpp \
    $(EPUB3_PATH)/utilities/executor.cpp \
    $(EPUB3_PATH)/utilities/future.cpp \
    $(EPUB3_PATH)/utilities/iri.cpp \
    $(EPUB3_PATH)/utilities/optional.cpp \
    $(EPUB3_PATH)/utilities/path_help.cpp \
    $(EPUB3_PATH)/utilities/ring_buffer.cpp \
    $(EPUB3_PATH)/utilities/run_loop_android.cpp \
    $(EPUB3_PATH)/utilities/utfstring.cpp \
    $(wildcard $(EPUB3_PATH)/ePub/*.cpp) \
    $(wildcard $(LOCAL_PATH)/src/main/jni/*.cpp) \
    $(wildcard $(LOCAL_PATH)/src/main/jni/jni/*.cpp)
#    $(wildcard $(LOCAL_PATH)/src/main/jni/android/*.cpp)

include $(BUILD_SHARED_LIBRARY)
