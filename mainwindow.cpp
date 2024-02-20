#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QTimer> // Include QTimer header

MainWindow::MainWindow(SDLApp *sdlApp, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sdlApp(sdlApp) // Store the SDLApp instance
{
    ui->setupUi(this);

    QPushButton *toggleLineButton = new QPushButton("Toggle Line", this);
    toggleLineButton->setGeometry(10, 10, 100, 30);
    connect(toggleLineButton, &QPushButton::clicked, this, &MainWindow::onToggleLineButtonClicked);

    // Setup a QTimer to periodically call SDLApp's processEvents method
    sdlTimer = new QTimer(this);
    connect(sdlTimer, &QTimer::timeout, sdlApp, &SDLApp::processEvents);
    sdlTimer->start(16); // Run approximately every 60Hz (16ms)
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onToggleLineButtonClicked() {
    sdlApp->toggleLine(); // Toggle the line in SDLApp
}

void MainWindow::closeEvent(QCloseEvent *event) {
    sdlApp->cleanUp(); // Ensure SDL cleanup is called
    QMainWindow::closeEvent(event); // Proceed with the usual close event
}
