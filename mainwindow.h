#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SDLApp.h"
#include <QLabel> // Include QLabel
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void onTextEntered(const QString& text);

public:
    MainWindow(SDLApp *sdlApp, QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void onToggleLineButtonClicked();
    //void onTextEntered(const QString& text); // Slot to handle text submission

private:
    Ui::MainWindow *ui;
    SDLApp *sdlApp;
    QTimer *sdlTimer;
    QLabel *displayTextLabel; // Add QLabel member variable
    QLineEdit *textInputField; // Add this line for the text input field

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
