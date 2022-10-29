#include "MainWindow.h"
// #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

int main()
{
    OpenGLWindow::setOpenGLContextVersion(4, 3);
    OpenGLWindow::setOpenGLProfile(OpenGLWindow::OpenGLProfile::Core);
    OpenGLWindow::setSamples(16);
    MainWindow window(1400, 800);

    window.load("assets/model/bunny_texture/bunny.obj");
    //window.load("assets/model/bun_zipper.ply");

    window.exec();
    return 0;
}

