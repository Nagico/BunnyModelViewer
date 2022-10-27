#include "MainWindow.h"

int main()
{
    OpenGLWindow::setOpenGLContextVersion(4, 3);
    OpenGLWindow::setOpenGLProfile(OpenGLWindow::OpenGLProfile::Core);
    OpenGLWindow::setSamples(16);
    MainWindow window(800, 600);

    window.loadModel("assets/model/bunny_texture/bunny.obj");
    //window.loadModel("assets/model/bun_zipper.ply");

    window.exec();
    return 0;
}

