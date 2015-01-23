#include "abstractcameramanager.h"

#include <QtCore>
#include <QDebug>
#include <QCheckBox>
#include <QSlider>
#include <algorithm>
#include <iostream>

using namespace CameraManager;
using namespace std;

Q_DECLARE_METATYPE(AbstractCamera *)
Q_DECLARE_METATYPE(QVideoWidget *)
Q_DECLARE_METATYPE(CameraManager::CameraProperty *)

using namespace CameraManager;

AbstractCameraManager::AbstractCameraManager(bool empty)
    : liveView(false), cameraTree() , newCameraList("Detected Cameras"), propertiesList(), selectedItem(NULL), selectedCamera(NULL), folderIcon(":/icons/folder"), activeCameras(), cameraProperties() {

    propertiesList.setRootIsDecorated(false);
    propertiesList.setColumnCount(4);
    propertiesList.setHeaderLabels(QStringList() << "Property" << "Auto" << "Value" << "Slider");


    if(empty) return;
    QObject::connect(&cameraTree, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(on_CameraTree_itemChanged(QStandardItem*)));
    cameraTree.appendRow(&newCameraList);
    newCameraList.setIcon( QIcon(":/icons/folder_home") );
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
        QVideoWidget* videoWidget = qobject_cast<QVideoWidget *>( camEntry.window->widget() );
        videoWidget->setImage(camEntry.camera->retrieveImage());
    }
}

void AbstractCameraManager::updateProperties(){
    AbstractCamera* selected = selectedCamera;
    if( selected == NULL ) return ;
    for( int i = propertiesList.topLevelItemCount()-1; i>=0; i--){
        QTreeWidgetItem* item = propertiesList.topLevelItem(i);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>( propertiesList.itemWidget(item, Ui::PropertyAuto) );
        CameraManager::CameraProperty * prop = reinterpret_cast<CameraManager::CameraProperty*>( checkBox->property("CameraProperty").value<quintptr>() );
        QSlider* slider = reinterpret_cast<QSlider*>( checkBox->property("TreeWidgetSlider").value<quintptr>() );
        //qDebug() << "updating:" << prop->getName().c_str();

        selected->updateProperty(prop);
        item->setText(Ui::PropertyValue, prop->formatValue() );
        checkBox->setChecked(prop->getAuto());

        if( prop->getType() == CameraManager::AUTOTRIGGER ) continue;
        slider->setValue(prop->getValueToSlider());
        slider->setEnabled(!prop->getAuto());

    }
}

void AbstractCameraManager::activateLiveView(bool active){
    liveView = active;
    if( active ){
        for(int i=activeCameras.size()-1; i>=0; i--){
            activeCameraEntry& camEntry = activeCameras.at(i);
            QVideoWidget* videoWidget = qobject_cast<QVideoWidget *>( camEntry.window->widget() );
            camEntry.camera->startCapture(videoWidget);
        }
    }else{
        for(int i=activeCameras.size()-1; i>=0; i--){
            activeCameraEntry& camEntry = activeCameras.at(i);
            camEntry.camera->stopAutoCapture();
        }
    }
}

///////////////////////////////////////////////////
/////////////// Camera List related ///////////////
///////////////////////////////////////////////////

QModelIndex AbstractCameraManager::detectNewCamerasAndExpand(){
    std::vector<AbstractCamera*> newCameras;
    std::vector<QStandardItem*> oldCameras;
    cameraTree_getCameraList(cameraTree.invisibleRootItem(), &oldCameras);
    detectNewCameras(&newCameras);
    /*cout << "oldCameras" << oldCameras.size() << endl;*/
    /*cout << "newCameras" << newCameras.size() << endl;*/
    //removing disconnected cameras
    for(unsigned int i=0; i<oldCameras.size(); i++){
        QStandardItem* item = oldCameras.at(i);
        AbstractCamera* cam = reinterpret_cast<AbstractCamera *>( item->data(CameraRole).value<quintptr>() );
        /*cout << "oldCameras(" << i << "):" << cam << endl;*/
        bool found = false;
        for(int j=newCameras.size()-1; j>=0; j--){
            if(cam->equalsTo(newCameras.at(j))){
                found = true;
                newCameras.erase(newCameras.begin()+j);
                continue;
            }
        }
        if( !found ){ //remove if disconnected
            activateCamera(cam, item, false);
            item->parent()->removeRow(item->row());
        }
    }
    /*cout << "newCameras" << newCameras.size()<< endl;*/
    //adding new cameras
    for(unsigned int i=0; i<newCameras.size(); i++){
        AbstractCamera* cam = newCameras.at(i);
        QStandardItem *item = new QStandardItem(cam->getString().c_str());
        item->setData(QVariant::fromValue( reinterpret_cast<quintptr>(cam) ), CameraRole);
        //qDebug() << "setData " << camera << " data " << item->data(CameraRole).value<AbstractCamera *>();
        item->setCheckable(true);
        item->setCheckState(Qt::Unchecked);
        item->setDropEnabled(false);
        newCameraList.appendRow(item);
    }

    return newCameraList.index();
}


QModelIndex AbstractCameraManager::addGroup(){
    QStandardItem *newGroup = new QStandardItem("new Group");
    newGroup->setCheckable(true);
    newGroup->setIcon(folderIcon);
    if( selectedItem != NULL && !selectedItem->data(CameraRole).isValid() )
        selectedItem->appendRow(newGroup);
    else
        cameraTree.appendRow(newGroup);
    return newGroup->index();
}

void AbstractCameraManager::removeGroup(QModelIndex index){
    QStandardItem * item = getModel()->itemFromIndex( index );
    if( !item->isEditable() ) return;

    QStandardItem * parent = item->parent();
    if( parent == NULL ) parent = item->model()->invisibleRootItem();

    for(int i=item->rowCount(); i>0; i--){
        newCameraList.insertRow(0, item->takeRow(0));
    }

    parent->removeRow( item->row() );
    detectNewCamerasAndExpand();
}

void AbstractCameraManager::resetItem(QModelIndex index){
    QStandardItem * item = getModel()->itemFromIndex( index );
    if( item == NULL || !item->data(CameraRole).isValid() ) return;
    AbstractCamera* cam = reinterpret_cast<AbstractCamera *>( item->data(CameraRole).value<quintptr>() );
    item->setText( cam->getString().c_str() );
}


void AbstractCameraManager::activateCamera(AbstractCamera* camera, QStandardItem* item, bool active){
    cout << "activateCamera( " << camera << ", " << active << ")" << endl;
    int i = activeCameras.size()-1;
    while(i>=0 && activeCameras.at(i).camera != camera) --i;

    if(i >= 0){ //trouve
        if(!active){ // desactivation
            //qDebug() << "desactivating Camera";
            activeCameraEntry* entry = &activeCameras.at(i);
            entry->camera->stopAutoCapture();
            mainWindow->modifySubWindow(entry->window, false);
            activeCameras.erase(activeCameras.begin()+i);
        }else{
            activeCameras.at(i).window->setWindowTitle(item->text());
        }
    }else{
        if(active){// activation
            //qDebug() << "activating Camera";
            activeCameraEntry entry = activeCameraEntry(camera, item);
            connect(entry.window, SIGNAL(destroyed(QObject*)),
                    this, SLOT(on_subwindow_closing(QObject*)) );
            entry.window->setWindowTitle(item->text());
            mainWindow->modifySubWindow(entry.window, true);
            activeCameras.push_back(entry);
            QObject::connect(mainWindow, SIGNAL(activateCrosshair(bool)),
                             qobject_cast<QVideoWidget*>(entry.window->widget()), SLOT(activateCrosshair(bool)) );
            if( liveView )
                entry.camera->startCapture( qobject_cast<QVideoWidget *>( entry.window->widget() ) );
        }
    }
}

void AbstractCameraManager::on_subwindow_closing(QObject *window){
    cout << "closing " << window << endl;
    int i = activeCameras.size()-1;
    while(i>=0 && activeCameras.at(i).window != window) --i;

    if(i>=0) activeCameras.at(i).treeItem->setCheckState(Qt::Unchecked);
}
void AbstractCameraManager::on_CameraTree_itemChanged(QStandardItem* item){
    Qt::CheckState checked = item->checkState();
    if( item->data(CameraRole).isValid() ){
        activateCamera( reinterpret_cast<AbstractCamera*>( item->data(CameraRole).value<quintptr>() ), item, checked==Qt::Checked);
    } else {
        cameraTree_recursiveCheck(item, checked);
    }

}


void AbstractCameraManager::cameraTree_itemClicked(const QModelIndex & index, QString &string, int &icon, bool &editable, bool &deleteable){
    QStandardItem* clicked = getModel()->itemFromIndex(index);
    selectedItem = clicked;
    QStandardItem* first = NULL;


    string = clicked->text();
    editable = true;
    deleteable = true;
    if( clicked->data(CameraRole).isValid() ){
        icon = 0;
        deleteable = false;
        first = clicked;
    }else{
        if( clicked == &newCameraList ){
            cout << "clicked == newCameraList" << endl;
            editable = false;
            deleteable = false;
        }
        first = cameraTree_recursiveFirstCamera(clicked);
        if( first == NULL )
            icon = 1;
        else{
            icon = 2;
            string = clicked->text() + " <br /> ("+ first->text() + ")";
        }
    }
    selectedCamera = (first == NULL) ? NULL : reinterpret_cast<AbstractCamera *>( first->data(CameraRole).value<quintptr>() );
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
         if(currItem->checkState() != checked)
             currItem->setCheckState(checked);
         else
             cameraTree_recursiveCheck(currItem, checked);
    }
}


// get first AbstractCamera in QStandardItem
// @return first found AbstractCamera
QStandardItem* AbstractCameraManager::cameraTree_recursiveFirstCamera(QStandardItem* parent){
    QVariant data = parent->data(CameraRole);
    if(data.isValid())
        return parent;

    for(int i=0; i<parent->rowCount(); ++i){
        QStandardItem* tmp = cameraTree_recursiveFirstCamera(parent->child(i));
        if( tmp != NULL ) return tmp;
    }
    return NULL;
}

// set a property for all the AbstractCamera in QStandardItem and its decendants
void AbstractCameraManager::cameraTree_recursiveSetProperty(QStandardItem* parent, CameraManager::CameraProperty* prop){
    QVariant data = parent->data(CameraRole);
    if(data.isValid()){
        reinterpret_cast<AbstractCamera *>( data.value<quintptr>() )->setProperty(prop);
        return;
    }

    for(int i=0; i<parent->rowCount(); ++i){
        cameraTree_recursiveSetProperty(parent->child(i), prop);
    }
}

//add to the vector all found AbstractCamera in QStandardItem and its decendants
void AbstractCameraManager::cameraTree_getCameraList(QStandardItem* parent, std::vector<QStandardItem*> *list){
    QVariant data = parent->data(CameraRole);
    if(data.isValid()){
        list->push_back( parent );
        return;
    }

    for(int i=0; i<parent->rowCount(); ++i){
        cameraTree_getCameraList(parent->child(i), list);
    }
}


///////////////////////////////////////////////////
//////////////// Properties related ///////////////
///////////////////////////////////////////////////

void AbstractCameraManager::setProperties(std::vector<CameraProperty> &properties){
    cameraProperties = std::vector<CameraProperty>(properties);
    for(unsigned int i=0; i<cameraProperties.size(); i++){
        CameraProperty &property = cameraProperties.at(i);
        //qDebug() << property.getName().c_str() << reinterpret_cast<quintptr>(&property);
        QTreeWidgetItem* it = new QTreeWidgetItem();
        it->setText( Ui::PropertyName, property.getName().c_str());
        propertiesList.addTopLevelItem(it);
        //checkbox
        QCheckBox* box = new QCheckBox();
        box->setProperty("CameraProperty", QVariant::fromValue(reinterpret_cast<quintptr>(&property)) );

        if(!property.getCanAuto()) box->setEnabled(false);
        propertiesList.setItemWidget(it, Ui::PropertyAuto, box);
        connect( box, SIGNAL(stateChanged(int)), this, SLOT(on_propertyCheckbox_changed(int)) );
        //slider
        if( property.getType() == CameraManager::AUTOTRIGGER ) continue;
        QSlider* slider = new QSlider(Qt::Horizontal);
        slider->setProperty("CameraProperty", QVariant::fromValue(reinterpret_cast<quintptr>(&property)) );
        slider->setProperty("TreeWidgetItem", QVariant::fromValue(reinterpret_cast<quintptr>(it)) );
        slider->setTracking(true); //might be wanted
        slider->setRange(property.getMinToSlider(), property.getMaxToSLider());
        propertiesList.setItemWidget(it, Ui::PropertySlider, slider);

        box->setProperty("TreeWidgetSlider", QVariant::fromValue(reinterpret_cast<quintptr>(slider)) );
        connect( slider, SIGNAL(valueChanged(int)), this, SLOT(on_propertySlider_changed(int)) );
    }
    propertiesList.resizeColumnToContents(0);
    propertiesList.resizeColumnToContents(1);
    propertiesList.resizeColumnToContents(2);
}

void AbstractCameraManager::on_propertyCheckbox_changed(int state){
    if( selectedItem == NULL ) return;
    CameraProperty* prop = reinterpret_cast<CameraProperty*>( sender()->property("CameraProperty").value<quintptr>() );
    cout << sender() << prop->getName().c_str()<< endl;
    prop->setAuto(state == Qt::Checked);
    cameraTree_recursiveSetProperty(selectedItem, prop);

    //(de)activate slider
    if( prop->getType() == CameraManager::AUTOTRIGGER ) return;
    QSlider* slider =  reinterpret_cast<QSlider*>( sender()->property("TreeWidgetSlider").value<quintptr>() );
    cout << "Qslider" << slider << endl;
    if(state != Qt::Checked){
        slider->setEnabled(true);
        slider->setValue(prop->getValueToSlider());
    } else {
        slider->setEnabled(false);
    }

}
void AbstractCameraManager::on_propertySlider_changed(int val){
    if( selectedItem == NULL ) return;
    CameraProperty* prop = reinterpret_cast<CameraProperty*>( sender()->property("CameraProperty").value<quintptr>() );
    prop->setValueFromSlider(val);
    cameraTree_recursiveSetProperty(selectedItem, prop);

    reinterpret_cast<QTreeWidgetItem*>( sender()->property("TreeWidgetItem").value<quintptr>() )->setText(Ui::PropertyValue, prop->formatValue() );
}


