#ifndef SDLAPP_H
#define SDLAPP_H

#include <QObject>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>

class SDLApp : public QObject {
    Q_OBJECT
public:
    explicit SDLApp(QObject *parent = nullptr);
    ~SDLApp();

    bool init();
    bool initResources();
    bool initTextAndImages();
    void processEvents();
    void toggleSDLWindow();
    SDL_Texture* createTextTexture(const std::string& message, int& textWidth, int& textHeight);

    void submitText(const std::string &text);
    bool shouldQuit() const;
    void openOrToggleWindow();
    void closeWindow();
    void cleanUpSDLWindow();
    void cleanUp();

signals:
    void textEntered(const QString& text);
    void quitApplication();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* imageTexture;
    bool drawLineFlag;
    bool closeWindowBtnClickedSDL;
    std::string buttonText;

    // UI dimensions for buttons
    static constexpr int BUTTON_WIDTH = 200;
    static constexpr int BUTTON_HEIGHT = 50;
    int buttonX = 220;
    int buttonY = 200;

    bool textInputMode;
    std::string textInputBuffer;




    TTF_Font* font;
    SDL_Color textColor;

    std::string submittedText;

    // Rendering methods
    void render();
    void renderMessageFromMainwindow(const std::string& message, int x, int y, int fontSize);
    void renderButton(SDL_Renderer* renderer, const char* text, int x, int y);
    bool isMouseInsideButton(int mouseX, int mouseY, int buttonX, int buttonY, int buttonWidth, int buttonHeight);
    void handleButtonClick(SDL_Event& event);
    void handleTextInput(SDL_Event& event);
};

#endif // SDLAPP_H
