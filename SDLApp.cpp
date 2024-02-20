// SDLApp.cpp

#include "SDLApp.h"
#include <QDebug>

SDLApp::SDLApp(QObject *parent) : QObject(parent), window(nullptr), renderer(nullptr), drawLineFlag(false), quit(false), textInputMode(false), textInputBuffer(""), font(nullptr) {
    if (!init()) {
        qDebug() << "Failed to initialize SDLApp.";
    }
}

SDLApp::~SDLApp() {
    cleanUp();
}

bool SDLApp::init() {
    qDebug() << "Initializing SDLApp...";
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        qDebug() << "SDL_Init Error:" << SDL_GetError();
        return false;
    }

    if (TTF_Init() == -1) {
        qDebug() << "TTF_Init Error:" << TTF_GetError();
        return false;
    }

    window = SDL_CreateWindow("Hello from SDL", 10, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        qDebug() << "SDL_CreateWindow Error:" << SDL_GetError();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        qDebug() << "SDL_CreateRenderer Error:" << SDL_GetError();
        return false;
    }

    font = TTF_OpenFont("/home/dan/old/assets/pacifico/Pacifico.ttf", 24);
    if (!font) {
        qDebug() << "Failed to load font:" << TTF_GetError();
        return false;
    }

    textColor = {255, 255, 255};

    qDebug() << "SDLApp initialized successfully.";
    return true;
}

void SDLApp::processEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            qDebug() << "QUIT button pushed.";
            quit = true;
            break;
        case SDL_TEXTINPUT:
            handleTextInput(e);
            break;
        case SDL_KEYDOWN:
            if (textInputMode && e.key.keysym.sym == SDLK_RETURN) {
                textInputMode = false;
                buttonText = "Enter Text";
                emit textEntered(QString::fromStdString(textInputBuffer));
                textInputBuffer.clear();
                render();
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            handleButtonClick(e);
            break;
        default:
            break;
        }
    }
}

void SDLApp::toggleLine() {
    qDebug() << "Toggle line called.";
    drawLineFlag = !drawLineFlag;
    render();
}

void SDLApp::renderText(const std::string& message, int x, int y, int fontSize) {
    if (!font) {
        qDebug() << "Font not initialized.";
        return;
    }

    if (message.empty()) {
        qDebug() << "Empty message provided.";
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), textColor);
    if (!surface) {
        qDebug() << "Failed to create text surface:" << TTF_GetError();
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        qDebug() << "Failed to create texture from surface:" << SDL_GetError();
        return;
    }

    int textWidth, textHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);

    SDL_Rect renderQuad = {x, y, textWidth, textHeight};
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
    SDL_DestroyTexture(texture);
}

void SDLApp::renderButton(SDL_Renderer* renderer, const char* text, int x, int y) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect buttonRect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    SDL_RenderFillRect(renderer, &buttonRect);

    renderText(text, x + 10, y + 10, 16);
}

bool SDLApp::isMouseInsideButton(int mouseX, int mouseY, int buttonX, int buttonY) {
    return (mouseX >= buttonX && mouseX <= buttonX + BUTTON_WIDTH && mouseY >= buttonY && mouseY <= buttonY + BUTTON_HEIGHT);
}

void SDLApp::handleButtonClick(SDL_Event& event) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (textInputMode) {
        textInputMode = false;
        buttonText = "Enter Text";
        emit textEntered(QString::fromStdString(textInputBuffer));
        textInputBuffer.clear();
        render();
    } else {
        if (isMouseInsideButton(mouseX, mouseY, buttonX, buttonY)) {
            textInputMode = true;
            SDL_StartTextInput();
            buttonText = "Submit entered text";
            qDebug() << "Button 1 clicked. Text input enabled.";
            render();
        } else if (isMouseInsideButton(mouseX, mouseY, buttonX + BUTTON_WIDTH + 20, buttonY)) {
            qDebug() << "Button 2 clicked. Closing window.";
            SDL_Event quitEvent;
            quitEvent.type = SDL_QUIT;
            SDL_PushEvent(&quitEvent);
        }
    }
}

void SDLApp::handleTextInput(SDL_Event& event) {
    if (textInputMode) {
        textInputBuffer += event.text.text;
        render();
    }
}

void SDLApp::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderButton(renderer, buttonText.c_str(), buttonX, buttonY);
    renderButton(renderer, "Close Window", buttonX + BUTTON_WIDTH + 20, buttonY);

    renderText(textInputBuffer, 50, 50, 24);

    SDL_RenderPresent(renderer);
}

bool SDLApp::shouldQuit() const {
    return quit;
}

void SDLApp::cleanUp() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    TTF_Quit();
    SDL_Quit();
    qDebug() << "SDLApp cleaned up and quit.";
}
