#include <QApplication>
#include "mainwindow.h"
#include "SDLApp.h" // Include SDLApp header

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SDLApp sdlApp; // Create the SDLApp instance
    QObject::connect(&sdlApp, &SDLApp::quitApplication, &app, &QCoreApplication::quit);

    MainWindow w(&sdlApp); // Pass the SDLApp instance to the MainWindow constructor
    w.show();

    return app.exec();
}
