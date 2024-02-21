#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(SDLApp *sdlApp, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), sdlApp(sdlApp)
{
    ui->setupUi(this);

    QPushButton *toggleLineButton = new QPushButton("Toggle Line", this);
    toggleLineButton->setGeometry(10, 10, 100, 30);
    connect(toggleLineButton, &QPushButton::clicked, this, &MainWindow::onToggleLineButtonClicked);

    // Initialize and setup displayTextLabel
    displayTextLabel = new QLabel(this);
    displayTextLabel->setGeometry(120, 50, 400, 30); // Adjust these values as needed for your layout

    connect(sdlApp, &SDLApp::textEntered, this, &MainWindow::onTextEntered);

    sdlTimer = new QTimer(this);
    connect(sdlTimer, &QTimer::timeout, sdlApp, &SDLApp::processEvents);
    sdlTimer->start(16); // Run approximately every 60Hz (16ms)
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onToggleLineButtonClicked() {
    sdlApp->toggleLine();
}

void MainWindow::onTextEntered(const QString& text) {
    displayTextLabel->setText(text);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    sdlApp->cleanUp();
    QMainWindow::closeEvent(event);
}
