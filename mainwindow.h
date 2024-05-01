#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define SDL_MAIN_HANDLED // Define this before including SDL headers

#include <QMainWindow>
#include <QLabel> // Include QLabel
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QCloseEvent>
#include "SDLApp.h" // Include your custom SDL application class

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(SDLApp *sdlApp, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onToggleLineButtonClicked();
    void onTextEntered(const QString &text);

private:
    Ui::MainWindow *ui;
    SDLApp *sdlApp;
    QTimer *sdlTimer;
    QTimer *cursorLocationTimer;
    QLabel *displayTextLabel;
    QLineEdit *textInputField;
};

#endif // MAINWINDOW_H
