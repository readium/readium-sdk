LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE:= xml2
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_SRC_FILES := \
	SAX.c \
	entities.c \
	encoding.c \
	error.c \
	parserInternals.c \
	parser.c \
	tree.c \
	hash.c \
	list.c \
	xmlIO.c \
	xmlmemory.c \
	uri.c \
	valid.c \
	xlink.c \
	HTMLparser.c \
	HTMLtree.c \
	debugXML.c \
	xpath.c \
	xpointer.c \
	xinclude.c \
	nanohttp.c \
	nanoftp.c \
	DOCBparser.c \
	catalog.c \
	globals.c \
	threads.c \
	c14n.c \
	xmlstring.c \
	xmlregexp.c \
	xmlschemas.c \
	xmlschemastypes.c \
	xmlunicode.c \
	xmlreader.c \
	relaxng.c \
	dict.c \
	SAX2.c \
	legacy.c \
	chvalid.c \
	pattern.c \
	xmlsave.c \
	xmlmodule.c \
	xmlwriter.c \
	schematron.c
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include # $(LOCAL_PATH)/../icu4c/common
# LOCAL_SHARED_LIBRARIES += $(common_SHARED_LIBRARIES)

include $(BUILD_STATIC_LIBRARY)


# For the host
# ========================================================
# 
# include $(CLEAR_VARS)
# LOCAL_SRC_FILES := $(common_SRC_FILES)
# LOCAL_C_INCLUDES += $(common_C_INCLUDES) $(LOCAL_PATH)/../icu4c/common
# LOCAL_SHARED_LIBRARIES += $(common_SHARED_LIBRARIES)
# LOCAL_MODULE:= libxml2
# include $(BUILD_HOST_STATIC_LIBRARY)
