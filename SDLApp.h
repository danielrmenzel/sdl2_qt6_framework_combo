#ifndef SDLAPP_H
#define SDLAPP_H

#include <QObject>
#include <SDL.h>
#include <SDL_ttf.h>

class SDLApp : public QObject {
    Q_OBJECT
public:
    explicit SDLApp(QObject *parent = nullptr);
    ~SDLApp();

    bool init();
    void processEvents();
    void toggleLine();
    bool shouldQuit() const;
    void cleanUp();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool drawLineFlag;
    bool quit;

    // Button constants
    static constexpr int BUTTON_WIDTH = 200;
    static constexpr int BUTTON_HEIGHT = 50;
    int buttonX = 220;
    int buttonY = 200;

    // Text input handling
    bool textInputMode;
    std::string textInputBuffer;

    // Font and text color
    TTF_Font* font;
    SDL_Color textColor;

    // Button text
    std::string buttonText;

    // Helper functions
    void render();
    void renderText(const std::string& message, int x, int y, int fontSize);
    void renderButton(SDL_Renderer* renderer, const char* text, int x, int y);
    bool isMouseInsideButton(int mouseX, int mouseY, int buttonX, int buttonY);
    void handleButtonClick(SDL_Event& event);
    void handleTextInput(SDL_Event& event);
};

#endif // SDLAPP_H
