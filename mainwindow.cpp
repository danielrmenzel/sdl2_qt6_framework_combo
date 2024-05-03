// mainwindow.cpp
#include "mainwindow.h"
#include <QDebug>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include "./ui_mainwindow.h"

/*
 * Constructor for MainWindow
 * @param sdlApp - Pointer to SDLApp object to control SDL window
 * @param parent - Pointer to parent widget
 */
MainWindow::MainWindow(SDLApp *sdlApp, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), sdlApp(sdlApp) {
    ui->setupUi(this); // Setup UI components for this window

    // Main vertical layout for the central widget
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->centralwidget);

    // Toggle SDL Window Button
    QPushButton *toggleSDLWindowButton = new QPushButton("Open/Close SDL Window");
    toggleSDLWindowButton->setStyleSheet("QPushButton { background-color: blue; color: white; }");
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
    submitButton->setStyleSheet("QPushButton { background-color: blue; color: white; }");
    connect(submitButton, &QPushButton::clicked, [this, sdlApp]() {
        sdlApp->submitText(this->textInputField->text().toStdString());
    });

    mainLayout->addWidget(submitButton);

    // Existing SDL Timer for processing events
    sdlTimer = new QTimer(this);
    connect(sdlTimer, &QTimer::timeout, sdlApp, &SDLApp::processEvents);
    sdlTimer->start(16); // Run approximately every 60Hz (16ms)

    // New Timer for checking cursor location
    QTimer *cursorLocationTimer = new QTimer(this);
    connect(cursorLocationTimer, &QTimer::timeout, sdlApp, &SDLApp::checkCursorLocation);
    cursorLocationTimer->start(100); // Check cursor location every 100 milliseconds

    // Connection for SDLApp's textEntered signal
    bool isConnected = connect(sdlApp, &SDLApp::textEntered, this, &MainWindow::onTextEntered);
    if (isConnected) {
        qDebug() << "Successfully connected SDLApp textEntered signal to MainWindow slot.";
    } else {
        qDebug() << "Failed to connect SDLApp textEntered signal to MainWindow slot.";
    }


    connect(sdlApp, &SDLApp::sdlWindowClosed, this, &MainWindow::onSDLWindowClosed);



    // Adjust the window size as needed
    this->setGeometry(100, 100, 400, 300); // Adjusted for better UI layout visibility
}

void MainWindow::onSDLWindowClosed() {
    qDebug() << "SDL window has been closed. Qt application remains running.";
    // Additional logic to handle the state after SDL window closure
}


/*
 * Destructor for MainWindow
 */
MainWindow::~MainWindow() {
    delete ui;
}

/*
 * Handles click event of toggle button (open/close)
 */
void MainWindow::onToggleLineButtonClicked() {
    sdlApp->openOrToggleWindow();
}

/*
 * Updates main window's display with submitted text from SDL2 window
 * @param text - Text from SDL2 window
 */
void MainWindow::onTextEntered(const QString &text) {
    displayTextLabel->setText(text);
}

/*
 * Handles window close event, releases SDL resources
 */
void MainWindow::closeEvent(QCloseEvent *event) {
    // Stop the SDL timer
    sdlTimer->stop();

    // Stop the cursor location timer
    cursorLocationTimer->stop();

    sdlApp->cleanUp();
    QMainWindow::closeEvent(event);
}
