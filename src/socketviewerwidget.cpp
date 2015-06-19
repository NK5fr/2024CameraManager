#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QTextStream>
#include <QtWidgets/QVBoxLayout>
#include <QFile>
#include <QString>
#include <QtWidgets/QLineEdit>
#include <QFont>
#include <QtOpenGL/QtOpenGL>
#include <QtNetwork/qtcpsocket.h>
#include <QtNetwork/qabstractsocket.h>
#include <QtNetwork/qhostinfo.h>
#include <QtNetwork/qnetworkinterface.h>
#include <QtNetwork/qnetworkconfiguration.h>
#include <QtNetwork/qnetworkconfigmanager.h>
#include <QtNetwork/qnetworksession.h>

#include <string.h>
#include <iostream>

#include "socketviewerwidget.h"

using namespace std;

/* Constructor */
SocketViewerWidget::SocketViewerWidget(QWidget* parent, QString path, QString filename, QString calibPath)
    : filename(filename), fullPath(path + "/" + filename), tmpPath(path), calibrationPath(calibPath), linesNumber(0), QMdiSubWindow(parent) {
    /* Creating QTextEdit, which need to be known to save file later if asked */
    fileContain = new QPlainTextEdit();
    fileContain->setReadOnly(true);
    fileContain->setContextMenuPolicy(Qt::CustomContextMenu);
    fileContain->setFont(QFont("Courier", 9));
    fileContain->setWindowTitle(filename);
    fileContain->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    const int tabStop = 16;  // 16 characters
    QFontMetrics metrics(fileContain->font());
    fileContain->setTabStopWidth(tabStop * metrics.width(' '));
    widgetGL = new WidgetGL(this, &pointData, calibrationPath);
    readTextFromFile();

    coordinatesShown = 0;
    rowNumber = 1;
    hideButtonPanel = true;

    // WidgetGL
    init();
    extractDataFromText();
    disconnectButton->hide();
    show3DView();
    setWindowTitle(filename);
}

SocketViewerWidget::SocketViewerWidget(QWidget* parent) : QMdiSubWindow(parent) {
    fileContain = new QPlainTextEdit();
    fileContain->setReadOnly(true);
    fileContain->setContextMenuPolicy(Qt::CustomContextMenu);
    fileContain->insertPlainText(fullText);
    //fileContain->setWindowTitle(filename);
    fileContain->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    coordinatesShown = 0;
    rowNumber = 1;
    hideButtonPanel = true;
    widgetGL = new WidgetGL(this);
    dialog = new HostAddressDialog(this, this);
    dialog->show();
    dialog->setFocus();
    init();
    show3DView();
    setWindowTitle("Live from TrackPoint-Server");
}

SocketViewerWidget::~SocketViewerWidget() {
}

void SocketViewerWidget::appendPoints(vector<Vector3d*> pos) {
    slider->setMaximum(rowNumber - 1);
    spinBox->setMaximum(rowNumber - 1);
    rowNumber++;
    widgetGL->appendPoints(pos);
    valueChanged(coordinatesShown);
    coordinatesShown += 1;
}

void SocketViewerWidget::readTextFromFile() {
    QFile myFile(fullPath);
    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    fullText = QString(myFile.readAll());
    myFile.close();
}

vector<Vector3d*> SocketViewerWidget::readLine(QString line) {
    QRegularExpression coordsRegEx("(-?\\d+(?:\\.?\\d*[eE]?\\-?\\d*)?)(?:\\s*)(-?\\d+(?:\\.?\\d*[eE]?\\-?\\d*)?)(?:\\s*)(-?\\d+(?:\\.?\\d*[eE]?\\-?\\d*)?)");
    QRegularExpressionMatchIterator i = coordsRegEx.globalMatch(line);
    vector<Vector3d*> vectors;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        Vector3d* v = new Vector3d();
        v->x = match.captured(1).toDouble();
        v->y = match.captured(3).toDouble();
        v->z = match.captured(2).toDouble();
        vectors.push_back(v);
    }
    return vectors;
}

void SocketViewerWidget::extractDataFromText() {
    QStringList lineList = fullText.split("\n", QString::SkipEmptyParts);
    linesNumber = lineList.size();

    int rowNumberLocal = 0;
    QTextCursor prev_cursor = fileContain->textCursor();
    for (int row = 0; row < linesNumber; row++) {
        if (lineList[row].isEmpty()) continue;
        vector<Vector3d*> points = readLine(lineList[row]);
        pointData.push_back(points);
        rowNumberLocal++;
        int grey = 200;
        //fileContain->setTextBackgroundColor(QColor(grey, grey, grey));
        //fileContain->inser
        //fileContain->insertPlainText(QString("%1  \t").arg((rowNumberLocal - 1), 6, 10, QChar(' ')));
        //fileContain->setTextBackgroundColor(QColor(255, 255, 255));
        fileContain->insertPlainText(QString("%1  \t").arg((rowNumberLocal - 1), 6, 10, QChar(' ')) + lineList[rowNumberLocal - 1] + "\n");
        slider->setMaximum(rowNumber - 1);
        spinBox->setMaximum(rowNumber - 1);
        rowNumber++;
        widgetGL->appendPoints(points);
    }
    fileContain->setTextCursor(prev_cursor);
}

void SocketViewerWidget::init() {
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    // Line Edit and Slider to choose time to show
    spinBox = new QSpinBox();
    spinBox->setMinimum(0);
    spinBox->setMaximum(rowNumber - 1);
    spinBox->setMinimumWidth(200);
    spinBox->setValue(coordinatesShown);

    slider = new QSlider(Qt::Horizontal);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(10);
    slider->setMinimum(0);
    slider->setMaximum(rowNumber - 1);
    //slider->setMaximumWidth(200);
    slider->setValue(coordinatesShown);

    QHBoxLayout* menuLayout = new QHBoxLayout();
    menuLayout->addWidget(slider);
    menuLayout->addWidget(spinBox);

    showPreceedingPoints = new QCheckBox("Show Preceeding Points");
    showPreceedingPoints->setCheckState((widgetGL->isShowPreceedingPoints()) ? Qt::Checked : Qt::Unchecked);
    showLines = new QCheckBox("Show Lines");
    showLines->setCheckState((widgetGL->isShowLines()) ? Qt::Checked : Qt::Unchecked);
    showCameras = new QCheckBox("Show Cameras");
    showCameras->setCheckState((widgetGL->isShowCameras()) ? Qt::Checked : Qt::Unchecked);
    showCameraLegs = new QCheckBox("Show Camera Tripod");
    showCameraLegs->setCheckState((widgetGL->isShowCameraLegs()) ? Qt::Checked : Qt::Unchecked);
    showCameraLegs->setEnabled(widgetGL->isShowCameras());
    showCameraFOV = new QCheckBox("Show Camera FOV");
    showCameraFOV->setCheckState((widgetGL->isShowFovCone()) ? Qt::Checked : Qt::Unchecked);
    showCameraFOV->setEnabled(widgetGL->isShowCameras());
    showCoordinateAxis = new QCheckBox("Show Coordinate Axis");
    showCoordinateAxis->setCheckState((widgetGL->isShowCoordinateSystem()) ? Qt::Checked : Qt::Unchecked);
    showFloor = new QCheckBox("Show Floor");
    showFloor->setCheckState((widgetGL->isShowFloorLines()) ? Qt::Checked : Qt::Unchecked);
    showOrtho = new QCheckBox("Show Orthographic Projection");
    showOrtho->setCheckState((widgetGL->isShowOrtho()) ? Qt::Checked : Qt::Unchecked);

    QVBoxLayout* checkBoxLayout1 = new QVBoxLayout();
    checkBoxLayout1->addWidget(showPreceedingPoints);
    checkBoxLayout1->addWidget(showLines);
    checkBoxLayout1->addWidget(showFloor);
    checkBoxLayout1->addWidget(showCoordinateAxis);
    checkBoxLayout1->setAlignment(Qt::AlignRight);
    //checkBoxLayout1

    QVBoxLayout* checkBoxLayout2 = new QVBoxLayout();
    checkBoxLayout2->addWidget(showCameras);
    checkBoxLayout2->addWidget(showCameraLegs);
    checkBoxLayout2->addWidget(showCameraFOV);
    checkBoxLayout2->addWidget(showOrtho);
    checkBoxLayout2->setAlignment(Qt::AlignRight);
    //checkBoxLayout2->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);

    showXYPlane = new QPushButton("Show XY-plane");
    showXZPlane = new QPushButton("Show XZ-plane");
    showYZPlane = new QPushButton("Show YZ-plane");
    showSocketText = new QPushButton("Show Socketfile");
    showSocketText->setMinimumHeight(50);
    show3DWidget = new QPushButton("Show 3D View");
    show3DWidget->setMinimumHeight(50);
    disconnectButton = new QPushButton("Disconnect Client");

    camDistanceSlider = new QSlider(Qt::Orientation::Horizontal);
    camDistanceSlider->setMaximum(20000);
    camDistanceSlider->setMinimum(200);
    camDistanceSlider->setSliderPosition((int) widgetGL->getCamDistance());
    camDistanceSlider->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
    fovConeSizeSlider = new QSlider(Qt::Orientation::Horizontal);
    fovConeSizeSlider->setEnabled(widgetGL->isShowFovCone() && widgetGL->isShowCameras());
    fovConeSizeSlider->setMaximum(8000);
    fovConeSizeSlider->setMinimum(200);
    fovConeSizeSlider->setSliderPosition((int) widgetGL->getFOVConeSize());
    //fovConeSizeSlider->setMaximumSize(QSize(400, 100));
    fovConeSizeSlider->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

    QGridLayout* sliderLayout = new QGridLayout();
    sliderLayout->addWidget(new QLabel("Camera Distance:"), 0, 0);
    sliderLayout->addWidget(new QLabel("FOV Camera-cone:"), 1, 0);
    sliderLayout->addWidget(camDistanceSlider, 0, 1);
    sliderLayout->addWidget(fovConeSizeSlider, 1, 1);

    QVBoxLayout* showPlaneButtonLayout = new QVBoxLayout();
    showPlaneButtonLayout->addWidget(showXYPlane);
    showPlaneButtonLayout->addWidget(showXZPlane);
    showPlaneButtonLayout->addWidget(showYZPlane);
    showPlaneButtonLayout->addLayout(sliderLayout);

    QHBoxLayout* buttonCheckBoxLayout = new QHBoxLayout();
    buttonCheckBoxLayout->addLayout(showPlaneButtonLayout);
    buttonCheckBoxLayout->addLayout(checkBoxLayout1);
    buttonCheckBoxLayout->addLayout(checkBoxLayout2);
    buttonCheckBoxLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* combinedLayout = new QVBoxLayout();
    //combinedLayout->addLayout(menuLayout);
    combinedLayout->addLayout(buttonCheckBoxLayout);
    //combinedLayout->addWidget(showSocketText);
    combinedLayout->setContentsMargins(QMargins(0, 0, 0, 0));

    buttonPanel = new QWidget();
    buttonPanel->setLayout(combinedLayout);
    buttonPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    buttonPanel->setContextMenuPolicy(Qt::CustomContextMenu);
    //buttonPanel->setContentsMargins(QMargins(0, 0, 0, 0));

    hideButton = new QPushButton("Hide/Show Button-Panel");

    // Main layout, and main widget
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(widgetGL);
    layout->addLayout(menuLayout);
    layout->addWidget(hideButton);
    layout->addWidget(buttonPanel);
    layout->addWidget(line);
    layout->addWidget(showSocketText);
    layout->addWidget(disconnectButton);

    hideButtonPanelFunc();

    widgetGLWindow = new QWidget();
    widgetGLWindow->setLayout(layout);


    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->addWidget(fileContain);
    textLayout->addWidget(show3DWidget);
    //textLayout->addWidget(disconnectButton);
    textWidget.setLayout(textLayout);

    connect(showPreceedingPoints, SIGNAL(stateChanged(int)), this, SLOT(showPreceedingPointsStateChanged(int)));
    connect(showLines, SIGNAL(stateChanged(int)), this, SLOT(showLinesStateChanged(int)));
    connect(showCameras, SIGNAL(stateChanged(int)), this, SLOT(showCamerasStateChanged(int)));
    connect(showCameraLegs, SIGNAL(stateChanged(int)), this, SLOT(showCameraLegsStateChanged(int)));
    connect(showCameraFOV, SIGNAL(stateChanged(int)), this, SLOT(showCameraFOVStateChanged(int)));
    connect(showCoordinateAxis, SIGNAL(stateChanged(int)), this, SLOT(showCoordinateAxisStateChanged(int)));
    connect(showFloor, SIGNAL(stateChanged(int)), this, SLOT(showFloorStateChanged(int)));
    connect(showOrtho, SIGNAL(stateChanged(int)), this, SLOT(showOrthoStateChanged(int)));
    connect(showSocketText, SIGNAL(clicked()), this, SLOT(showSocketTextFunc()));
    connect(show3DWidget, SIGNAL(clicked()), this, SLOT(show3DWidgetFunc()));
    connect(showXYPlane, SIGNAL(clicked()), this, SLOT(showXYPlaneFunc()));
    connect(showXZPlane, SIGNAL(clicked()), this, SLOT(showXZPlaneFunc()));
    connect(showYZPlane, SIGNAL(clicked()), this, SLOT(showYZPlaneFunc()));
    if (dialog != nullptr) connect(disconnectButton, SIGNAL(clicked()), dialog, SLOT(disconnectClient()));
    connect(hideButton, SIGNAL(clicked()), this, SLOT(hideButtonPanelFunc()));
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
    connect(camDistanceSlider, SIGNAL(valueChanged(int)), this, SLOT(camDistanceValueChanged(int)));
    connect(fovConeSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(fovConeSizeValueChanged(int)));
}

// Show 3D view of coordinates
void SocketViewerWidget::show3DView() {
    setFocusPolicy(Qt::TabFocus);
    // Initializing coordinatesShown, which is the int to know which time has to be displayed
    coordinatesShown = 0;
    setWidget(widgetGLWindow);
}

// Make all the sroll bars moving at the same time
// with the same value
void SocketViewerWidget::areaBarsMoved(int newValue) {
    timeArea->verticalScrollBar()->setValue(newValue);
    coordinatesArea->verticalScrollBar()->setValue(newValue);
    timeAreaBis->verticalScrollBar()->setValue(newValue);
}

// Display a tooltip the show the point number, the axis, the value and the time
void SocketViewerWidget::displayToolTip(CoordinatesLabel *label) {
    int layoutIndex = -1, indLayout = -1;
    while (layoutIndex == -1){
        indLayout++;
        layoutIndex = coordinatesLayout.at(indLayout)->indexOf(label);
    }
    int row, column, rowSpan, columnSpan;
    coordinatesLayout.at(indLayout)->getItemPosition(layoutIndex, &row, &column, &rowSpan, &columnSpan);
    QString axis;
    if (column % 3 == 0) axis = "x";
    else if (column % 3 == 1) axis = "y";
    else if (column % 3 == 2) axis = "z";
    // Creating the QString to de displayed
    QString degreeSign = QString::fromUtf8("\u00b0");
    QString pointNb = QString("%1").arg(indLayout);
    QString time = QString("%1").arg(row);
    QString string = QString("Point n" + degreeSign + pointNb + "\n"
                             + axis + " : " + label->text() + "\n" +
                             "t : " + time);
    QToolTip::showText(cursor().pos(), string, label);
}

// time changed into the 3D view
void SocketViewerWidget::valueChanged(int value) {
    coordinatesShown = value;
    slider->setValue(coordinatesShown);
    spinBox->setValue(coordinatesShown);
    widgetGL->showView(coordinatesShown);
}

QSlider *SocketViewerWidget::getTimeSlider() {
    return slider;
}

void SocketViewerWidget::showXYPlaneFunc() {
    widgetGL->showXYPlane();
}

void SocketViewerWidget::showXZPlaneFunc() {
    widgetGL->showXZPlane();
}

void SocketViewerWidget::showYZPlaneFunc() {
    widgetGL->showYZPlane();
}

void SocketViewerWidget::showSocketTextFunc() {
    setWidget(&textWidget);
    //showTextView();
}

void SocketViewerWidget::show3DWidgetFunc() {
    setWidget(widgetGLWindow);
    //showTextView();
}

void SocketViewerWidget::hideButtonPanelFunc() {
    if (hideButtonPanel) buttonPanel->hide();
    else buttonPanel->show();
    hideButtonPanel = !hideButtonPanel;
}

void SocketViewerWidget::showPreceedingPointsStateChanged(int state) {
    widgetGL->setShowPreceedingPoints(state == Qt::Checked);
    widgetGL->update();
}

void SocketViewerWidget::showLinesStateChanged(int state) {
    widgetGL->setShowLines(state == Qt::Checked);
    widgetGL->update();
}

void SocketViewerWidget::showCamerasStateChanged(int state) {
    widgetGL->setShowCameras(state == Qt::Checked);
    showCameraFOV->setEnabled(widgetGL->isShowCameras());
    showCameraLegs->setEnabled(widgetGL->isShowCameras());
    fovConeSizeSlider->setEnabled(widgetGL->isShowFovCone() && state == Qt::Checked);
    widgetGL->update();
}

void SocketViewerWidget::showCameraLegsStateChanged(int state) {
    widgetGL->setShowCameraLegs(state == Qt::Checked);
    widgetGL->update();
}

void SocketViewerWidget::showCameraFOVStateChanged(int state) {
    widgetGL->setShowFovCone(state == Qt::Checked);
    fovConeSizeSlider->setEnabled(state == Qt::Checked);
    widgetGL->update();
}

void SocketViewerWidget::showCoordinateAxisStateChanged(int state) {
    widgetGL->setShowCoordinateSystem(state == Qt::Checked);
    widgetGL->update();
}

void SocketViewerWidget::showFloorStateChanged(int state) {
    widgetGL->setShowFloorLines(state == Qt::Checked);
    widgetGL->update();
}

void SocketViewerWidget::showOrthoStateChanged(int state) {
    widgetGL->setShowOrtho(state == Qt::Checked);
    widgetGL->update();
}
void SocketViewerWidget::camDistanceValueChanged(int value) {
    widgetGL->setCamDistance((double) value);
    widgetGL->update();
}

void SocketViewerWidget::fovConeSizeValueChanged(int value) {
    widgetGL->setFOVConeSize((double) value);
    widgetGL->update();
}

void HostAddressDialog::onTextEdited(const QString& text) {
    QObject* s = sender();
    if (s == hostCombo) {
        QComboBox* comboBox = (QComboBox*) s;
        comboBox->setItemText(comboBox->currentIndex(), text);
    } else if (s == portLineEdit) {
        QLineEdit* lineEdit = (QLineEdit*) s;
        lineEdit->setText(text);
    }
}

void HostAddressDialog::connectToServer() {
    tcpSocket->abort();
    tcpSocket->connectToHost(hostCombo->currentText(), portLineEdit->text().toInt());
}

void HostAddressDialog::disconnectClient() {
    tcpSocket->close();
}

HostAddressDialog::HostAddressDialog(QWidget* parent, SocketViewerWidget* svw) : QDialog(parent), socketWidget(svw) {
    QLabel* hostLabel = new QLabel("Server name:");
    QLabel* portLabel = new QLabel("Server port:");
    hostCombo = new QComboBox;
    hostCombo->setEditable(true);
    
    hostCombo->addItem(QString("127.0.0.1"));
    // find out IP addresses of this machine
    //QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    portLineEdit = new QLineEdit("1250");
    //portLineEdit->setEchoMode(QLineEdit::Normal);
    //portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    hostLabel->setBuddy(hostCombo);
    //portLabel->setBuddy(portLineEdit);

    QPushButton* quitButton = new QPushButton("Quit");
    quitButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QPushButton* startButton = new QPushButton("Connect");
    startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QDialogButtonBox* buttonBox = new QDialogButtonBox;
    buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    buttonBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    tcpSocket = new QTcpSocket(this);

    //connect(hostCombo, SIGNAL(editTextChanged(QString)), this, SLOT(onTextEdited(QString)));
    //connect(portLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextEdited(QString)));
    connect(startButton, SIGNAL(clicked()), this, SLOT(connectToServer()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readSocketLine()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(hostLabel,    0, 0);
    mainLayout->addWidget(hostCombo,    0, 1);
    mainLayout->addWidget(portLabel,    1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(buttonBox,    2, 0, 1, 2);
    setLayout(mainLayout);
}

void HostAddressDialog::stopClient() {
    close();
}

void HostAddressDialog::readSocketLine() {
    QTextStream in(tcpSocket);
    //qDebug() << "Reading (bytes): " << tcpSocket->bytesAvailable();
    QByteArray buffer;
    while (tcpSocket->bytesAvailable() > 0) {
        buffer += tcpSocket->readAll();
    }
    //qDebug() << "Data:\n" << QString::fromLocal8Bit(buffer) << "\n";

    QString socketLine = QString::fromLocal8Bit(buffer);
    QStringList list = socketLine.split("\n", QString::SkipEmptyParts);
    for (int i = 0; i < list.size(); i++) {
        vector<Vector3d*> pos = socketWidget->readLine(list[i]);
        if (pos.size() > 0) {
            //list[i].remove("\n");
            socketWidget->getFileContain()->insertPlainText(list[i]);
            socketWidget->appendPoints(pos);

        } else {
            socketWidget->getFileContain()->insertPlainText("ERROR: \"" + list[i] + "\"\n");
        }
    }
}

void HostAddressDialog::displayError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            QMessageBox::information(this, "Camera Manager Client",
                                      "Remote Host Closed.");
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, "Camera Manager Client",
                                     "The host was not found. Please check the "
                                     "host name and port settings.");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, "Camera Manager Client",
                                     "The connection was refused by the peer. "
                                     "Make sure the TrackPoint server is running, "
                                     "and check that the host name and port "
                                     "settings are correct.");
            break;
        default:
            QMessageBox::information(this, "Camera Manager Client", QString("The following error occurred: %1.").arg(tcpSocket->errorString()));
    }
}

void HostAddressDialog::socketConnected() {
    close();
    //coordinatesShown = 0;
}
