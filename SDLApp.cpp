// SDLApp.cpp
#include "SDLApp.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <filesystem>
#include <iostream>
#include <unistd.h>



#define SDL_MAIN_HANDLED
static constexpr int SQUARE_WIDTH = 540;
static constexpr int SQUARE_HEIGHT = 50;
int squareX = 50;
int squareY = 140;

bool sdlWindowActive = false;

/*
 * Constructor for SDLApp
 * Initializes SDL and sets up member variables
 * @param parent - Parent QObject pointer
*/
SDLApp::SDLApp(QObject *parent)
    : QObject(parent), window(nullptr), renderer(nullptr), closeWindowBtnClickedSDL(false),
    textInputMode(false), textInputBuffer(""), font(nullptr) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        qDebug() << "SDL_Init Error:" << SDL_GetError();
        return;
    }
}

/*
 * Destructor for SDLApp
 * Cleans up SDL resources
*/
SDLApp::~SDLApp() {
    cleanUp();
}

/*
 * Initializes SDLApp
 * @return bool - True if initialization succeeds, false otherwise
*/bool SDLApp::init() {
    qDebug() << "Initializing SDLApp...";
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        qDebug() << "SDL_Init Error:" << SDL_GetError();
        return false;
    }

    // Ensure TTF is initialized every time we intend to use it
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
    //render();
    return initTextAndImages();
}

/*
 * Cleans up SDL resources specific to the window
*/void SDLApp::cleanUpSDLWindow() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO); // Properly shut down the SDL video subsystem
    qDebug() << "SDL resources have been released.";
}

/*
 * Cleans up all SDL resources
*/void SDLApp::cleanUp() {
    cleanUpSDLWindow(); // Ensure window-specific resources are cleaned first

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    // Only call TTF_Quit() when you're completely done with all font operations in the application
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    qDebug() << "SDLApp cleaned up and quit.";
}

/*
 * Opens or toggles the visibility of the SDL window
*/void SDLApp::openOrToggleWindow() {
    if (window == nullptr) {
        if (!init()) {
            qDebug() << "Failed to initialize SDLApp.";
        }
        render();
    } else {
        //render();
        cleanUpSDLWindow();
    }
}

/*
 * Checks the current position of the cursor relative to the SDL window
 * @return bool - True if the cursor is inside the SDL window, false otherwise
*/
bool SDLApp::checkCursorLocation() {
    int mouseX, mouseY;
    SDL_GetGlobalMouseState(&mouseX, &mouseY); // Get the cursor position relative to the screen

    int winX, winY, winWidth, winHeight;
    SDL_GetWindowPosition(window, &winX, &winY);
    SDL_GetWindowSize(window, &winWidth, &winHeight);

    bool cursorIsInside = (mouseX >= winX && mouseX <= winX + winWidth) &&
                          (mouseY >= winY && mouseY <= winY + winHeight);

    static bool lastCursorInside = false;
    if (cursorIsInside != lastCursorInside) {
        if (cursorIsInside) {
            qDebug() << "Cursor has entered the SDL window.";
            bringWindowToFront(); // Custom function to manipulate window focus
        } else {
            qDebug() << "Cursor has left the SDL window.";
        }
        lastCursorInside = cursorIsInside;
    }
    return cursorIsInside;
}


#ifdef __APPLE__
void SDLApp::bringWindowToFront() {
    SDL_RaiseWindow(window);
}
#elif defined(_WIN32)
#include "SDL_syswm.h"
#include <windows.h> // Include for SetForegroundWindow on Windows
void SDLApp::bringWindowToFront() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (SDL_GetWindowWMInfo(window, &wmInfo)) {
        HWND hwnd = wmInfo.info.win.window;
        SetForegroundWindow(hwnd);
    }
}
#else // Assuming Linux or other platforms
#include <X11/Xlib.h>
#include <SDL2/SDL_syswm.h>

void SDLApp::bringWindowToFront() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);  // Initialize wmInfo to the version SDL compiles against
    if (SDL_GetWindowWMInfo(window, &wmInfo)) {
        Display* display = wmInfo.info.x11.display;
        Window win = wmInfo.info.x11.window;

        // This raises the window and attempts to focus it
        XRaiseWindow(display, win);
        XSetInputFocus(display, win, RevertToParent, CurrentTime);
    }
}
#endif

/*
 * Initializes textures and fonts required for rendering text and images
 * @return bool - True if initialization succeeds, false otherwise
*/
bool SDLApp::initTextAndImages() {
    std::string fullPath(__FILE__);
    std::filesystem::path pathObj(fullPath);
    std::filesystem::path dirPath = pathObj.parent_path();

    std::string imagePath = dirPath.string() + "/assets/images/banner.jpg";
    std::string fontPath = dirPath.string() + "/assets/fonts/alagard.ttf";

    imageTexture = IMG_LoadTexture(renderer, imagePath.c_str());
    if (!imageTexture) {
        qDebug() << "Failed to load image:" << IMG_GetError();
        return false;
    }

    font = TTF_OpenFont(fontPath.c_str(), 24);
    if (!font) {
        qDebug() << "Failed to load font:" << TTF_GetError();
        return false;
    }
    textColor = {64, 64, 64, 255};

    return true;
}

/*
 * Handles the quit event by cleaning up the SDL window and associated resources
 * and emitting a signal indicating that the SDL window has closed
*/void SDLApp::handleQuit() {
    cleanUpSDLWindow(); // Cleanup the SDL window and associated resources
    emit sdlWindowClosed(); // Signal that the SDL window has closed
    qDebug() << "SDL resources cleaned up and SDL_Quit() called.";
}

/*
 * Processes SDL events such as user input and window events
*/void SDLApp::processEvents()
{
    SDL_Event e;
    bool running = true;

    #ifndef __linux__
    while (running) {

    #endif
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:

                handleQuit();

                break;
            case SDL_TEXTINPUT:
                qDebug() << "Character typed in SDL2 window: " << e.text.text;
                handleTextInput(e);
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_BACKSPACE && !textInputBuffer.empty()) {
                    textInputBuffer.pop_back();
                    render();
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    if (!textInputBuffer.empty()) {
                        qDebug() << "Enter pressed, submitting text: "
                                 << QString::fromStdString(textInputBuffer);
                        emit textEntered(QString::fromStdString(textInputBuffer));
                        textInputBuffer.clear();
                        render();
                    }
                    textInputMode = false;
                    SDL_StopTextInput();
                }
                break;
            case SDL_MOUSEMOTION:
                checkCursorLocation();
                break;
            case SDL_WINDOWEVENT:
                checkCursorLocation();
                break;
            case SDL_MOUSEBUTTONDOWN:
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (mouseX >= squareX && mouseX <= squareX + SQUARE_WIDTH && mouseY >= squareY
                    && mouseY <= squareY + SQUARE_HEIGHT) {
                    qDebug() << "Mouse click in designated text input area detected.";
                    textInputMode = true;
                    render();
                    SDL_StartTextInput();
                } else {
                    handleButtonClick(e);
                }
                break;
            default:
                break;
            }
        }

        // Adding a small delay can reduce CPU usage
        SDL_Delay(10); // Delay in milliseconds to reduce CPU usage
#ifndef __linux__
         }

#endif
}



/*
 * Handles button clicks
 * @param event - SDL Event
*/
void SDLApp::handleButtonClick(SDL_Event &event)
{
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Calculate button positions based on input field dimensions
    int inputFieldX = 50;
    int inputFieldY = 140;
    int inputFieldWidth = 540;
    int inputFieldHeight = 50;
    int buttonX1 = inputFieldX + inputFieldWidth / 4 - BUTTON_WIDTH / 2;
    int buttonX2 = inputFieldX + inputFieldWidth * 3 / 4 - BUTTON_WIDTH / 2;

    // Button 1 area check
    if (isMouseInsideButton(mouseX, mouseY, buttonX1, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
        if (textInputMode) {
            textInputMode = false;
            SDL_StopTextInput();
            emit textEntered(QString::fromStdString(textInputBuffer));
            textInputBuffer.clear();
            render();
        } else {
            qDebug() << "No text entered.";
        }
    } else if (isMouseInsideButton(mouseX, mouseY, buttonX2, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
        qDebug() << "'Close Window' Button clicked in SDL2 Window.";
        closeWindowBtnClickedSDL = true;
        if (closeWindowBtnClickedSDL) {
            cleanUpSDLWindow();
        }
    }
}

/*
 * Checks if the mouse cursor is inside a button
 * @param mouseX - The x-coordinate of the mouse cursor
 * @param mouseY - The y-coordinate of the mouse cursor
 * @param buttonX - The x-coordinate of the top-left corner of the button
 * @param buttonY - The y-coordinate of the top-left corner of the button
 * @param buttonWidth - The width of the button
 * @param buttonHeight - The height of the button
 * @return bool - True if the mouse cursor is inside the button, false otherwise
*/
bool SDLApp::isMouseInsideButton(
    int mouseX, int mouseY, int buttonX, int buttonY, int buttonWidth, int buttonHeight)
{
    return (mouseX >= buttonX && mouseX <= buttonX + buttonWidth && mouseY >= buttonY
            && mouseY <= buttonY + buttonHeight);
}

/*
 * Handles text input events
 * @param event - The SDL event containing the text input
*/
void SDLApp::handleTextInput(SDL_Event &event) {
    if (textInputMode) {
        textInputBuffer += event.text.text;
        render();
    }
}

/*
 * Submits text entered by the user
 * @param text - The text entered by the user
*/
void SDLApp::submitText(const std::string &text) {
    qDebug() << "Emitting textEntered signal with text:" << QString::fromStdString(text);
    this->submittedText = "Message from mainwindow: " + text;
    render();
}

/*
 * Renders the SDL window with various elements such as text, buttons, and images
*/
void SDLApp::render() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red, for visibility
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // Background color
    SDL_RenderClear(renderer);
    if (!submittedText.empty()) {
        renderMessageFromMainwindow(submittedText, 50, 400, 24);
    }

    int imgWidth, imgHeight;
    SDL_QueryTexture(imageTexture, NULL, NULL, &imgWidth, &imgHeight);
    SDL_Rect destRect = {0, 0, imgWidth, imgHeight};
    SDL_RenderCopy(renderer, imageTexture, NULL, &destRect);

    int inputFieldX = 50;
    int inputFieldY = 140;
    int inputFieldWidth = 540;
    int inputFieldHeight = 50;

    if (textInputMode) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    }
    SDL_Rect inputRect = {inputFieldX, inputFieldY, inputFieldWidth, inputFieldHeight};
    SDL_RenderFillRect(renderer, &inputRect);

    int buttonX1 = inputFieldX + inputFieldWidth / 4 - BUTTON_WIDTH / 2;
    int buttonX2 = inputFieldX + inputFieldWidth * 3 / 4 - BUTTON_WIDTH / 2;

    renderButton(renderer, "Submit Text", buttonX1, inputFieldY + 60);
    renderButton(renderer, "Close Window", buttonX2, inputFieldY + 60);

    if (!textInputBuffer.empty()) {
        renderMessageFromMainwindow(textInputBuffer, 55, 145, 24);
    }

    SDL_RenderPresent(renderer);
}

/*
 * Creates a texture from the provided message using the initialized font
 * @param message - The text to create the texture from
 * @param textWidth - Reference to store the width of the created texture
 * @param textHeight - Reference to store the height of the created texture
 * @return SDL_Texture* - Pointer to the created texture, or nullptr if creation fails
*/
SDL_Texture *SDLApp::createTextTexture(const std::string &message, int &textWidth, int &textHeight) {
    if (!font) {
        qDebug() << "Font not initialized.";
        return nullptr;
    }

    SDL_Surface *surface = TTF_RenderText_Solid(font, message.c_str(), textColor);
    if (!surface) {
        qDebug() << "Failed to create text surface:" << TTF_GetError();
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        qDebug() << "Failed to create texture from surface:" << SDL_GetError();
        return nullptr;
    }

    SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);
    return texture;
}

/*
 * Renders a message from the main window on the specified renderer at the given position with the provided font size
 * @param message - The message to render
 * @param x - The x-coordinate of the top-left corner of the message
 * @param y - The y-coordinate of the top-left corner of the message
 * @param fontSize - The font size of the message
*/
void SDLApp::renderMessageFromMainwindow(const std::string &message, int x, int y, int fontSize) {
    int textWidth, textHeight;
    SDL_Texture *texture = createTextTexture(message, textWidth, textHeight);
    if (!texture) return;

    SDL_Rect renderQuad = {x, y, textWidth, textHeight};
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
    SDL_DestroyTexture(texture);
}

/*
 * @param renderer - The SDL renderer to render the button on
 * @param text - The text to display on the button
 * @param x - The x-coordinate of the top-left corner of the button
 * @param y - The y-coordinate of the top-left corner of the button
*/
void SDLApp::renderButton(SDL_Renderer *renderer, const char *text, int x, int y) {
    SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
    SDL_Rect buttonRect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    SDL_RenderFillRect(renderer, &buttonRect);

    int textWidth, textHeight;
    SDL_Texture *textTexture = createTextTexture(text, textWidth, textHeight);
    if (!textTexture) return;

    int textX = x + (BUTTON_WIDTH - textWidth) / 2;
    int textY = y + (BUTTON_HEIGHT - textHeight) / 2;
    SDL_Rect textRect = {textX, textY, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}
