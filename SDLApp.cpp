#include "SDLApp.h"
#include <QDebug>
#include <iostream>


#include <unistd.h>

// Add these to your SDLApp class definition
static constexpr int SQUARE_WIDTH = 540;
static constexpr int SQUARE_HEIGHT = 50;
int squareX = 50; // X position of the square's top-left corner
int squareY = 140;  // Y position of the square's top-left corner

// int inputFieldX = 50;
// int inputFieldY = 140;
// int inputFieldWidth = 540;
// int inputFieldHeight = 50;



SDLApp::SDLApp(QObject *parent) : QObject(parent), window(nullptr), renderer(nullptr), drawLineFlag(false), quit(false), textInputMode(false), textInputBuffer(""), font(nullptr), buttonText("Enter Text") {
    if (!init()) {
        qDebug() << "Failed to initialize SDLApp.";
    }
}

SDLApp::~SDLApp() {
    cleanUp();
}

bool SDLApp::init() {
    qDebug() << "Initializing SDLApp...";
    buttonText = "Enter Text"; // Set initial button text @TODO
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
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working dir: " << cwd << std::endl;
    } else {
        perror("getcwd() error");
    }

    std::string fontPath = std::string(PROJECT_ROOT_PATH) + "assets/fonts/alagard.ttf";
    font = TTF_OpenFont(fontPath.c_str(), 24);
    if (!font) {
        qDebug() << "Failed to load font:" << TTF_GetError();
        return false;
    }
    //64 64 64 for grey
    textColor = {255, 105, 180, 255}; // RGB values for dark grey, 255 for full opacity

    std::string imagePath = std::string(PROJECT_ROOT_PATH) + "assets/images/banner.jpg";
    qDebug() << "img path: " + imagePath;

    imageTexture = IMG_LoadTexture(renderer, imagePath.c_str());
    if (!imageTexture) {
        qDebug() << "Failed to load image:" << IMG_GetError();
        return false;
    }

    qDebug() << "SDLApp initialized successfully.";
    return true;
}
void SDLApp::submitText(const std::string &text) {
    this->submittedText = text;
    render();
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
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Check for click within the designated square
            if (mouseX >= squareX && mouseX <= squareX + SQUARE_WIDTH &&
                mouseY >= squareY && mouseY <= squareY + SQUARE_HEIGHT) {
                // Toggle text input mode or any other desired action
                qDebug("Mouse click in designated text input area detected.");
                textInputMode = !textInputMode;
                if (textInputMode) SDL_StartTextInput();
                else SDL_StopTextInput();
            } else {
                // Handle other button clicks if not within square
                handleButtonClick(e);
            }
        }
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
    SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
    SDL_Rect buttonRect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    SDL_RenderFillRect(renderer, &buttonRect);

    renderText(text, x + 10, y + 10, 16);
}

bool SDLApp::isMouseInsideButton(int mouseX, int mouseY, int buttonX, int buttonY, int buttonWidth, int buttonHeight) {
    return (mouseX >= buttonX && mouseX <= buttonX + buttonWidth && mouseY >= buttonY && mouseY <= buttonY + buttonHeight);
}



void SDLApp::handleButtonClick(SDL_Event& event) {
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
            // Only if we are in text input mode, we process the text submission
            textInputMode = false;
            SDL_StopTextInput(); // Optionally stop SDL text input
            emit textEntered(QString::fromStdString(textInputBuffer));
            textInputBuffer.clear();
            buttonText = "Enter Text"; // Reset button text or handle as needed
            render();
        } else {
            // If not in text input mode, enable it
            textInputMode = true;
            SDL_StartTextInput();
            buttonText = "Submit Text";
            render();
        }
    } else if (isMouseInsideButton(mouseX, mouseY, buttonX2, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
        // Handle other button clicks, for example, closing the application
        qDebug() << "Button 2 clicked. Closing window.";
        quit = true;
        if (quit) {
            emit quitApplication();
        }
    }
    // Clicks outside of button areas do not change textInputMode
}


void SDLApp::handleTextInput(SDL_Event& event) {
    if (textInputMode) {
        textInputBuffer += event.text.text;
        render();
    }
}
void SDLApp::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Background color
    SDL_RenderClear(renderer);
    // Render the submitted text in pink
    if (!submittedText.empty()) {
        renderText(submittedText, 50, 400, 24); // Example position and size
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    SDL_Rect inputRect = {inputFieldX, inputFieldY, inputFieldWidth, inputFieldHeight};
    SDL_RenderFillRect(renderer, &inputRect);

    // Reset drawing color for other elements
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Calculate button positions based on input field dimensions
    int buttonX1 = inputFieldX + inputFieldWidth / 4 - BUTTON_WIDTH / 2;
    int buttonX2 = inputFieldX + inputFieldWidth * 3 / 4 - BUTTON_WIDTH / 2;

    // Draw buttons centered horizontally relative to the input field
    renderButton(renderer, buttonText.c_str(), buttonX1, buttonY);
    renderButton(renderer, "Close Window", buttonX2, buttonY);

    // Render the text input buffer if any
    renderText(textInputBuffer, 55, 145, 24); // Adjust text position

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
    if (imageTexture) {
        SDL_DestroyTexture(imageTexture);
        imageTexture = nullptr;
    }

    TTF_Quit();
    IMG_Quit();

    SDL_Quit();
    qDebug() << "SDLApp cleaned up and quit.";
}
