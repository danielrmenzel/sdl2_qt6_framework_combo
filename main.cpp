// main.cpp
#include <QApplication>
#include "mainwindow.h"
#include "SDLApp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);    // CreateQApplication

    SDLApp sdlApp; // Create SDLApp instance

    
    /* Connects signals from sender object to receiver object after signal emission
     * @param &sdlApp - pointer to sender object
     * @param &SDLApp::quitApplication - trigger signal
     * @param &app - receiver object (QApplication)
     * @param &QCoreApplication::quit - terminates event loop
    */
    QObject::connect(&sdlApp, &SDLApp::quitApplication, &app, &QCoreApplication::quit);

    MainWindow w(&sdlApp); // Pass  SDLApp instance to MainWindow constructor
    w.show();

    return app.exec();
}


