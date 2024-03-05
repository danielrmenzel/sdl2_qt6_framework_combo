// mainwindow.cpp
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QLineEdit>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame> // For QFrame
#include <QLabel> // For QLabel


MainWindow::MainWindow(SDLApp *sdlApp, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), sdlApp(sdlApp) {
    ui->setupUi(this);

    // Main vertical layout for the central widget
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->centralwidget);

    // Toggle SDL Window Button
    QPushButton *toggleSDLWindowButton = new QPushButton("Open/Close SDL Window");
    toggleSDLWindowButton->setStyleSheet("QPushButton { background-color: blue; color: white; }"); // Blue background, white text
    mainLayout->addWidget(toggleSDLWindowButton);
    connect(toggleSDLWindowButton, &QPushButton::clicked, this, &MainWindow::onToggleLineButtonClicked);

    // Frame to hold the label and display text with an outline
    QFrame *messageFrame = new QFrame();
    messageFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    messageFrame->setLineWidth(1);

    QHBoxLayout *messageLayout = new QHBoxLayout(messageFrame);
    QLabel *messageLabel = new QLabel("Message from SDL Window:");
    messageLabel->setStyleSheet("QLabel { color : grey; }");
    messageLayout->addWidget(messageLabel);

    displayTextLabel = new QLabel();
    displayTextLabel->setMinimumSize(200, 30);
    messageLayout->addWidget(displayTextLabel);
    mainLayout->addWidget(messageFrame);

    // New section: Horizontal layout for the new label and text input field
    QHBoxLayout *textInputLayout = new QHBoxLayout();
    QLabel *textInputLabel = new QLabel("Enter message for SDL2 Window:");
    textInputField = new QLineEdit();
    textInputField->setMinimumSize(150, 30);

    textInputLayout->addWidget(textInputLabel);
    textInputLayout->addWidget(textInputField);
    mainLayout->addLayout(textInputLayout);

    // Submit Button
    QPushButton *submitButton = new QPushButton("Submit");
    submitButton->setStyleSheet("QPushButton { background-color: blue; color: white; }"); // Blue background, white text

    mainLayout->addWidget(submitButton);
    connect(submitButton, &QPushButton::clicked, [this, sdlApp]() {
        sdlApp->submitText(this->textInputField->text().toStdString());
    });


    // SDL Timer setup
    sdlTimer = new QTimer(this);
    connect(sdlTimer, &QTimer::timeout, sdlApp, &SDLApp::processEvents);
    sdlTimer->start(16); // Run approximately every 60Hz (16ms)

    // Connection for SDLApp's textEntered signal
    bool isConnected = connect(sdlApp, &SDLApp::textEntered, this, &MainWindow::onTextEntered);
    if (isConnected) {
        qDebug() << "Successfully connected SDLApp textEntered signal to MainWindow slot.";
    } else {
        qDebug() << "Failed to connect SDLApp textEntered signal to MainWindow slot.";
    }

    // Adjust the window size as needed
    this->setGeometry(100, 100, 200, 100);
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
