QT += core gui widgets opengl network

HEADERS += \
    src/abstractcamera.h \
    src/abstractcameramanager.h \
    src/calibrationfile.h \
    src/calibrationviewerwidget.h \
    src/cameraproperty.h \
    src/configfilereader.h \
    src/configfileviewerwidget.h \
    src/databuffer.h \
    src/datastructs.h \
    src/emptycameramanager.h \
    src/externalprocess.h \
    src/flycamera.h \
    src/flycameramanager.h \
    src/imagedetect.h \
    src/imagelabel.h \
    src/imageselectpointwidget.h \
    src/imageviewerwidget.h \
    src/imagewidget.h \
    src/mainwindow.h \
    src/menubar.h \
    src/qopenglvideowidget.h \
    src/qvideowidget.h \
    src/settings.h \
    src/socketviewerwidget.h \
    src/testcamera.h \
    src/testcameramanager.h \
    src/texture.h \
    src/trackpointproperty.h \
    src/ui_mainwindow.h \
    src/widgetgl.h \
    include/AVIRecorder.h \
    include/BusManager.h \
    include/Camera.h \
    include/CameraBase.h \
    include/Error.h \
    include/FlyCapture2.h \
    include/FlyCapture2Defs.h \
    include/FlyCapture2GUI.h \
    include/FlyCapture2Platform.h \
    include/GigECamera.h \
    include/Image.h \
    include/ImageStatistics.h \
    include/PGRDirectShow.h \
    include/TopologyNode.h \
    include/Utilities.h

SOURCES += \
    src/abstractcamera.cpp \
    src/abstractcameramanager.cpp \
    src/calibrationfile.cpp \
    src/calibrationviewerwidget.cpp \
    src/cameraproperty.cpp \
    src/configfilereader.cpp \
    src/configfileviewerwidget.cpp \
    src/databuffer.cpp \
    src/emptycameramanager.cpp \
    src/externalprocess.cpp \
    src/flycamera.cpp \
    src/flycameramanager.cpp \
    src/imagedetect.cpp \
    src/imagelabel.cpp \
    src/imageselectpointwidget.cpp \
    src/imageviewerwidget.cpp \
    src/imagewidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/menubar.cpp \
    src/qopenglvideowidget.cpp \
    src/qvideowidget.cpp \
    src/socketviewerwidget.cpp \
    src/testcamera.cpp \
    src/testcameramanager.cpp \
    src/texture.cpp \
    src/ui_mainwindow.cpp \
    src/widgetgl.cpp

unix {
    CONFIG += c++11
    LIBS += -lflycapture -lGLU
}

RESOURCES += \
    CameraManager/cameramanager.qrc

win32: LIBS += -L$$PWD/lib/ -lFlyCapture2_v100

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
