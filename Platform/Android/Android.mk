LOCAL_PATH := $(call my-dir)/../..
THIRD_PARTY := $(LOCAL_PATH)/ePub3/ThirdParty

###########################################################
# Prebuilt libraries for ICU

ICU_LIB_PATH := /ePub3/ThirdParty/icu4c/lib
ICU_INCLUDE_PATH := $(THIRD_PARTY)/icu4c/include

# Unicode

include $(CLEAR_VARS)
LOCAL_MODULE := icuuc
LOCAL_SRC_FILES := $(ICU_LIB_PATH)/libicuuc.a
LOCAL_EXPORT_C_INCLUDES := $(ICU_INCLUDE_PATH)
include $(PREBUILT_STATIC_LIBRARY)

# Internationalization

include $(CLEAR_VARS)
LOCAL_MODULE := icui18n
LOCAL_SRC_FILES := $(ICU_LIB_PATH)/libicui18n.a
LOCAL_EXPORT_C_INCLUDES := $(ICU_INCLUDE_PATH)
include $(PREBUILT_STATIC_LIBRARY)

# Input/Output

include $(CLEAR_VARS)
LOCAL_MODULE := icuio
LOCAL_SRC_FILES := $(ICU_LIB_PATH)/libicuio.a
LOCAL_EXPORT_C_INCLUDES := $(ICU_INCLUDE_PATH)
include $(PREBUILT_STATIC_LIBRARY)

# Data

include $(CLEAR_VARS)
LOCAL_MODULE := icudata
LOCAL_SRC_FILES := $(ICU_LIB_PATH)/libicudata.a
LOCAL_EXPORT_C_INCLUDES := $(ICU_INCLUDE_PATH)
include $(PREBUILT_STATIC_LIBRARY)

###########################################################
# libxml2

include $(CLEAR_VARS)
LOCAL_MODULE := xml2
LOCAL_SRC_FILES := \
		$(THIRD_PARTY)/libxml2-android/SAX.c \
		$(THIRD_PARTY)/libxml2-android/entities.c \
		$(THIRD_PARTY)/libxml2-android/encoding.c \
		$(THIRD_PARTY)/libxml2-android/error.c \
		$(THIRD_PARTY)/libxml2-android/parserInternals.c \
		$(THIRD_PARTY)/libxml2-android/parser.c \
		$(THIRD_PARTY)/libxml2-android/tree.c \
		$(THIRD_PARTY)/libxml2-android/hash.c \
		$(THIRD_PARTY)/libxml2-android/list.c \
		$(THIRD_PARTY)/libxml2-android/xmlIO.c \
		$(THIRD_PARTY)/libxml2-android/xmlmemory.c \
		$(THIRD_PARTY)/libxml2-android/uri.c \
		$(THIRD_PARTY)/libxml2-android/valid.c \
		$(THIRD_PARTY)/libxml2-android/xlink.c \
		$(THIRD_PARTY)/libxml2-android/HTMLparser.c \
		$(THIRD_PARTY)/libxml2-android/HTMLtree.c \
		$(THIRD_PARTY)/libxml2-android/debugXML.c \
		$(THIRD_PARTY)/libxml2-android/xpath.c \
		$(THIRD_PARTY)/libxml2-android/xpointer.c \
		$(THIRD_PARTY)/libxml2-android/xinclude.c \
		$(THIRD_PARTY)/libxml2-android/nanohttp.c \
		$(THIRD_PARTY)/libxml2-android/nanoftp.c \
		$(THIRD_PARTY)/libxml2-android/DOCBparser.c \
		$(THIRD_PARTY)/libxml2-android/catalog.c \
		$(THIRD_PARTY)/libxml2-android/globals.c \
		$(THIRD_PARTY)/libxml2-android/threads.c \
		$(THIRD_PARTY)/libxml2-android/c14n.c \
		$(THIRD_PARTY)/libxml2-android/xmlstring.c \
		$(THIRD_PARTY)/libxml2-android/xmlregexp.c \
		$(THIRD_PARTY)/libxml2-android/xmlschemas.c \
		$(THIRD_PARTY)/libxml2-android/xmlschemastypes.c \
		$(THIRD_PARTY)/libxml2-android/xmlunicode.c \
		$(THIRD_PARTY)/libxml2-android/xmlreader.c \
		$(THIRD_PARTY)/libxml2-android/relaxng.c \
		$(THIRD_PARTY)/libxml2-android/dict.c \
		$(THIRD_PARTY)/libxml2-android/SAX2.c \
		$(THIRD_PARTY)/libxml2-android/legacy.c \
		$(THIRD_PARTY)/libxml2-android/chvalid.c \
		$(THIRD_PARTY)/libxml2-android/pattern.c \
		$(THIRD_PARTY)/libxml2-android/xmlsave.c \
		$(THIRD_PARTY)/libxml2-android/xmlmodule.c \
		$(THIRD_PARTY)/libxml2-android/xmlwriter.c \
		$(THIRD_PARTY)/libxml2-android/schematron.c
LOCAL_EXPORT_C_INCLUDES := $(THIRD_PARTY)/libxml2-android/include

include $(BUILD_STATIC_LIBRARY)

###########################################################

include $(CLEAR_VARS)

subdirs := $(addprefix $(LOCAL_PATH)/, )

LOCAL_MODULE := epub3
LOCAL_CPPFLAGS := -std=gnu++11
LOCAL_CFLAGS := -std=gnu11
LOCAL_CXXFLAGS := -std=gnu++11
LOCAL_CPP_FEATURES += exceptions rtti
LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/ePub3 \
		$(LOCAL_PATH)/ePub3/utilities \
		$(LOCAL_PATH)/ePub3/ThirdParty
LOCAL_C_INCLUDES += ${shell find $(LOCAL_PATH)/ePub3/xml/ -type d}
LOCAL_C_INCLUDES += ${shell find $(LOCAL_PATH)/ePub3/ePub -type d}
LOCAL_STATIC_LIBRARIES := icuuc icui18n icuio icudata xml2
LOCAL_LDLIBS := -lz
LOCAL_SRC_FILES := \
		ePub3/ThirdParty/libzip/mkstemp.c \
		ePub3/ThirdParty/libzip/zip_add.c \
		ePub3/ThirdParty/libzip/zip_add_dir.c \
		ePub3/ThirdParty/libzip/zip_close.c \
		ePub3/ThirdParty/libzip/zip_delete.c \
		ePub3/ThirdParty/libzip/zip_dirent.c \
		ePub3/ThirdParty/libzip/zip_entry_free.c \
		ePub3/ThirdParty/libzip/zip_entry_new.c \
		ePub3/ThirdParty/libzip/zip_err_str.c \
		ePub3/ThirdParty/libzip/zip_error.c \
		ePub3/ThirdParty/libzip/zip_error_clear.c \
		ePub3/ThirdParty/libzip/zip_error_get.c \
		ePub3/ThirdParty/libzip/zip_error_get_sys_type.c \
		ePub3/ThirdParty/libzip/zip_error_strerror.c \
		ePub3/ThirdParty/libzip/zip_error_to_str.c \
		ePub3/ThirdParty/libzip/zip_fclose.c \
		ePub3/ThirdParty/libzip/zip_file_error_clear.c \
		ePub3/ThirdParty/libzip/zip_file_error_get.c \
		ePub3/ThirdParty/libzip/zip_file_get_offset.c \
		ePub3/ThirdParty/libzip/zip_file_strerror.c \
		ePub3/ThirdParty/libzip/zip_filerange_crc.c \
		ePub3/ThirdParty/libzip/zip_fopen.c \
		ePub3/ThirdParty/libzip/zip_fopen_index.c \
		ePub3/ThirdParty/libzip/zip_fread.c \
		ePub3/ThirdParty/libzip/zip_free.c \
		ePub3/ThirdParty/libzip/zip_get_archive_comment.c \
		ePub3/ThirdParty/libzip/zip_get_archive_flag.c \
		ePub3/ThirdParty/libzip/zip_get_file_comment.c \
		ePub3/ThirdParty/libzip/zip_get_name.c \
		ePub3/ThirdParty/libzip/zip_get_num_files.c \
		ePub3/ThirdParty/libzip/zip_memdup.c \
		ePub3/ThirdParty/libzip/zip_name_locate.c \
		ePub3/ThirdParty/libzip/zip_new.c \
		ePub3/ThirdParty/libzip/zip_open.c \
		ePub3/ThirdParty/libzip/zip_rename.c \
		ePub3/ThirdParty/libzip/zip_replace.c \
		ePub3/ThirdParty/libzip/zip_set_archive_comment.c \
		ePub3/ThirdParty/libzip/zip_set_archive_flag.c \
		ePub3/ThirdParty/libzip/zip_set_file_comment.c \
		ePub3/ThirdParty/libzip/zip_set_name.c \
		ePub3/ThirdParty/libzip/zip_source_buffer.c \
		ePub3/ThirdParty/libzip/zip_source_file.c \
		ePub3/ThirdParty/libzip/zip_source_filep.c \
		ePub3/ThirdParty/libzip/zip_source_free.c \
		ePub3/ThirdParty/libzip/zip_source_function.c \
		ePub3/ThirdParty/libzip/zip_source_zip.c \
		ePub3/ThirdParty/libzip/zip_stat.c \
		ePub3/ThirdParty/libzip/zip_stat_index.c \
		ePub3/ThirdParty/libzip/zip_stat_init.c \
		ePub3/ThirdParty/libzip/zip_strerror.c \
		ePub3/ThirdParty/libzip/zip_unchange.c \
		ePub3/ThirdParty/libzip/zip_unchange_all.c \
		ePub3/ThirdParty/libzip/zip_unchange_archive.c \
		ePub3/ThirdParty/libzip/zip_unchange_data.c \
		ePub3/xml/utilities/io.cpp \
		ePub3/xml/validation/schema.cpp \
		ePub3/xml/tree/node.cpp \
		ePub3/xml/tree/xpath.cpp \
		ePub3/xml/validation/ns.cpp \
		ePub3/xml/utilities/base.cpp \
		ePub3/xml/tree/document.cpp \
		ePub3/xml/validation/c14n.cpp \
		ePub3/xml/tree/element.cpp \
		ePub3/ePub/zip_archive.cpp \
		ePub3/ePub/archive.cpp \
		ePub3/ePub/container.cpp \
		ePub3/ePub/package.cpp \
		ePub3/ePub/archive_xml.cpp \
		ePub3/ePub/xpath_wrangler.cpp \
		ePub3/ePub/spine.cpp \
		ePub3/ePub/manifest.cpp \
		ePub3/ePub/metadata.cpp \
		ePub3/ePub/cfi.cpp \
		ePub3/ePub/nav_point.cpp \
		ePub3/ePub/nav_table.cpp \
		ePub3/ePub/glossary.cpp \
		ePub3/ePub/library.cpp \
		ePub3/ePub/font_obfuscation.cpp \
		ePub3/ePub/encryption.cpp \
		ePub3/ePub/signatures.cpp \
		ePub3/utilities/iri.cpp \
		ePub3/utilities/utfstring.cpp \
		ePub3/ThirdParty/google-url/base/string16.cc \
		ePub3/ThirdParty/google-url/src/gurl.cc \
		ePub3/ThirdParty/google-url/src/url_canon_etc.cc \
		ePub3/ThirdParty/google-url/src/url_canon_filesystemurl.cc \
		ePub3/ThirdParty/google-url/src/url_canon_fileurl.cc \
		ePub3/ThirdParty/google-url/src/url_canon_host.cc \
		ePub3/ThirdParty/google-url/src/url_canon_internal.cc \
		ePub3/ThirdParty/google-url/src/url_canon_ip.cc \
		ePub3/ThirdParty/google-url/src/url_canon_mailtourl.cc \
		ePub3/ThirdParty/google-url/src/url_canon_path.cc \
		ePub3/ThirdParty/google-url/src/url_canon_pathurl.cc \
		ePub3/ThirdParty/google-url/src/url_canon_query.cc \
		ePub3/ThirdParty/google-url/src/url_canon_relative.cc \
		ePub3/ThirdParty/google-url/src/url_canon_stdurl.cc \
		ePub3/ThirdParty/google-url/src/url_parse.cc \
		ePub3/ThirdParty/google-url/src/url_parse_file.cc \
		ePub3/ThirdParty/google-url/src/url_util.cc \
		ePub3/ThirdParty/google-url/src/url_canon_cpp11.cc \
		ePub3/ThirdParty/google-url/base/logging.cc \
		ePub3/ePub/content_handler.cpp \
		ePub3/ePub/switch_preprocessor.cpp \
		ePub3/ePub/object_preprocessor.cpp \
		ePub3/ePub/media_support_info.cpp \
		ePub3/utilities/byte_stream.cpp \
		ePub3/utilities/ring_buffer.cpp

include $(BUILD_SHARED_LIBRARY)