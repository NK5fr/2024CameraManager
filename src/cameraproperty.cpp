#include "cameraproperty.h"
#include <QDebug>

using namespace CameraManager;

CameraProperty::CameraProperty(CameraManager::PropertyType type, QString name, double min, double max, int decimals, bool canAuto, bool isAuto, bool onOff, double value)
    : type(type), name(name), min(min), max(max), decimals(decimals), canAuto(canAuto), isAuto(isAuto), onOff(onOff), val(value) {
}