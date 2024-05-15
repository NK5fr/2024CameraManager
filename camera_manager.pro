m# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = app
TARGET = camera_manager
DESTDIR = ./debug
QT += core opengl network gui widgets openglwidgets
CONFIG += debug

win32{
DEFINES += _WINDOWS _UNICODE _ENABLE_EXTENDED_ALIGNED_STORAGE WIN64 _WINDOWS _UNICODE _ENABLE_EXTENDED_ALIGNED_STORAGE WIN64 QT_OPENGL_LIB QT_WIDGETS_LIB QT_NETWORK_LIB QT_OPENGL_LIB QT_WIDGETS_LIB
INCLUDEPATH += $$PWD/GeneratedFiles/$(ConfigurationName)
INCLUDEPATH += $$PWD/GeneratedFiles
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/GeneratedFiles/$(ConfigurationName)
INCLUDEPATH += $$PWD/GeneratedFiles
INCLUDEPATH += $$PWD/include/Spinnaker
INCLUDEPATH += /QT/6.7.0/msvc2019_64/include
INCLUDEPATH += /QT/6.7.0/msvc2019_64/include/QtOpenGL
INCLUDEPATH += /QT/6.7.0/msvc2019_64/include/QtWidgets
INCLUDEPATH += /QT/6.7.0/msvc2019_64/include/QtGui
INCLUDEPATH += /QT/6.7.0/msvc2019_64/include/QtANGLE
INCLUDEPATH += /QT/6.7.0/msvc2019_64/include/QtNetwork
INCLUDEPATH += /QT/6.7.0/msvc2019_64/include/QtCore
INCLUDEPATH += /QT/6.7.0/msvc2019_64/include/QtOpenGLWidgets
INCLUDEPATH += $$PWD/debug
INCLUDEPATH += $$PWD/../../../../../VulkanSDK/1.0.51.0/include
INCLUDEPATH += $(QTDIR)/mkspecs/win-32-msvc
INCLUDEPATH += /QT/5.12.0/MSVC2015_64/mkspecs/win32-msvc
INCLUDEPATH += $$PWD/../../../../../Program
INCLUDEPATH += $$PWD/Files/Point
INCLUDEPATH += $$PWD/Grey
INCLUDEPATH += $$PWD/Research/FlyCapture2/include
INCLUDEPATH += $$PWD/Files
INCLUDEPATH += $$PWD/(x86)/Point
INCLUDEPATH += $$PWD/$(Configuration)
INCLUDEPATH += $$PWD/../../../../../VulkanSDK/1.0.51.0/include
LIBS += -L"$$PWD/lib/" -lSpinnaker_v140
#LIBS += -L"$$PWD/lib"
#LIBS += -l$$PWD/FlyCapture2_v100
#LIBS += -l$$PWD/Spinnaker_v140
#LIBS += -L"$$PWD/../../../../../Program Files/Point Grey Research/FlyCapture2/lib64"
#LIBS += -L"$$PWD/../../../../../Program Files (x86)/Point Grey Research/FlyCapture2/lib"
#LIBS += -l"$$PWD/../../../../../Program Files (x86)/Windows Kits/10/lib/10.0.17763.0/um/x64/opengl32"
#LIBS += -l"$$PWD/../../../../../Program Files (x86)/Windows Kits/10/lib/10.0.17763.0/um/x64/glu32"
#LIBS += -l"$$PWD/../../../../../Program Files/Point Grey Research/FlyCapture2/lib64/FlyCapture2d_v100"
LIBS += -lopengl32
LIBS += -lglu32
}

unix {
    CONFIG += c++11
#QMAKE_CXXFLAGS += -std=gnu++0x
    INCLUDEPATH += $$PWD/include/
    #LIBS += -lflycapture
    LIBS += -L"/opt/spinnaker/lib/" -lGenApi_gcc11_v3_0
    LIBS += -L"/opt/spinnaker/lib/" -lGCBase_gcc11_v3_0
    LIBS += -lGLU
}

unix:!macx: LIBS += -L$$PWD/../../../../opt/spinnaker/lib/ -lSpinnaker

INCLUDEPATH += $$PWD/../../../../opt/spinnaker/include
DEPENDPATH += $$PWD/../../../../opt/spinnaker/include





DEPENDPATH += $$PWD
MOC_DIR += $$PWD
OBJECTS_DIR += debug
UI_DIR += $$PWD/GeneratedFiles
RCC_DIR += $$PWD
include(camera_manager.pri)
