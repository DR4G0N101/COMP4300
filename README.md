# COMP4300 – Game Programming

I wanted to learn game programming from the ground up, so I found Dave Churchill’s COMP4300 Game Programming course on YouTube and decided to follow it independently.

This repository documents my progress through the course. It contains the original starter files and my own completed implementations. My goal is not only to complete the assignments, but also to understand the concepts and techniques used in game programming.

## Technologies Used

- C++ / C++ 17 
- SFML 3
- ImGui
- ImGui-SFML
- CMake
- Visual Studio

## Topics Covered

Throughout the course, I will learn and practise concepts such as:

- Game loops
- Event handling and user input
- 2D graphics and rendering
- Movement and collision detection
- File handling and configuration parsing
- Data-driven programming
- Object-oriented programming
- Entity-component systems
- Game physics
- User-interface development
- Game architecture and code organisation

## Assignments

Each assignment has its own directory containing:

- The original starter files
- My completed implementation
- Configuration files and other required resources
- Assignment-specific documentation

## Assignment 1 – Shape Animation

Assignment 1 is a data-driven SFML application that reads shape definitions from a configuration file and displays animated circles and rectangles.

### Features

- Reads window, font, and shape data from a configuration file
- Stores an arbitrary number of shapes using `std::vector`
- Draws circles and rectangles with SFML
- Moves shapes using configurable velocities
- Detects collisions with the window boundaries
- Makes shapes bounce off the window edges
- Draws each shape’s name in its centre
- Provides an ImGui interface for selecting and editing shapes
- Allows the user to change visibility, scale, velocity, color, and name

### What I Learned

- Reading files with `std::ifstream`
- Parsing individual lines with `std::istringstream`
- Storing related data in C++ structures
- Using references and standard library containers
- Separating input, update, and rendering operations
- Loading fonts and drawing text with SFML
- Calculating shape and text boundaries
- Implementing basic collision detection
- Building an immediate-mode interface with ImGui
- Converting RGB values between integer and floating-point formats
- Updating program behaviour from external configuration data

## Course Resources

For more information about Dave Churchill and his courses, visit:

- [Dave Churchill’s website](https://davechurchill.ca/)
- [Dave Churchill’s YouTube courses](https://www.youtube.com/@DaveChurchill/courses)

## Disclaimer

This repository was created for independent study and is not an official COMP4300 repository. The course and its original materials belong to Dave Churchill and their respective institution.
