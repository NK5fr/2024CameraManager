#include "spincameraproperty.h"
#include <QDebug>

using namespace CameraManagerSpin;

SpinCameraProperty::SpinCameraProperty(CameraManagerSpin::PropertyType type, QString name, double min, double max, int decimals, bool canAuto, bool isAuto, bool onOff, double value)
    : type(type), name(name), min(min), max(max), decimals(decimals), canAuto(canAuto), isAuto(isAuto), onOff(onOff), val(value) {
}
