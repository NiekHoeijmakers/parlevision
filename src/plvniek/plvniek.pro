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
#		   Trigger.cpp \
#		   Sum.cpp \
            Split.cpp \
#		   ScalarToFile.cpp \
#		   OpenCVScalar.cpp \
            Multiply.cpp \
#		   ImageTypeScaleConversion.cpp \
            SaveImageToFile.cpp \
#		   ImageThreshold.cpp \
#		   ImageSequenceLoader.cpp \
#		   ImageLoader.cpp \
#		   DistanceTransform.cpp
		   	   

HEADERS +=  ../../include/plvniek/plvniek_plugin.h \
            ../../include/plvniek/plvniek_global.h \
            ../../include/plvniek/Xor.h \
#           Trigger.h \
#           Sum.h \
            ../../include/plvniek/Split.h \
#		    ScalarToFile.h \
#		    OpenCVScalar.h \
            ../../include/plvniek/Multiply.h \
#		    ImageTypeScaleConversion.h \
            ../../include/SaveImageToFile.h \
#		    ImageThreshold.h \
#		    ImageSequenceLoader.h \
#		    ImageLoader.h \
#		    DistanceTransform.h
            
