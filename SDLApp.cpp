// SDLApp.cpp
#include "SDLApp.h"
#include <QCoreApplication> // Include QCoreApplication for platform detection
#include <QDebug>
#include <QDir>
#include <iostream>
#include <filesystem>
#include <unistd.h>

// Constants for text input field size and position within SDL2 Window
static constexpr int SQUARE_WIDTH = 540;
static constexpr int SQUARE_HEIGHT = 50;
int squareX = 50;
int squareY = 140;

/*
 * Constructor for SDLApp
 * @param parent - The parent object
 */
SDLApp::SDLApp(QObject *parent)
    : QObject(parent)
    , // Set the parent QObject
    window(nullptr)
    , // SDL Window
    renderer(nullptr)
    , // SDL Renderer
    closeWindowBtnClickedSDL(false)
    , // Flag to track status of 'Close Window' button
    textInputMode(false)
    , // Flag to track if text input mode is active inside SDL2 Window
    textInputBuffer("")
    ,             // Buffer to store text input
    font(nullptr) // Font for rendering text, tbd
{}

/*
 *Destructor for SDLApp
*/
SDLApp::~SDLApp()
{
    cleanUp();
}

/*
 * Opens or closes SDL2 Window from within QT main window
*/
void SDLApp::openOrToggleWindow()
{
    if (window == nullptr) {
        if (!init()) {
            qDebug() << "Failed to initialize SDLApp.";
        } else {
            render();
        }
    } else {
        cleanUp(); // closes SDL2 window if it already exists
    }
}

/*
 * Cleans up SDL resources specific to the window
*/
void SDLApp::cleanUpSDLWindow()
{
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

/*
 * Initializes SDLApp
*/
bool SDLApp::init()
{
    qDebug() << "Initializing SDLApp...";
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        qDebug() << "SDL_Init Error:" << SDL_GetError();
        return false;
    }

    if (TTF_Init() == -1) {
        qDebug() << "TTF_Init Error:" << TTF_GetError();
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        qDebug() << "IMG_Init Error:" << IMG_GetError();
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
    initTextAndImages();

    qDebug() << "SDLApp initialized successfully.";
    return true;
}

/*
 * Initializes SDL2 text and image capabilities
*/

bool SDLApp::initTextAndImages()
{
    QString projectRootPath = QCoreApplication::applicationDirPath(); // Get current directory path
    qDebug() << "project root path before moving up in hierarchy" << projectRootPath;
    std::filesystem::path currentDir = std::filesystem::current_path();
    qDebug() << "currentDir:" << currentDir;
    std::string fullPath(__FILE__);
    std::filesystem::path pathObj(fullPath);
    std::filesystem::path dirPath = pathObj.parent_path();

    std::cout << "Directory of main.cpp: " << dirPath << std::endl;



    QDir dir(projectRootPath); // Create a QDir object with the current directory path
    dir.cdUp();                // Move up one directory
    dir.cdUp();                // Move up another directory

    projectRootPath = dir.absolutePath() + "/"; // Get the absolute path of the new directory

// Define PROJECT_ROOT_PATH based on platform
    std::string imagePath;
    std::string fontPath;

#ifdef Q_OS_WIN
    //projectRootPath = QCoreApplication::applicationDirPath() + "/";
    //qDebug() << "projectRootPath windows: " << projectRootPath;
    imagePath = projectRootPath.toStdString() + "assets/images/banner.jpg";
    fontPath = projectRootPath.toStdString() + "assets/fonts/alagard.ttf";


#else
    imagePath = dirPath.string() + "/assets/images/banner.jpg";
    fontPath = dirPath.string() + "/assets/fonts/alagard.ttf";
    //projectRootPath = QCoreApplication::applicationDirPath().section("/", 0, -4) + "/";

#endif
    qDebug() << "project root path" << projectRootPath;
    qDebug() << "img path: " + QString::fromStdString(imagePath);

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
 * Submitted text entered in QT Window
*/
void SDLApp::submitText(const std::string &text)
{
    qDebug() << "Emitting textEntered signal with text:" << QString::fromStdString(text);

    this->submittedText = "Message from mainwindow: " + text;
    render();
    //emit textEntered(QString::fromStdString(text));
}

/*
 * Processes text entering and buttons
*/
void SDLApp::processEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            qDebug() << "SDL_QUIT event received (X-Button), closing SDL window.";
            cleanUpSDLWindow();
            break;
        case SDL_TEXTINPUT:
            // Add the text from the event to your input buffer
            qDebug("character typed in SDL2 window");
            handleTextInput(e);
            break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_BACKSPACE && !textInputBuffer.empty()) {
                // Remove the last character from the text input buffer
                textInputBuffer.pop_back();
                render();
            } else if (e.key.keysym.sym == SDLK_RETURN) {
                // When Enter key is pressed in text input mode
                if (!textInputBuffer.empty()) {
                    // Assuming you want to process/submit the text when Enter is pressed
                    qDebug() << "Enter pressed, submitting text: "
                             << QString::fromStdString(textInputBuffer);
                    emit textEntered(
                        QString::fromStdString(textInputBuffer)); // Emit signal with the text

                    // Optionally clear the buffer after submission
                    textInputBuffer.clear();
                    render(); // Update the rendering to reflect the submitted or cleared text
                }
                textInputMode = false; // Exit text input mode if desired
            }
            break;
        case SDL_MOUSEBUTTONDOWN: {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Check for click within the designated square
            if (mouseX >= squareX && mouseX <= squareX + SQUARE_WIDTH && mouseY >= squareY
                && mouseY <= squareY + SQUARE_HEIGHT) {
                qDebug("Mouse click in designated text input area detected.");
                textInputMode = true;
                render();
                if (textInputMode) {
                    qDebug("Entering SDL_StartTextInput()");
                    SDL_StartTextInput();
                } else {
                    qDebug("Exiting SDL_StartTextInput()");

                    SDL_StopTextInput();
                }
            } else {
                handleButtonClick(e);
            }
        } break;
        default:
            break;
        }
    }
}

/*
 * Renders text message received from mainwindow
 * @param message  - message from mainwindow
 * @param x, y     - coordinates for position
 * @param fontSize - font Size
*/
void SDLApp::renderMessageFromMainwindow(const std::string &message, int x, int y, int fontSize)
{
    int textWidth, textHeight;
    SDL_Texture *texture = createTextTexture(message, textWidth, textHeight);
    if (!texture)
        return;

    SDL_Rect renderQuad = {x, y, textWidth, textHeight};
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
    SDL_DestroyTexture(texture);
}

/*
 * Renders Buttons
 * @param renderer - SDL Renderer
 * @param text     - button text
 * @param x,y      - coordinates for button position
*/
void SDLApp::renderButton(SDL_Renderer *renderer, const char *text, int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
    SDL_Rect buttonRect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    SDL_RenderFillRect(renderer, &buttonRect);

    int textWidth, textHeight;
    SDL_Texture *textTexture = createTextTexture(text, textWidth, textHeight);
    if (!textTexture)
        return;

    // Adjust text position based on button position and size
    int textX = x + (BUTTON_WIDTH - textWidth) / 2;
    int textY = y + (BUTTON_HEIGHT - textHeight) / 2;
    SDL_Rect textRect = {textX, textY, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

/*
 * creates texture for text
 * @param message    - text
 * @param textWidth  - width for text
 * @param textHeigth - heigth for text
*/
SDL_Texture *SDLApp::createTextTexture(const std::string &message, int &textWidth, int &textHeight)
{
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
 * Checks if mouse is hovering over button
 * @param mouseX, mouseY            - coordinates of cursor
 * @param buttonX, buttonY          - coordinates of button
 * @param buttonWidth, buttonHeigth - dimensions of button
*/
bool SDLApp::isMouseInsideButton(
    int mouseX, int mouseY, int buttonX, int buttonY, int buttonWidth, int buttonHeight)
{
    return (mouseX >= buttonX && mouseX <= buttonX + buttonWidth && mouseY >= buttonY
            && mouseY <= buttonY + buttonHeight);
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
 * handles text input
 * @param event - SDL Event
*/
void SDLApp::handleTextInput(SDL_Event &event)
{
    if (textInputMode) {
        textInputBuffer += event.text.text;
        render();
    }
}

/*
 * renders SDL window
*/
void SDLApp::render()
{
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // Background color
    SDL_RenderClear(renderer);
    if (!submittedText.empty()) {
        renderMessageFromMainwindow(submittedText, 50, 400, 24); // Example position and size
    }

    int imgWidth, imgHeight;
    SDL_QueryTexture(imageTexture, NULL, NULL, &imgWidth, &imgHeight);
    SDL_Rect destRect = {0, 0, imgWidth, imgHeight}; // Corrected line

    SDL_RenderCopy(renderer, imageTexture, NULL, &destRect);

    // Calculate input field position and size
    int inputFieldX = 50;
    int inputFieldY = 140;
    int inputFieldWidth = 540;
    int inputFieldHeight = 50;

    // Draw the white input field
    if (textInputMode) {
        // Render input field in a different color when in text input mode
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Light blue color
    } else {
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255); // Lighter grey color
    }
    SDL_Rect inputRect = {inputFieldX, inputFieldY, inputFieldWidth, inputFieldHeight};
    SDL_RenderFillRect(renderer, &inputRect);

    // Calculate button positions based on input field dimensions
    int buttonX1 = inputFieldX + inputFieldWidth / 4 - BUTTON_WIDTH / 2;
    int buttonX2 = inputFieldX + inputFieldWidth * 3 / 4 - BUTTON_WIDTH / 2;

    // Draw buttons centered horizontally relative to the input field
    renderButton(renderer, "Submit Text", buttonX1, buttonY);
    renderButton(renderer, "Close Window", buttonX2, buttonY);

    // Render the text input buffer if it's not empty
    if (!textInputBuffer.empty()) {
        renderMessageFromMainwindow(textInputBuffer, 55, 145, 24); // Adjust text position
    }

    SDL_RenderPresent(renderer);
}

/*
 * Checks if sdl2 window should be closed
*/
bool SDLApp::shouldQuit() const
{
    return closeWindowBtnClickedSDL;
}

/*
 * cleans up SDL app
*/
void SDLApp::cleanUp()
{
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
    if (imageTexture) {
        SDL_DestroyTexture(imageTexture);
        imageTexture = nullptr;
    }

    TTF_Quit();
    IMG_Quit();

    SDL_Quit();
    qDebug() << "SDLApp cleaned up and quit.";
}
