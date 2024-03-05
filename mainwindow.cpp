#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QLineEdit>
#include <QDebug> // Include for qDebug()

MainWindow::MainWindow(SDLApp *sdlApp, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), sdlApp(sdlApp) {
    ui->setupUi(this);

    QPushButton *toggleSDLWindowButton = new QPushButton("Open/Toggle SDL Window", this);
    toggleSDLWindowButton->setGeometry(10, 10, 200, 30);
    connect(toggleSDLWindowButton, &QPushButton::clicked, this, &MainWindow::onToggleLineButtonClicked);

    displayTextLabel = new QLabel(this);
    displayTextLabel->setGeometry(120, 50, 400, 30);

    sdlTimer = new QTimer(this);
    connect(sdlTimer, &QTimer::timeout, sdlApp, &SDLApp::processEvents);
    sdlTimer->start(16); // Run approximately every 60Hz (16ms)

    QPushButton *closeSDLButton = new QPushButton("Close SDLApp", this);
    closeSDLButton->setGeometry(220, 10, 200, 30);
    connect(closeSDLButton, &QPushButton::clicked, sdlApp, &SDLApp::closeWindow);

    QLineEdit *textInputField = new QLineEdit(this);
    textInputField->setGeometry(120, 90, 200, 30);

    QPushButton *submitButton = new QPushButton("Submit", this);
    submitButton->setGeometry(330, 90, 100, 30);
    connect(submitButton, &QPushButton::clicked, [this, textInputField, sdlApp]() {
        sdlApp->submitText(textInputField->text().toStdString());
    });

    // Connecting SDLApp's textEntered signal to MainWindow's onTextEntered slot
    // and confirming the connection.
    bool isConnected = connect(sdlApp, &SDLApp::textEntered, this, &MainWindow::onTextEntered);
    if (isConnected) {
        qDebug() << "Successfully connected SDLApp textEntered signal to MainWindow slot.";
    } else {
        qDebug() << "Failed to connect SDLApp textEntered signal to MainWindow slot.";
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onToggleLineButtonClicked() {
    sdlApp->openOrToggleWindow();
}

void MainWindow::onTextEntered(const QString& text) {
    displayTextLabel->setText(text);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    sdlApp->cleanUp();
    QMainWindow::closeEvent(event);
}
