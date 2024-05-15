/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "activateCrosshair",
    "",
    "on_actionLiveView_toggled",
    "arg1",
    "on_actionUpdateImages_triggered",
    "on_actionCrosshair_toggled",
    "on_actionCrosshairReal_toggled",
    "on_actionMosaic_triggered",
    "on_actionRemoveWindows_triggered",
    "on_actionHighQuality_toggled",
    "menuBarClicked",
    "QAction*",
    "on_CameraTree_customContextMenuRequested",
    "menuCameraAction_triggered",
    "cameraTree_itemClicked",
    "QModelIndex",
    "cameraTree_itemDoubleClicked",
    "on_SelectCameras_currentIndexChanged",
    "index",
    "projectTree_customContextMenuRequested",
    "projectTree_doubleClicked",
    "projectTree_clicked",
    "menuProjectAction_triggered",
    "combobox_changeSDK",
    "quickLoadTrackPointSettings",
    "quickSaveTrackPointSettings",
    "loadDefaultTrackPointSettings_clicked",
    "loadDefaultCameraProperties_clicked",
    "on_TrackPointChecked",
    "state",
    "on_FilteredImageChecked",
    "on_ShowCoordinateLabelChecked",
    "on_RemoveDuplicatesChecked",
    "on_ShowMinSepCircleChecked",
    "on_TrackPointValueChanged",
    "on_TrackPointSliderValueChanged",
    "quickLoadCameraSettings",
    "quickSaveCameraSettings"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  206,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,  209,    2, 0x08,    3 /* Private */,
       5,    0,  212,    2, 0x08,    5 /* Private */,
       6,    1,  213,    2, 0x08,    6 /* Private */,
       7,    1,  216,    2, 0x08,    8 /* Private */,
       8,    0,  219,    2, 0x08,   10 /* Private */,
       9,    0,  220,    2, 0x08,   11 /* Private */,
      10,    1,  221,    2, 0x08,   12 /* Private */,
      11,    1,  224,    2, 0x08,   14 /* Private */,
      13,    1,  227,    2, 0x08,   16 /* Private */,
      14,    1,  230,    2, 0x08,   18 /* Private */,
      15,    1,  233,    2, 0x08,   20 /* Private */,
      17,    1,  236,    2, 0x08,   22 /* Private */,
      18,    1,  239,    2, 0x08,   24 /* Private */,
      20,    1,  242,    2, 0x08,   26 /* Private */,
      21,    1,  245,    2, 0x08,   28 /* Private */,
      22,    1,  248,    2, 0x08,   30 /* Private */,
      23,    1,  251,    2, 0x08,   32 /* Private */,
      24,    0,  254,    2, 0x08,   34 /* Private */,
      25,    0,  255,    2, 0x08,   35 /* Private */,
      26,    0,  256,    2, 0x08,   36 /* Private */,
      27,    0,  257,    2, 0x08,   37 /* Private */,
      28,    0,  258,    2, 0x08,   38 /* Private */,
      29,    1,  259,    2, 0x08,   39 /* Private */,
      31,    1,  262,    2, 0x08,   41 /* Private */,
      32,    1,  265,    2, 0x08,   43 /* Private */,
      33,    1,  268,    2, 0x08,   45 /* Private */,
      34,    1,  271,    2, 0x08,   47 /* Private */,
      35,    0,  274,    2, 0x08,   49 /* Private */,
      36,    1,  275,    2, 0x08,   50 /* Private */,
      37,    0,  278,    2, 0x08,   52 /* Private */,
      38,    0,  279,    2, 0x08,   53 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, 0x80000000 | 12,    2,
    QMetaType::Void, QMetaType::QPoint,    2,
    QMetaType::Void, 0x80000000 | 12,    2,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Void, QMetaType::QPoint,    2,
    QMetaType::Void, 0x80000000 | 16,   19,
    QMetaType::Void, 0x80000000 | 16,   19,
    QMetaType::Void, 0x80000000 | 12,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'activateCrosshair'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionLiveView_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionUpdateImages_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionCrosshair_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionCrosshairReal_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionMosaic_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionRemoveWindows_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionHighQuality_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'menuBarClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'on_CameraTree_customContextMenuRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'menuCameraAction_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'cameraTree_itemClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex, std::false_type>,
        // method 'cameraTree_itemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex, std::false_type>,
        // method 'on_SelectCameras_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'projectTree_customContextMenuRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'projectTree_doubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'projectTree_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'menuProjectAction_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'combobox_changeSDK'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'quickLoadTrackPointSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'quickSaveTrackPointSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loadDefaultTrackPointSettings_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loadDefaultCameraProperties_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_TrackPointChecked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_FilteredImageChecked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_ShowCoordinateLabelChecked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_RemoveDuplicatesChecked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_ShowMinSepCircleChecked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_TrackPointValueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_TrackPointSliderValueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'quickLoadCameraSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'quickSaveCameraSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->activateCrosshair((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->on_actionLiveView_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->on_actionUpdateImages_triggered(); break;
        case 3: _t->on_actionCrosshair_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->on_actionCrosshairReal_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->on_actionMosaic_triggered(); break;
        case 6: _t->on_actionRemoveWindows_triggered(); break;
        case 7: _t->on_actionHighQuality_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->menuBarClicked((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 9: _t->on_CameraTree_customContextMenuRequested((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 10: _t->menuCameraAction_triggered((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 11: _t->cameraTree_itemClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 12: _t->cameraTree_itemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 13: _t->on_SelectCameras_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->projectTree_customContextMenuRequested((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 15: _t->projectTree_doubleClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 16: _t->projectTree_clicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 17: _t->menuProjectAction_triggered((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 18: _t->combobox_changeSDK(); break;
        case 19: _t->quickLoadTrackPointSettings(); break;
        case 20: _t->quickSaveTrackPointSettings(); break;
        case 21: _t->loadDefaultTrackPointSettings_clicked(); break;
        case 22: _t->loadDefaultCameraProperties_clicked(); break;
        case 23: _t->on_TrackPointChecked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 24: _t->on_FilteredImageChecked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 25: _t->on_ShowCoordinateLabelChecked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 26: _t->on_RemoveDuplicatesChecked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 27: _t->on_ShowMinSepCircleChecked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 28: _t->on_TrackPointValueChanged(); break;
        case 29: _t->on_TrackPointSliderValueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 30: _t->quickLoadCameraSettings(); break;
        case 31: _t->quickSaveCameraSettings(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(bool );
            if (_t _q_method = &MainWindow::activateCrosshair; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 32)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 32;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 32)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 32;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::activateCrosshair(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
