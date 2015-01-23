#include "widgetgl.h"
#include "sleeper.h"
#include <iostream>
#include <QtCore/qmath.h>
#include <QMenu>

using namespace std;

WidgetGL::WidgetGL(vector< vector < double > > *points, std::vector < float > minmax,
                   SocketViewerWidget *socket, QString calibrationPath)
    : pointsDatas(*points), minMax(minmax), initialScale(false), rotationAxis(' '),
      keyPressed(0), svw(socket){

    coordinatesShown = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;

    qtBlack = QColor::fromCmykF(0.0, 0.0, 0.0, 0.25);

    initializeDefaultRotation();
    initializingCameraCoordinates(calibrationPath);
    initializeGL();



    /* Initializing the selected array */
    for(int i=0;i<pointsDatas[0].size();i++)
        selected.insert(i, false);

    setMouseTracking(true);
    grabKeyboard();

    /*initializingCameraCoordinates(calibrationPath);*/

    installEventFilter(this);

    updateGL();
}


WidgetGL::~WidgetGL(){

}

void WidgetGL::initializeGL(){
 /*   qglClearColor(qtBlack.light());
    glMatrixMode(GL_MODELVIEW);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE);
    glDisable(GL_CULL_FACE);

    /* Scaling the painting area */
/*    glScalef(1/((minMax[3]-minMax[0])*1.5),
            1/((minMax[4]-minMax[1])*1.5),
            1/((minMax[5]-minMax[2])*1.5));*/
}

void WidgetGL::initializeDefaultRotation(){
    /* Initializing the cumulate rotation values */
    cumulateXRot=0;
    cumulateYRot=0;
    cumulateZRot=0;

    defaultRotation.resize(4);
    for(int i=0;i<defaultRotation.size();i++)
        defaultRotation[i].resize(3);
    /* First */
    defaultRotation[0][0]=351;
    defaultRotation[0][1]=0;
    defaultRotation[0][2]=9;
    /* Second */
    defaultRotation[1][0]=351;
    defaultRotation[1][1]=0;
    defaultRotation[1][2]=261;
    /* Third */
    defaultRotation[2][0]=351;
    defaultRotation[2][1]=93;
    defaultRotation[2][2]=306;
    /* Fourth */
    defaultRotation[3][0]=351;
    defaultRotation[3][1]=0;
    defaultRotation[3][2]=84;
}

void WidgetGL::initializingCameraCoordinates(QString calibrationPath){
    cout << "calibrationPath : " << calibrationPath.toUtf8().constData() << endl;
    QFile myFile(calibrationPath);
    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text)){ return; }
    /* Getting the combination */
    vector < QString > combinations;
    QString line = myFile.readLine();
    QStringList combinationsList = line.split(" ");
    combinations.resize(combinationsList.size()-1);
    for(int i=0;i<combinationsList.size()-1;i++)
        combinations[i]=combinationsList.at(i+1);

    camerasData.resize(combinations.size()*3);
    for(int i=0;i<camerasData.size();i++)
        camerasData.at(i).resize(3);

    /* while file is not finished */
    while(!myFile.atEnd()){
        line = myFile.readLine();
        /* Testing all the combinations from the vector 'combination' */
        for(int i=0;i<combinations.size();i++){
            /* If the line is the path line */
            if(line.contains("C:\\")){
                cout << "Line : " << line.mid(0,line.size()-1).toUtf8().constData()
                     << ", combination : " << combinations[i].toUtf8().constData() << endl;
                cout << "Line contains : " << line.contains(combinations[i]) << endl;
            }
            if(line.contains("C:\\") && line.contains(combinations[i])){
                cout << "Combination : " << combinations[i].toUtf8().constData() << endl;
                /* Loop for 3 cameras */
                for(int j=0;j<3;j++){
                    while(!line.contains("Camno") && !line.contains("Serial no."))
                        line = myFile.readLine();
                    /* Getting the camera number */
                    int number = line.split(" ").at(1).split(".").at(0).toInt();
                    /* Line down, to have the coordinates */
                    line = myFile.readLine();
                    /* Cleaning the list */
                    QStringList list = line.split(" ");
                    vector < QString > listCleaned;
                    listCleaned.resize(list.size());
                    QString tmp;
                    /* cout << "Cleaning" << endl;*/
                    int ind=0;
                    for(int k=0;k<list.size();k++){
                        tmp=list.at(k);
                        tmp.replace(" ","");
                        if(!tmp.isEmpty()){
                            listCleaned[ind]=tmp;
                            ind++;
                        }
                    }
                    /* X, Y and Z */
                    /*for(int k=0;k<listCleaned.size();k++)
                        cout << "Value : '" << listCleaned.at(k).toUtf8().constData() << "'" << endl;
                    cout << endl;*/
                    camerasData[number][0]=listCleaned.at(1).toDouble();
                    camerasData[number][1]=listCleaned.at(3).toDouble();
                    camerasData[number][2]=listCleaned.at(5).toDouble();
                }
            }

        }
    }
    for(int i=0;i<camerasData.size();i++){
        for(int j=0;j<camerasData[i].size();j++){
            cout << "Camera n°" << i << ", coordinate : " << j << ", value : " << camerasData[i][j] << endl;
        }
    }
}

void WidgetGL::paintGL(){
    /* This one should be usefull
                        * glTranslatef(1.5f, 0.0f, -7.0f);*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*    glPointSize(4.0);
    glLineWidth(3.0);

    /* Rotation
           * If the last force update was owed by a rotation
           */
/*    if(rotationAxis=='X')
        glRotatef(xRot, 1.0, 0.0, 0.0);
    if(rotationAxis=='Z')
        glRotatef(zRot, 0.0, 1.0, 0.0);
    if(rotationAxis=='Y')
        glRotatef(yRot, 0.0, 0.0, 1.0);


    /* Drawing points at the 'coordinatesShown' time */
/*    glColor3f(0, 0, 0);
    for(int i=0;i<(pointsDatas[coordinatesShown]).size();i=i+3){
        glBegin(GL_POINTS);
        glVertex3f(GLfloat(pointsDatas[coordinatesShown][i]),
                   GLfloat(pointsDatas[coordinatesShown][i+2]),
                GLfloat(pointsDatas[coordinatesShown][i+1]));
        glEnd();
    }
    /* Drawing the cameras point */
/*    glColor3f(0.9, 0, 0.9);
    glPointSize(8.0);
    for(int i=0;i<camerasData.size();i++){
        glBegin(GL_POINTS);
        glVertex3f(GLfloat(camerasData[i][0]),
                GLfloat(camerasData[i][2]),
                GLfloat(camerasData[i][1]));
        glEnd();
    }


    /* Drawing axis lines
           * X axis */

/*    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(1000, 0, 0);
    glEnd();

    /* Y Axis */
/*    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1000);
    glEnd();

    /* Z Axis */
/*    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1000, 0);
    glEnd();

    rotationAxis=' ';*/
}

void WidgetGL::resizeGL(int w, int h){
    glViewport(0, 0, w, h);
}


void WidgetGL::showView(int viewTime){
    coordinatesShown=viewTime;
    /* Update with no rotation */
    updateGL();
}


bool WidgetGL::eventFilter(QObject *obj, QEvent *event){
    if(event->type() == QEvent::KeyPress){
        keyPressed=((QKeyEvent *)event)->key();
        /* If Left Arrow and into range */
        if(keyPressed==Qt::Key_Left && coordinatesShown-1>=0 && coordinatesShown-1<pointsDatas.size()-1){
            int newValue = coordinatesShown-1;
            showView(newValue);
            svw->getTimeSlider()->setValue(newValue);
            return true;
        }
        /* If Right Arrow and into range */
        if(keyPressed==Qt::Key_Right && coordinatesShown+1>=0 && coordinatesShown+1<pointsDatas.size()-1){
            int newValue = coordinatesShown+1;
            showView(newValue);
            svw->getTimeSlider()->setValue(newValue);
            return true;
        }
        return true;
    }
    if(event->type() == QEvent::KeyRelease){
        keyPressed=0;
        return true;
    }
    if(event->type() == QEvent::Wheel){
        QWheelEvent *eventWheel = (QWheelEvent *)event;
        int num = (eventWheel->delta()/120)*-1;
        /* No key Pressed, then change view */
        if(keyPressed==0){
            if(coordinatesShown+num>=0 && coordinatesShown+num<pointsDatas.size()-1){
                int newValue = coordinatesShown+num;
                showView(newValue);
                svw->getTimeSlider()->setValue(newValue);
            }
            return true;
        }
        if(keyPressed==Qt::Key_X){
            setXRotation(num*3);
            return true;
        }
        if(keyPressed==Qt::Key_Y){
            setYRotation(num*3);
            return true;
        }
        if(keyPressed==Qt::Key_Z){
            setZRotation(num*3);
            return true;
        }
    }
    if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        if(mouseEvent->button()==Qt::RightButton){
            QMenu *menu = new QMenu();
            QString degree = QString::fromUtf8("\u00b0");
            for(int i=0;i<defaultRotation.size();i++)
                menu->addAction(QString("View point n"+degree+"%1").arg(i));

            menu->popup(cursor().pos());
            connect(menu, SIGNAL(triggered(QAction*)),
                    this, SLOT(clickOnMenu(QAction*)));
            return true;
        }
/*        if(mouseEvent->button()==Qt::LeftButton){
            cout << "Present !" << endl;
            GLdouble model_view[16];
            glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

            GLdouble projection[16];
            glGetDoublev(GL_PROJECTION_MATRIX, projection);

            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            cout << "Get model, proj and view" << endl;
            GLdouble *x, *y, *z;
            int res = gluProject(0.0, 0.0, 0.0, model_view, projection, viewport, x, y, z);
            cout << "Res : " << res << endl;
            cout << "x : " << *x
                 << ",y : " << *y
                 << ",z : " << *z << endl;
            cout << "Event : x : " << mouseEvent->x()
                 << ", y : " << mouseEvent->y() << endl;
        }*/
    }
    return false;
}
void WidgetGL::clickOnMenu(QAction *action){
    QString number = action->text().split(QString::fromUtf8("\u00b0")).at(1);
    int nb = number.toInt();
    setXRotation(360-cumulateXRot);
    setYRotation(360-cumulateYRot);
    setZRotation(360-cumulateZRot);

    initializeGL();

    setXRotation(defaultRotation[nb][0]);
    setYRotation(defaultRotation[nb][1]);
    setZRotation(defaultRotation[nb][2]);
}

static void qNormalizeAngle(int &angle){
    while (angle < 0)
        angle += 360;
    while (angle >= 360)
        angle -= 360;
}
void WidgetGL::setXRotation(int angle){
    qNormalizeAngle(angle);
    xRot = angle;
    cumulateXRot=cumulateXRot+angle;
    qNormalizeAngle(cumulateXRot);
    cout << "Cumulate X rot : " << cumulateXRot << endl;
    /* Update with rotation */
    rotationAxis='X';
    updateGL();
}

void WidgetGL::setYRotation(int angle){
    qNormalizeAngle(angle);
    yRot = angle;
    cumulateYRot=cumulateYRot+angle;
    qNormalizeAngle(cumulateYRot);
    cout << "Cumulate Y rot : " << cumulateYRot << endl;
    /* Update with rotation */
    rotationAxis='Y';
    updateGL();
}

void WidgetGL::setZRotation(int angle){
    qNormalizeAngle(angle);
    zRot = angle;
    cumulateZRot=cumulateZRot+angle;
    qNormalizeAngle(cumulateZRot);
    cout << "Cumulate Z rot : " << cumulateZRot << endl;
    /* Update with rotation */
    rotationAxis='Z';
    updateGL();
}
