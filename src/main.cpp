#include "mainwindow.h"
#include <QtWidgets/qapplication.h>

int main(int argc, char** argv) {
    #if defined(WIN32) || defined(_WIN32)
        qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");
    #endif
    QApplication app(argc, argv);
    //app.setStyle("gtk+");
    //QFont serifFont("Times", 4);
    //app.setFont(serifFont);
    MainWindow window;
    // qInfo() << QApplication::primaryScreen()->size();

    window.show();
    return app.exec();
}
