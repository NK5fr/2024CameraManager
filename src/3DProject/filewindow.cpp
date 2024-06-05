#include "filewindow.h"

FileWindow::FileWindow(QWidget *parent) : QWidget(parent){

    this->setGeometry(100,100,600,400);
}

void FileWindow::setViewMrHolt(){
    this->setWindowTitle("Rows = timesteps / Columns = markers");
    delete this->layout();
    QVBoxLayout *layout = new QVBoxLayout;
            QScrollArea *zoneDefilement = new QScrollArea;
            QLabel *label = new QLabel("test");
            setLayout(layout);
    QStringList split;
    int countZero=0;
    QTableWidget *table = new QTableWidget(data->getDataCoordinatesSize(), data->get1Vector(0).size());
    table->setEditTriggers(QAbstractItemView::EditTriggers(0));
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    zoneDefilement->setWidget(table);
    layout->addWidget(table);
    layout->addWidget(label);
    QTableWidgetItem *item;
    int i = 0;
    int l=0;
    while(i<data->getDataCoordinatesSize()) {
        l=0;
        for(int j = 0 ; j < data->get1Vector(0).size() ; j++) {
            item = new QTableWidgetItem(QString::number(data->get1Marker(i,j).getX())+" "+ QString::number(data->get1Marker(i,j).getY())
                                        + " " + QString::number(data->get1Marker(i,j).getZ()));
            split=item->text().split(" ");


            table->setItem(i, l, item);
            QString x=split.value(0);
            QString y=split.value(1);
            QString z=split.value(2);

            if(x.toDouble()==0&&y.toDouble()==0&&z.toDouble()==0){
                item->setBackground(QColor(255, 0, 0));
                countZero++;
            }
            l++;
        }
        i++;
    }
    label->setText(""+ QString::number(countZero));
    table->resizeColumnsToContents();
}

void FileWindow::setViewMrNilsen(){
    this->setWindowTitle("Rows = markers / Columns = timesteps");
    delete this->layout();
    QVBoxLayout *layout = new QVBoxLayout;
            QScrollArea *zoneDefilement = new QScrollArea;
            QLabel *label = new QLabel("ligne vide");
            setLayout(layout);
      int countZero=0;
      QTableWidget *table = new QTableWidget(data->get1Vector(0).size()*3,data->getDataCoordinatesSize());
      table->setEditTriggers(QAbstractItemView::EditTriggers(0));
      table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      zoneDefilement->setWidget(table);
      layout->addWidget(table);
      layout->addWidget(label);
;
      int i = 0;
      QTableWidgetItem *itemx;
      QTableWidgetItem* itemy;
      QTableWidgetItem* itemz;
      while(i<data->getDataCoordinatesSize()){
          for(int j = 0 ; j < data->get1Vector(0).size() ; j++) {

                  itemx=new QTableWidgetItem(QString::number(data->get1Marker(i,j).getX()));
                  itemy = new QTableWidgetItem(QString::number(data->get1Marker(i,j).getY()));
                  itemz = new QTableWidgetItem(QString::number(data->get1Marker(i,j).getZ()));
                  table->setItem((j*3), i, itemx);
                  table->setItem((j*3)+1, i, itemy);
                  table->setItem((j*3)+2, i, itemz);

              if(itemx->text().toDouble()==0&&itemy->text().toDouble()==0&&itemz->text().toDouble()==0){

                  itemx->setBackground(QColor(255,0,0));
                  itemy->setBackground(QColor(255,0,0));
                  itemz->setBackground(QColor(255, 0, 0));
                  countZero++;
              }
          }
          i++;
      }
      label->setText(""+ QString::number(countZero));
      table->resizeColumnsToContents();
}

void FileWindow::setData(const Data *pointerToData) {
    data = pointerToData;
}
