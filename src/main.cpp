#include "mainwindow.h"
#include <QtWidgets/qapplication.h>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    //app.setStyle("gtk+");
    //QFont serifFont("Times", 4);
    //app.setFont(serifFont);
    MainWindow window;

    window.show();
    return app.exec();
}
