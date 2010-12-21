TEMPLATE = lib
TARGET = plv_niek_plugin
CONFIG += plugin
QT += core
QT -= gui
DESTDIR = ../../libs/plugins
INCLUDEPATH += ../../include/plvniek
LIBS += -lplvcore
QMAKE_LIBDIR += ../../libs/plugins

include(../../ParleVision.local)

DEFINES += PLV_NIEK_PLUGIN_LIBRARY

SOURCES +=  plvniek_plugin.cpp \
            Xor.cpp \
            Trigger.cpp \
#		   Sum.cpp \
            Split.cpp \
#		   ScalarToFile.cpp \
#		   OpenCVScalar.cpp \
            Multiply.cpp \
#		   ImageTypeScaleConversion.cpp \
            SaveImageToFile.cpp \
            ImageSequenceLoader.cpp \
            ImageLoader.cpp \
#		   DistanceTransform.cpp \
#                  MedianSmooth.cpp
		   	   

HEADERS +=  ../../include/plvniek/plvniek_plugin.h \
            ../../include/plvniek/plvniek_global.h \
            ../../include/plvniek/Xor.h \
            ../../include/plvniek/Trigger.h \
#                   ../../include/plvniek/Sum.h \
            ../../include/plvniek/Split.h \
#		    ../../include/plvniek/ScalarToFile.h \
#		    ../../include/plvniek/OpenCVScalar.h \
            ../../include/plvniek/Multiply.h \
#		    ../../include/plvniek/ImageTypeScaleConversion.h \
            ../../include/plvniek/SaveImageToFile.h \
            ../../include/plvniek/ImageSequenceLoader.h \
            ../../include/plvniek/ImageLoader.h \
#		    ../../include/plvniek/DistanceTransform.h \
#                   ../../include/plvniek/MedianSmooth.h
            
