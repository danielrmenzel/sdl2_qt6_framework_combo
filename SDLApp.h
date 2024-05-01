#ifndef SDLAPP_H
#define SDLAPP_H

#define SDL_MAIN_HANDLED
#include <QObject>
#include <QMouseEvent> // Include this for QMouseEvent

#include <SDL.h>
#ifdef Q_OS_WIN
#include "C:/Users/danie/OneDrive/Desktop/bachelor/local_project_files/SDL2_image-2.8.2/include/SDL_image.h"
#include "C:/Users/danie/OneDrive/Desktop/bachelor/local_project_files/SDL2_ttf-2.22.0/include/SDL_ttf.h"
#else
#include <SDL_image.h>
#include <SDL_ttf.h>
#endif

#include <string>

class SDLApp : public QObject {
    Q_OBJECT

public:
    explicit SDLApp(QObject *parent = nullptr);
    ~SDLApp();

    bool init();
    void processEvents();
    bool checkCursorLocation();
    void bringWindowToFront();


    void openOrToggleWindow();
    void cleanUp();
    void cleanUpSDLWindow();

    bool initTextAndImages();
    SDL_Texture *createTextTexture(const std::string &message, int &textWidth, int &textHeight);

    void submitText(const std::string &text);
    void render();
    bool shouldQuit() const;
    bool isCloseWindowBtnClickedSDL() const;

signals:
    void textEntered(const QString &text);
    void windowMoved(int x, int y);
    void quitApplication();

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *imageTexture;

    bool closeWindowBtnClickedSDL;
    bool textInputMode;
    std::string textInputBuffer;

    TTF_Font *font;
    SDL_Color textColor;

    std::string submittedText;

    // UI dimensions and rendering methods
    static constexpr int BUTTON_WIDTH = 200;
    static constexpr int BUTTON_HEIGHT = 50;
    int buttonX = 220;
    int buttonY = 200;

    int windowX, windowY, windowWidth, windowHeight;

    void handleTextInput(SDL_Event &event);
    void renderMessageFromMainwindow(const std::string &message, int x, int y, int fontSize);
    void renderButton(SDL_Renderer *renderer, const char *text, int x, int y);
    bool isMouseInsideButton(int mouseX, int mouseY, int buttonX, int buttonY, int buttonWidth, int buttonHeight);
    void handleButtonClick(SDL_Event &event);
    void setTextInputMode(bool mode);

    // Cursor-related functions
    void renderTextInput();
    void renderCloseButton();
    void renderSquare();
};

#endif // SDLAPP_H
