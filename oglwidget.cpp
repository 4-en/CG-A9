#include "oglwidget.h"
#include <math.h>
#include "ui_mainwindow.h"
#include "QVector3D"
#include <iostream>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <universe.hpp>

// simulation loop
// runs in separate thread
void OGLWidget::runSim()
{
    constexpr unsigned int fps = 60;
    constexpr double dtime = 1.0 / fps;
    double dt = dtime;

    constexpr int micros = dtime * 1000 * 1000;
    constexpr auto waitTime = std::chrono::microseconds(micros);
    auto lastTime = std::chrono::high_resolution_clock::now();
    unsigned long long frame = 0;
    double time = 0.0;
    double timeScale = 60.0*60.0 * 24.0; // 1 day per second


    running = true;
    while (running)
    {
        lastTime = std::chrono::high_resolution_clock::now();
        // parama+=0.1;
        dt = dtime * paramb * timeScale;
        time = time + dt;
        //world->tick(lastTime.time_since_epoch().count());

        // world handles all logic (movement, collision, gravity...)
        world->tick(time);




        update();

        // sleep until next tick
        std::this_thread::sleep_until(lastTime + waitTime);
        frame++;
    }
}



// default OGLWidget functions

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    parama = 1;
    paramb = 20;
    paramc = 1;
    lightDirection = 0;
    world = new Universe();

}

void OGLWidget::startSim()
{
    std::thread thr([&]
                    { this->runSim(); });
    thr.detach();
}

OGLWidget::~OGLWidget()
{
    delete world;
}

void OGLWidget::setUi(Ui::MainWindow *ui)
{
    this->ui = ui;
}

void OGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //float light_diffuse_color[] = {0.1, 0.1, 0.1, 0};
    //glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse_color);
    //glEnable(GL_LIGHT1);


    // point light
    GLfloat lightPosition[] = {0.0,0.0,0.0, 1.0};  // Set the light position (x, y, z)
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);  // Set lighting to one side only

    GLfloat lightAmbient[] = {0.2, 0.2, 0.2, 1.0};   // Set ambient color (RGBA)
    GLfloat lightDiffuse[] = {1.0, 1.0, 1.0, 1.0};   // Set diffuse color (RGBA)
    GLfloat lightSpecular[] = {1.0, 1.0, 1.0, 1.0};  // Set specular color (RGBA)

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    GLfloat constantAttenuation = 1.0;   // Set constant attenuation factor
    GLfloat linearAttenuation = 0.09;    // Set linear attenuation factor
    GLfloat quadraticAttenuation = 0.032; // Set quadratic attenuation factor

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, constantAttenuation);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linearAttenuation);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadraticAttenuation);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    this->startSim();
}


void OGLWidget::setParamA(int newa)
{
    parama = newa / 10.0; // Set new value

    update(); // Trigger redraw of scene with paintGL
}

void OGLWidget::setParamB(int newb)
{
    paramb = newb * 1.0;
    update();
}

void OGLWidget::setParamC(int newc)
{
    paramc = newc / 10.0;
    update();
}

void OGLWidget::setLight(int newlight)
{
    lightDirection = newlight;
    update();
}

void OGLWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();




    glPushMatrix();

    // rotate with gravity
    //glRotatef(gravDirection, 0, 0, 1);
    QMatrix4x4 qViewMatrix;
    glRotatef(-45, 1, 0, 0);
    qViewMatrix.rotate(-45, 1, 0, 0);
    glRotatef(-135, 0, 1, 0);
    qViewMatrix.rotate(-135, 0, 1, 0);
    glScaled(0.1*parama, 0.1*parama, 0.1*parama);
    qViewMatrix.scale(0.1, 0.1, 0.1);

    float lightRot = parama * 36;
    glPushMatrix();
    //glRotatef(lightRot, 0, 0, 1);

    // light at 0/0/10
    //float light_position[] = {0.0, 0.0001, 0.0, 0};
    //glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    //glEnable(GL_LIGHT1);
    glPopMatrix();


    if(OGLWidget::showAxis) {
        
        // draw 3D axes and grid
        glNormal3f(3, 1, 1);
        glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(10, 0, 0);
        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 10, 0);
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 10);

        glEnd();
    }

    world->draw();

    glPushMatrix();


    glGetIntegerv(GL_VIEWPORT, viewport);

    GLfloat normalizedX = (2.0f * lastMousePos.x - viewport[0]) / viewport[2] - 1.0f;
    GLfloat normalizedY = 1.0f - (4.0f *lastMousePos.y - viewport[1]) / viewport[3];

    
    //GLfloat modelViewMatrix[16];
    GLfloat projectionMatrixData[16];

    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrixData);

    for(int i = 0; i < 16; i++) {
        projectionMatrix.data()[i] = projectionMatrixData[i];
    }



    // Get the current OpenGL model-view matrix
    GLfloat modelViewMatrixData[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrixData);
    for(int i = 0; i < 16; i++) {
        modelViewMatrix.data()[i] = modelViewMatrixData[i];
    }

    Vec3 worldMouse = screenToWorld(lastMousePos.x, lastMousePos.y);

    
    /*
    Sphere s;
    s.setRadius(0.2);
    s.setPosition(worldMouse);
    s.draw();
    */
    
    glPopMatrix();
    




    glPopMatrix();


    
}

Vec3 OGLWidget::screenToWorld(int x, int y) {


    GLfloat normalizedX = (2.0f * x - viewport[0]) / viewport[2] - 1.0f;
    GLfloat normalizedY = 1.0f - (4.0f *y - viewport[1]) / viewport[3];

    
    QMatrix4x4 projectionMatrixInverse = projectionMatrix.inverted();
    QVector4D rayClip(normalizedX, normalizedY, 1.0f, 1.0f);
    QVector4D rayView = projectionMatrixInverse * rayClip;
    
    double w = rayView.w();
    QVector3D rayView3D(rayView.x() / w, rayView.y() / w, rayView.z() / w);

    // Transform the ray to world space
    QMatrix4x4 modelViewMatrixInverse = modelViewMatrix.inverted();
    QVector4D rayWorld = modelViewMatrixInverse * QVector4D(rayView3D, 1.0f);

    return Vec3(rayWorld.x(), 0, rayWorld.z());


}

bool OGLWidget::showAxis = false;

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    woh = static_cast<double>(w) / static_cast<double>(h);
    glMatrixMode(GL_PROJECTION);
    /*glLoadIdentity();

    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();*/
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    // Upon mouse pressed, we store the current position...
    lastMousePos.x = event->x();
    lastMousePos.y = event->y();

}

void OGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    // something
    //std::cout << " Release " << std::endl;

}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    // ... and while moving, we calculate the dragging deltas
    // Left button: Rotating around x and y axis
    //int rx = (event->buttons() & Qt::LeftButton) ? lastMousePos.y - event->y() : 0;
    //int ry = lastMousePos.x - event->x();
    // Right button: Rotating around z and y axis
    //int rz = (event->buttons() & Qt::RightButton) ? lastMousePos.y - event->y() : 0;


    lastMousePos.x = event->x();
    lastMousePos.y = event->y();

    Vec3 worldPos = screenToWorld(event->x(), event->y());


}

void OGLWidget::keyPressEvent(QKeyEvent *event)
{

    switch(event->key())
    {
        // Up/Down: Rotating around x axis
        case Qt::Key_Up:
            break;

        // All other will be ignored
        default:
            break;
    }
}
