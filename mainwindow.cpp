#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QLineEdit> // Include for QLineEdit

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

    // Add QLineEdit for text input
    QLineEdit *textInputField = new QLineEdit(this);
    textInputField->setGeometry(120, 10, 200, 30); // Adjust geometry as needed

    // Add QPushButton for submission
    QPushButton *submitButton = new QPushButton("Submit", this);
    submitButton->setGeometry(330, 10, 100, 30); // Adjust geometry as needed

    connect(submitButton, &QPushButton::clicked, [this, textInputField, sdlApp]() {
        sdlApp->submitText(textInputField->text().toStdString());
    });

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
