# Qt and SDL Integration Project

This project demonstrates the integration of the Qt framework with the Simple DirectMedia Layer (SDL) library to create a GUI application that leverages SDL's functionalities for rendering and handling multimedia.
![final_app](https://github.com/danielrmenzel/sdl2_qt6_framework_combo/assets/109724990/52805229-624b-4ccb-a0eb-d9063c580bec)



## Getting Started

These instructions will show you how to get a copy of the project up and running.

### Prerequisites

- Qt6 (Ensure you have Qt Creator installed for ease of use)
- SDL2, SDL2_image, and SDL2_ttf libraries
- CMake (for building the application)

### Installing

A step-by-step series of examples that tell you how to get a development environment running.

1. **Install SDL2 Libraries**

Depending on your operating system, you will need to install SDL2, SDL2_image, and SDL2_ttf.

**Linux**

For Linux, you can use:
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

**Apple**

For Apple, you can use install the SDL2 Libraries via homebrew.
First, if not done so already, install homebrew:

/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

Then, run the following commands:

brew install sdl2
brew install sdl2_image
brew install sdl2_ttf

**Windows**

In Windows, the installation of the SDL2 related Frameworks is somewhat difficult.
For this reason, I've included the recent releases [April 2024] of SDL2, SDL2image and SDL2ttf in this project.
The paths in the CMakeLists.txt also lead to these libraries.
Feel free to replace them with other versions or change paths if needed.


2. **Clone the repository**
In order to clone this repo into your QT Creator, open the app and go to File->New Project.
The following window will open:

![newproject](https://github.com/danielrmenzel/sdl2_qt6_framework_combo/assets/109724990/afff8911-8cd0-46fb-84ed-b299a737049f)
Select the marked options a click on "Choose..."

The following will show:

![path](https://github.com/danielrmenzel/sdl2_qt6_framework_combo/assets/109724990/02b65b6b-82ed-454f-8e51-672d622f8efc)

Copy this link https://github.com/danielrmenzel/sdl2_qt6_framework_combo/tree/main to this repo into the field named "Repository", adjust the directory path as needed, and click on "Next".

The following warning will show up:

![warning](https://github.com/danielrmenzel/sdl2_qt6_framework_combo/assets/109724990/524f6017-493b-421b-8dae-bbfd1bc3077f)

Click "OK".

3. **Configure the Project**

QT Creator will now ask you to configure your project.
Qt Creator will prompt you to configure the project. Select the appropriate kit for your development environment.

![configure](https://github.com/danielrmenzel/sdl2_qt6_framework_combo/assets/109724990/abc25784-dcf0-4f74-b3e6-375394cee069)


4. **Build the Project**

Click on the "Build" button in Qt Creator to build the project.

## Running the Application

After building the project, you can run the application directly from Qt Creator by clicking the "Run" button.
This will create an executable file which you can also run independently from QT Creator by simply double clicking it.


## Authors

* **Daniel Menzel** - *Initial work* - [YourGitHub](https://github.com/daniel_r_menzel)
