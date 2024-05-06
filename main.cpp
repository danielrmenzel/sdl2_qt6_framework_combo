// main.cpp
#define SDL_MAIN_HANDLED // Define this before including SDL headers

#include <QApplication>
#include "SDLApp.h"
#include "mainwindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // Hide the console window
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
#endif

    QApplication app(argc, argv); // CreateQApplication

    SDLApp sdlApp; // Create SDLApp instance

    /* Connects signals from sender object to receiver object after signal emission
     * @param &sdlApp - pointer to sender object
     * @param &SDLApp::quitApplication - trigger signal
     * @param &app - receiver object (QApplication)
     * @param &QCoreApplication::quit - terminates event loop
    */
    QObject::connect(&sdlApp, &SDLApp::quitApplication, &app, &QCoreApplication::quit);

    MainWindow w(&sdlApp); // Passes SDLApp instance to MainWindow constructor
    w.show();

    return app.exec();
}
