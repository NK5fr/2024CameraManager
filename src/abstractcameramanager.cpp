
#include "abstractcameramanager.h"

using namespace CameraManager;
using namespace std;

Q_DECLARE_METATYPE(AbstractCamera *)
Q_DECLARE_METATYPE(QVideoWidget *)
Q_DECLARE_METATYPE(CameraManager::CameraProperty *)

AbstractCameraManager::AbstractCameraManager(bool empty) : liveView(false), cameraTree() , newCameraList(), propertiesList(), selectedItem(NULL), selectedCamera(NULL), folderIcon(":/icons/folder"), activeCameras(), cameraProperties() {
    updateProps = true;
    propertiesList.setRootIsDecorated(false);
    propertiesList.setColumnCount(4);
    propertiesList.setHeaderLabels(QStringList() << "Property" << "Auto" << "Write value" << "Read value" << "Slider");
    
    if(empty) return;
    QObject::connect(&cameraTree, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_CameraTree_itemChanged(QStandardItem*)));
    //cameraTree.setColumnCount(4);
    cameraTree.setHorizontalHeaderLabels(QStringList() << "Cameras" << "Serial" << "Model" << "Custom Name");
    //cameraTree.setHorizontalHeaderItem(0, new QStandardItem("Hallo"));
    cameraTree.appendRow(&newCameraList);
    //newCameraList.setIcon(QIcon(":/icons/folder_home"));
    newCameraList.setCheckable(true);
    newCameraList.setDragEnabled(false);
    //newCameraList.setCheckState(Qt::Checked);
    newCameraList.setEditable(false);
}

void AbstractCameraManager::setMainWindow(MainWindow* window){
    mainWindow = window;
}

QStandardItemModel* AbstractCameraManager::getModel(){
    return &cameraTree;
}

QTreeWidget *AbstractCameraManager::getPropertiesWidget(){
    return &propertiesList;
}

void AbstractCameraManager::updateImages(){
    for(int i=activeCameras.size()-1; i>=0; i--){
        activeCameraEntry& camEntry = activeCameras.at(i);
        QVideoWidget* videoWidget = qobject_cast<QVideoWidget*>(camEntry.window->widget());
        unsigned char* imgBuffer = nullptr;
        unsigned int bufferSize = 0;
        unsigned int imageWidth = 0;
        unsigned int imageHeight = 0;
        imgBuffer = camEntry.camera->retrieveImage(&bufferSize, &imageWidth, &imageHeight);
        if (imgBuffer != nullptr) {
            videoWidget->setImage(imgBuffer, bufferSize, imageWidth, imageHeight);
        }
    }
}

void AbstractCameraManager::updateProperties() {
    //if (!updateProps) return;
    AbstractCamera* selected = selectedCamera;
    if (selected == nullptr) return;
    for( int i = propertiesList.topLevelItemCount()-1; i>=0; i--){
        QTreeWidgetItem* item = propertiesList.topLevelItem(i);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(propertiesList.itemWidget(item, Ui::PropertyAuto));
        CameraManager::CameraProperty * prop = reinterpret_cast<CameraManager::CameraProperty*>(checkBox->property("CameraProperty").value<quintptr>());
        QSlider* slider = reinterpret_cast<QSlider*>(checkBox->property("TreeWidgetSlider").value<quintptr>());
        //qDebug() << "updating:" << prop->getName().c_str();

        // Lars Aksel - 09.02.2015 - Changed from updateProperties to setProperties
        //selected->updateProperty(prop);
        selected->setProperty(prop);

        item->setText(Ui::PropertyWriteValue, prop->formatValue());
        checkBox->setChecked(prop->getAuto());

        CameraManager::CameraProperty readProp(prop->getType(), prop->getName(), prop->getMin(), prop->getMax(), prop->getDecimals(), prop->getCanAuto(), prop->getAuto(), prop->getOnOff(), prop->getValue());
        selected->getProperty(&readProp);
        QString val = readProp.formatValue();
        //item->setText(Ui::PropertyReadValue, readProp.formatValue());

        QLineEdit* valueBox = (QLineEdit*) propertiesList.itemWidget(item, Ui::PropertyReadValue);
        if (valueBox != nullptr) valueBox->setText(val);

        if( prop->getType() == CameraManager::AUTOTRIGGER ) continue;
        slider->setValue(prop->getValueToSlider());
        slider->setEnabled(!prop->getAuto());
    }
}

// Lars Aksel - Helper function for getting values
void setPropFromSettings(QSettings& settings, std::vector<CameraProperty>& prop, CameraManager::PropertyType val) {
    CameraProperty camProp(val,
        settings.value("text").toString(),
        settings.value("min").toFloat(),
        settings.value("max").toFloat(),
        settings.value("decimals").toFloat(),
        true,
        settings.value("auto").toBool(),
        settings.value("on_off").toBool(),
        settings.value("value").toFloat());
    for (unsigned int i = 0; i < prop.size(); i++) {
        if (prop[i].getType() == val) {
            memcpy(&prop[i], &camProp, sizeof(camProp));
            break;
        }
    }
}

void setNewPropFromSettings(QSettings& settings, std::vector<CameraProperty>& prop, CameraManager::PropertyType val) {
    prop.push_back(CameraProperty(val,
        settings.value("text").toString(),
        settings.value("min").toFloat(),
        settings.value("max").toFloat(),
        settings.value("decimals").toFloat(),
        true,
        settings.value("auto").toBool(),
        settings.value("on_off").toBool(),
        settings.value("value").toFloat()));
}

bool AbstractCameraManager::updatePropertiesFromFile(QString& filepath, std::vector<CameraProperty>& prop) {
    QSettings settings(filepath, QSettings::IniFormat);
    QFileInfo fileInfo = QFileInfo(filepath);
    if (fileInfo.exists()) {
        settings.beginGroup("Brightness");
        setPropFromSettings(settings, prop, CameraManager::BRIGHTNESS);
        settings.endGroup();
        settings.beginGroup("Gain");
        setPropFromSettings(settings, prop, CameraManager::GAIN);
        settings.endGroup();
        settings.beginGroup("Exposure");
        setPropFromSettings(settings, prop, CameraManager::EXPOSURE);
        settings.endGroup();
        settings.beginGroup("Gamma");
        setPropFromSettings(settings, prop, CameraManager::GAMMA);
        settings.endGroup();
        settings.beginGroup("Shutter");
        setPropFromSettings(settings, prop, CameraManager::SHUTTER);
        settings.endGroup();
        settings.beginGroup("Pan");
        setPropFromSettings(settings, prop, CameraManager::PAN);
        settings.endGroup();
        settings.beginGroup("FrameRate");
        setPropFromSettings(settings, prop, CameraManager::FRAMERATE);
        settings.endGroup();
        settings.beginGroup("AutoTrigger");
        setPropFromSettings(settings, prop, CameraManager::AUTOTRIGGER);
        settings.endGroup();
        return true;
    } else {
        printf("Settings file does not exist: %s\n", qPrintable(filepath));
        return false;
    }
}

// Lars Aksel - Helper function for setting values
void setPropToSettings(QSettings& settings, CameraProperty& prop) {
    settings.setValue("text", prop.getName());
    settings.setValue("min", QString::number(prop.getMin()));
    settings.setValue("max", QString::number(prop.getMax()));
    settings.setValue("decimals", QString::number(prop.getDecimals()));
    settings.setValue("abs_control", (prop.getDecimals() > 0));
    settings.setValue("auto", prop.getAuto());
    settings.setValue("on_off", prop.getOnOff());
    settings.setValue("value", QString::number(prop.getValue()));
}

// Lars Aksel - Load default values from default-file
void AbstractCameraManager::loadPropertiesDefaults() {
    QString settingsFile = QString(QDir::currentPath() + "/" + PROPERTY_PATH + "/" + CAMERA_PROPERTY_DEFAULT_FILE);
    std::vector<CameraProperty> prop = std::vector<CameraProperty>();
    //std::vector<CameraProperty> prop;
    loadPropertiesFromFile(settingsFile, prop);
    updateProperties(prop);
}

void AbstractCameraManager::loadPropertiesDefaultsInit() {
    QString settingsFile = QString(QDir::currentPath() + "/" + PROPERTY_PATH + "/" + CAMERA_PROPERTY_DEFAULT_FILE);
    //std::vector<CameraProperty> prop = std::vector<CameraProperty>();
    std::vector<CameraProperty> prop;
    loadPropertiesFromFile(settingsFile, prop);
    setProperties(prop);
}

// Lars Aksel - Load camera properties from file
bool AbstractCameraManager::loadPropertiesFromFile(QString& filepath, std::vector<CameraProperty>& prop) {
    QSettings settings(filepath, QSettings::IniFormat);
    QFileInfo fileInfo = QFileInfo(filepath);
    if (fileInfo.exists()) {
        settings.beginGroup("Brightness");
        setNewPropFromSettings(settings, prop, CameraManager::BRIGHTNESS);
        settings.endGroup();
        settings.beginGroup("Gain");
        setNewPropFromSettings(settings, prop, CameraManager::GAIN);
        settings.endGroup();
        settings.beginGroup("Exposure");
        setNewPropFromSettings(settings, prop, CameraManager::EXPOSURE);
        settings.endGroup();
        settings.beginGroup("Gamma");
        setNewPropFromSettings(settings, prop, CameraManager::GAMMA);
        settings.endGroup();
        settings.beginGroup("Shutter");
        setNewPropFromSettings(settings, prop, CameraManager::SHUTTER);
        settings.endGroup();
        settings.beginGroup("Pan");
        setNewPropFromSettings(settings, prop, CameraManager::PAN);
        settings.endGroup();
        settings.beginGroup("FrameRate");
        setNewPropFromSettings(settings, prop, CameraManager::FRAMERATE);
        settings.endGroup();
        settings.beginGroup("AutoTrigger");
        setNewPropFromSettings(settings, prop, CameraManager::AUTOTRIGGER);
        settings.endGroup();
        return true;
    } else {
        printf("Settings file does not exist: %s\n", qPrintable(filepath));
        return false;
    }
}

// Lars Aksel - Save camera properties to file
bool AbstractCameraManager::savePropertiesToFile(QString& filepath) {
    QSettings settings(filepath, QSettings::IniFormat);
    settings.beginGroup("Brightness");
    setPropToSettings(settings, cameraProperties[0]);
    settings.endGroup();
    settings.beginGroup("Gain");
    setPropToSettings(settings, cameraProperties[1]);
    settings.endGroup();
    settings.beginGroup("Exposure");
    setPropToSettings(settings, cameraProperties[2]);
    settings.endGroup();
    settings.beginGroup("Gamma");
    setPropToSettings(settings, cameraProperties[3]);
    settings.endGroup();
    settings.beginGroup("Shutter");
    setPropToSettings(settings, cameraProperties[4]);
    settings.endGroup();
    settings.beginGroup("Pan");
    setPropToSettings(settings, cameraProperties[5]);
    settings.endGroup();
    settings.beginGroup("FrameRate");
    setPropToSettings(settings, cameraProperties[6]);
    settings.endGroup();
    settings.beginGroup("AutoTrigger");
    setPropToSettings(settings, cameraProperties[7]);
    settings.endGroup();
    return true;
}

void AbstractCameraManager::activateLiveView(bool active){
    liveView = active;
    if (active) {
        for(int i=activeCameras.size()-1; i>=0; i--){
            activeCameraEntry& camEntry = activeCameras.at(i);
            QVideoWidget* videoWidget = qobject_cast<QVideoWidget*>(camEntry.window->widget());
            camEntry.camera->startCapture(videoWidget);
        }
    } else {
        for(int i=activeCameras.size()-1; i>=0; i--){
            activeCameraEntry& camEntry = activeCameras.at(i);
            camEntry.camera->stopAutoCapture();
        }
    }
}

///////////////////////////////////////////////////
/////////////// Camera List related ///////////////
///////////////////////////////////////////////////
QModelIndex AbstractCameraManager::detectNewCamerasAndExpand() {
    std::vector<AbstractCamera*> newCameras;
    std::vector<QStandardItem*> oldCameras;
    cameraTree_getCameraList(cameraTree.invisibleRootItem(), &oldCameras);
    detectNewCameras(&newCameras);
    //cout << "oldCameras" << oldCameras.size() << endl;
    //cout << "newCameras" << newCameras.size() << endl;
    //removing disconnected cameras
    for (unsigned int i = 0; i < oldCameras.size(); i++) {
        QStandardItem* item = oldCameras.at(i);
        AbstractCamera* cam = reinterpret_cast<AbstractCamera*>(item->data(CameraRole).value<quintptr>());
        /*cout << "oldCameras(" << i << "):" << cam << endl;*/
        bool found = false;
        for (int j = newCameras.size() - 1; j >= 0; j--) {
            if (cam->equalsTo(newCameras.at(j))) {
                found = true;
                delete newCameras[j]; // Fix for memory-leak - Lars Aksel - 19.04.2015
                newCameras.erase(newCameras.begin()+j);
                continue;
            }
        }
        if (!found){ //remove if disconnected
            activateCamera(cam, item, false);
            item->parent()->removeRow(item->row());
        }
    }
    //adding new cameras
    for (unsigned int i = 0; i < newCameras.size(); i++) {
        AbstractCamera* cam = newCameras.at(i);
        //QStandardItem* item = new QStandardItem(cam->getString().c_str());
        QList<QStandardItem*> items;
        QStandardItem* item = new QStandardItem();
        item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(cam)), CameraRole);
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setDropEnabled(false);
        item->setEditable(false);
        items.append(item);

        item = new QStandardItem(cam->getSerial());
        item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(cam)), CameraRole);
        item->setEditable(false);
        items.append(item);

        item = new QStandardItem(cam->getModel());
        item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(cam)), CameraRole);
        item->setEditable(false);
        items.append(item);

        items.append(new QStandardItem(cam->getCustomName()));

        newCameraList.appendRow(items);
    }
    return newCameraList.index();
}

QModelIndex AbstractCameraManager::addGroup(){
    QStandardItem *newGroup = new QStandardItem("new Group");
    newGroup->setCheckable(true);
    newGroup->setIcon(folderIcon);
    if( selectedItem != NULL && !selectedItem->data(CameraRole).isValid()) selectedItem->appendRow(newGroup);
    else cameraTree.appendRow(newGroup);
    return newGroup->index();
}

void AbstractCameraManager::removeGroup(QModelIndex index){
    QStandardItem * item = getModel()->itemFromIndex( index );
    if (!item->isEditable()) return;

    QStandardItem * parent = item->parent();
    if (parent == NULL) parent = item->model()->invisibleRootItem();

    for (int i = item->rowCount(); i > 0; i--){
        newCameraList.insertRow(0, item->takeRow(0));
    }
    parent->removeRow( item->row() );
    detectNewCamerasAndExpand();
}

void AbstractCameraManager::resetItem(QModelIndex index){
    QStandardItem * item = getModel()->itemFromIndex( index );
    if( item == NULL || !item->data(CameraRole).isValid() ) return;
    AbstractCamera* cam = reinterpret_cast<AbstractCamera *>(item->data(CameraRole).value<quintptr>());
    item->setText(cam->getString().c_str());
}

void AbstractCameraManager::activateCamera(AbstractCamera* camera, QStandardItem* item, bool active){
    cout << "activateCamera( " << camera << ", " << active << ")" << endl;
    int i = activeCameras.size() - 1;
    while (i >= 0 && activeCameras.at(i).camera != camera) --i;

    if (i >= 0){
        if (!active) {
            activeCameraEntry* entry = &activeCameras.at(i);
            entry->camera->stopAutoCapture();
            mainWindow->modifySubWindow(entry->window, false);
            activeCameras.erase(activeCameras.begin()+i);
        } else {
            activeCameras.at(i).window->setWindowTitle(item->text());
        }
    } else {
        if (active) {
            activeCameraEntry entry = activeCameraEntry(camera, item); // Potential Memory-leak of created windows in constructor!
            connect(entry.window, SIGNAL(destroyed(QObject*)), this, SLOT(on_subwindow_closing(QObject*)));
            entry.window->setWindowTitle(camera->getString().c_str());
            mainWindow->modifySubWindow(entry.window, true);
            QVideoWidget* videoWidget = qobject_cast<QVideoWidget*>(entry.window->widget());
            camera->setVideoContainer(videoWidget);
            activeCameras.push_back(entry);
            connect(mainWindow, SIGNAL(activateCrosshair(bool)), qobject_cast<QVideoWidget*>(entry.window->widget()), SLOT(activateCrosshair(bool)));
            if(liveView) entry.camera->startCapture(qobject_cast<QVideoWidget *>(entry.window->widget()));
        }
    }
}

void AbstractCameraManager::on_subwindow_closing(QObject *window) {
    cout << "closing " << window << endl;
    int i = activeCameras.size()-1;
    while(i>=0 && activeCameras.at(i).window != window) --i;

    if(i>=0) activeCameras.at(i).treeItem->setCheckState(Qt::Unchecked);
}

void AbstractCameraManager::on_CameraTree_itemChanged(QStandardItem* item) {
    Qt::CheckState checked = item->checkState();
    if( item->data(CameraRole).isValid() ){
        activateCamera(reinterpret_cast<AbstractCamera*>(item->data(CameraRole).value<quintptr>()), item, checked == Qt::Checked);
    } else {
        cameraTree_recursiveCheck(item, checked);
    }
}

void AbstractCameraManager::cameraTree_itemClicked(const QModelIndex &index, QString &string, int &icon, bool &editable, bool &deleteable){
    QStandardItem* clicked = getModel()->itemFromIndex(index);
    selectedItem = clicked;
    QStandardItem* first = nullptr;
    
    string = "No Selection";
    editable = true;
    deleteable = true;
    if (clicked->data(CameraRole).isValid()) {
        icon = 0;
        deleteable = false;
        first = clicked;
        QModelIndex textIndex = getModel()->index(index.row(), 1, index.parent());
        string = getModel()->itemFromIndex(textIndex)->text() + " - ";
        textIndex = getModel()->index(index.row(), 2, index.parent());
        string.append(getModel()->itemFromIndex(textIndex)->text());
    } else {
        if (clicked == &newCameraList) {
            //cout << "clicked == newCameraList" << endl;
            editable = false;
            deleteable = false;
        }
        first = cameraTree_recursiveFirstCamera(clicked);
        if (first == nullptr) icon = 1;
        else {
            icon = 2;
            // Lars Aksel - 27.01.2015 - Removed unnecessary text
            //string = clicked->text() + " <br /> (" + first->text() + ")";
        }
    }
    selectedCamera = (first == nullptr) ? nullptr : reinterpret_cast<AbstractCamera*>(first->data(CameraRole).value<quintptr>());
    updateProperties();
}

///////////////////////////////////////////////////
//////////// Recursive in Camera List /////////////
///////////////////////////////////////////////////
//uncheck all
void AbstractCameraManager::desactiveAllCameras(){
    cameraTree_recursiveCheck(cameraTree.invisibleRootItem(), Qt::Unchecked);
}

// check QStandardItem and its decendants
void AbstractCameraManager::cameraTree_recursiveCheck(QStandardItem* parent, Qt::CheckState checked){
    /*cout << "recursivecheck(" << checked << ") on " << parent->text() << endl;*/
    for(int i=0; i<parent->rowCount(); ++i){
         QStandardItem* currItem = parent->child(i);
         if(currItem->checkState() != checked) currItem->setCheckState(checked);
         else cameraTree_recursiveCheck(currItem, checked);
    }
}

// get first AbstractCamera in QStandardItem
// @return first found AbstractCamera
QStandardItem* AbstractCameraManager::cameraTree_recursiveFirstCamera(QStandardItem* parent){
    QVariant data = parent->data(CameraRole);
    if(data.isValid()) return parent;

    for (int i = 0; i < parent->rowCount(); ++i) {
        QStandardItem* tmp = cameraTree_recursiveFirstCamera(parent->child(i));
        if(tmp != nullptr) return tmp;
    }
    return nullptr;
}

// set a property for all the AbstractCamera in QStandardItem and its decendants
void AbstractCameraManager::cameraTree_recursiveSetProperty(QStandardItem* parent, CameraManager::CameraProperty* prop){
    QVariant data = parent->data(CameraRole);
    if (data.isValid()) {
        reinterpret_cast<AbstractCamera *>(data.value<quintptr>())->setProperty(prop);
        return;
    }
    for (int i = 0; i < parent->rowCount(); ++i) {
        cameraTree_recursiveSetProperty(parent->child(i), prop);
    }
}

//add to the vector all found AbstractCamera in QStandardItem and its decendants
void AbstractCameraManager::cameraTree_getCameraList(QStandardItem* parent, std::vector<QStandardItem*> *list){
    QVariant data = parent->data(CameraRole);
    if (data.isValid()) {
        list->push_back(parent);
        return;
    }
    for (int i = 0; i < parent->rowCount(); ++i) {
        cameraTree_getCameraList(parent->child(i), list);
    }
}

///////////////////////////////////////////////////
//////////////// Properties related ///////////////
///////////////////////////////////////////////////
void AbstractCameraManager::setProperties(std::vector<CameraProperty> &properties) {
    //propertiesList.clear();
    cameraProperties = std::vector<CameraProperty>(properties);
    for(unsigned int i = 0; i < cameraProperties.size(); i++){
        CameraProperty &property = cameraProperties.at(i);
        QTreeWidgetItem* it = new QTreeWidgetItem();
        it->setText(Ui::PropertyName, property.getName());
        propertiesList.addTopLevelItem(it);
        //checkbox
        QCheckBox* box = new QCheckBox();
        box->setProperty("CameraProperty", QVariant::fromValue(reinterpret_cast<quintptr>(&property)));
        box->setChecked(property.getAuto());
        if (!property.getCanAuto()) box->setEnabled(false);
        propertiesList.setItemWidget(it, Ui::PropertyAuto, box);
        connect(box, SIGNAL(stateChanged(int)), this, SLOT(on_propertyCheckbox_changed(int)));

        if (property.getType() == CameraManager::AUTOTRIGGER) continue;

        QSlider* slider = new QSlider(Qt::Horizontal);
        QLineEdit* writeValueBox = new QLineEdit(property.formatValue());
        const int boxWidth = 50;
        writeValueBox->setFixedWidth(boxWidth);
        writeValueBox->setProperty("TreeWidgetSlider", QVariant::fromValue(reinterpret_cast<quintptr>(slider)));
        writeValueBox->setProperty("CameraProperty", QVariant::fromValue(reinterpret_cast<quintptr>(&property)));
        writeValueBox->setProperty("TreeWidgetItem", QVariant::fromValue(reinterpret_cast<quintptr>(it)));
        writeValueBox->setFrame(false);
        QDoubleValidator* validator = new QDoubleValidator(property.getMin(), property.getMax(), property.getDecimals(), this);
        validator->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        writeValueBox->setValidator(validator);
        propertiesList.setItemWidget(it, Ui::PropertyWriteValue, writeValueBox);
        connect(writeValueBox, SIGNAL(returnPressed()), this, SLOT(on_propertyValue_changed()));



        QLineEdit* readValueBox = new QLineEdit(property.formatValue());
        readValueBox->setFixedWidth(boxWidth);
        readValueBox->setFrame(false);
        readValueBox->setReadOnly(true);
        propertiesList.setItemWidget(it, Ui::PropertyReadValue, readValueBox);

        //slider
        slider->setProperty("CameraProperty", QVariant::fromValue(reinterpret_cast<quintptr>(&property)));
        slider->setProperty("ValueBox", QVariant::fromValue(reinterpret_cast<quintptr>(writeValueBox)));
        slider->setTracking(true); //might be wanted
        slider->setValue(property.getValueToSlider());
        slider->setRange(property.getMinToSlider(), property.getMaxToSLider());
        propertiesList.setItemWidget(it, Ui::PropertySlider, slider);

        box->setProperty("TreeWidgetSlider", QVariant::fromValue(reinterpret_cast<quintptr>(slider)));
        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_propertySlider_changed(int)));
    }
    propertiesList.resizeColumnToContents(0);
    propertiesList.resizeColumnToContents(1);
    propertiesList.resizeColumnToContents(2);
    propertiesList.resizeColumnToContents(3);
    propertiesList.resizeColumnToContents(4);
}

// Lars Aksel - Update Properties-UI elements.
void AbstractCameraManager::updateProperties(std::vector<CameraProperty> &properties) {
    //cameraProperties = std::vector<CameraProperty>(properties);
    for (unsigned int i = 0; i < cameraProperties.size(); i++){
        QTreeWidgetItem* it = propertiesList.topLevelItem(i);
        CameraProperty &property = properties.at(i);
        for (int j = activeCameras.size() - 1; j >= 0; j--){
            activeCameras.at(j).camera->setProperty(&property);
        }
        QCheckBox* box = (QCheckBox*) propertiesList.itemWidget(it, Ui::PropertyAuto);
        box->setChecked(property.getAuto());
        if (!property.getCanAuto()) box->setEnabled(false);
        if (property.getType() == CameraManager::AUTOTRIGGER) continue;
        QLineEdit* valueBox = (QLineEdit*) propertiesList.itemWidget(it, Ui::PropertyWriteValue);
        valueBox->setText(QString::number(property.getValue()));
        
        CameraManager::CameraProperty readProp(property.getType(), property.getName(), property.getMin(), property.getMax(), property.getDecimals(), property.getCanAuto(), property.getAuto(), property.getOnOff(), property.getValue());
        selectedCamera->getProperty(&readProp);
        QString val = readProp.formatValue();
        //it->setText(Ui::PropertyReadValue, readProp.formatValue());
        QLineEdit* readValueBox = (QLineEdit*) propertiesList.itemWidget(it, Ui::PropertyReadValue);
        //readValueBox->setText(readProp.formatValue());

        QSlider* slider = (QSlider*) propertiesList.itemWidget(it, Ui::PropertySlider);
        slider->setValue(property.getValueToSlider());
        slider->setRange(property.getMinToSlider(), property.getMaxToSLider());        
    }
}

void AbstractCameraManager::on_propertyCheckbox_changed(int state){
    if (selectedItem == NULL) return;
    CameraProperty* prop = reinterpret_cast<CameraProperty*>( sender()->property("CameraProperty").value<quintptr>() );
    cout << sender() << qPrintable(prop->getName()) << endl;
    prop->setAuto(state == Qt::Checked);
    cameraTree_recursiveSetProperty(selectedItem, prop);

    //(de)activate slider
    if (prop->getType() == CameraManager::AUTOTRIGGER) return;
    QSlider* slider =  reinterpret_cast<QSlider*>( sender()->property("TreeWidgetSlider").value<quintptr>());
    cout << "Qslider" << slider << endl;
    if (state != Qt::Checked) {
        slider->setEnabled(true);
        slider->setValue(prop->getValueToSlider());
    } else {
        slider->setEnabled(false);
    }
}

void AbstractCameraManager::on_propertySlider_changed(int val){
    if (selectedItem == NULL) return;
    CameraProperty* prop = reinterpret_cast<CameraProperty*>( sender()->property("CameraProperty").value<quintptr>() );
    prop->setValueFromSlider(val);
    cameraTree_recursiveSetProperty(selectedItem, prop);
    reinterpret_cast<QLineEdit*>(sender()->property("ValueBox").value<quintptr>())->setText(QString::number(prop->getValue()));
}

void AbstractCameraManager::on_propertyValue_changed() {
    if (selectedItem == NULL) return;
    CameraProperty* prop = reinterpret_cast<CameraProperty*>(sender()->property("CameraProperty").value<quintptr>());
    QString valueS = reinterpret_cast<QLineEdit*>(sender())->text();
    QSlider* slider = reinterpret_cast<QSlider*>(sender()->property("TreeWidgetSlider").value<quintptr>());
    valueS = valueS.replace(",", ".");
    prop->setValue(valueS.toFloat());
    slider->setValue(prop->getValueToSlider());
    cameraTree_recursiveSetProperty(selectedItem, prop);
}

