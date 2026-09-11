/************************************************************
* 
*          Assignment 1
*           COMP4300
* 
* ***********************************************************/

#include <cstddef>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

//////////////////////////////////
//       FILE HANDLING          //
//////////////////////////////////

struct WindowConfig {
    unsigned int width{};
    unsigned int height{};
};

struct FontConfig {
    std::string filename;

    unsigned int characterSize{};

	int red{};
	int green{};
	int blue{};
};

struct ShapeConfig {
    std::string type;
    std::string name;

	float positionX{};
    float positionY{};
	float velocityX{};
	float velocityY{};

	int red{};
	int green{};
	int blue{};

	float width{};
	float height{};
	float radius{};
    };

bool ReadConfigFile(
    const std::string& filename,
    WindowConfig& windowConfig,
    FontConfig& fontConfig,
    std::vector<ShapeConfig>& shapes) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open configuration file: " << filename << "\n";
        return false;
    }

    std::string line;

    // std::cout << filename << " loaded to memory: \n";

    while (std::getline(file, line)) {

        if (line.empty()) {
            continue;
        }

        std::istringstream lineStream(line);
        std::string type;

        if (!(lineStream >> type)) {
            std::cerr << "Error reading line: " << line << "\n";
            continue;
        }

        // std::cout << "Configuration type: "<< type << '\n';

        if (type == "Window") {
            if (!(lineStream
                >> windowConfig.width
                >> windowConfig.height)) {

                std::cerr << "Error reading window configuration: " << line << "\n";
                return false;
            }
        }
        else if (type == "Font") {
            if (!(lineStream
                >> fontConfig.filename
                >> fontConfig.characterSize
                >> fontConfig.red
                >> fontConfig.green
                >> fontConfig.blue)) {

                std::cerr << "Error reading font configuration: " << line << "\n";
                return false;
            }
        }
        else if (type == "Circle") {
            ShapeConfig shape;
            shape.type = type;

            if (!(lineStream
                >> shape.name
                >> shape.positionX
                >> shape.positionY
                >> shape.velocityX
                >> shape.velocityY
                >> shape.red
                >> shape.green
                >> shape.blue
                >> shape.radius)) {

                std::cerr << "Error reading circle configuration: " << line << "\n";
                return false;
            }
            shape.width = shape.radius * 2.0f;
            shape.height = shape.radius * 2.0f;
            shapes.push_back(shape);

        }
        else if (type == "Rectangle") {
            ShapeConfig shape;
            shape.type = type;

            if (!(lineStream
                >> shape.name
                >> shape.positionX
                >> shape.positionY
                >> shape.velocityX
                >> shape.velocityY
                >> shape.red
                >> shape.green
                >> shape.blue
                >> shape.width
                >> shape.height)) {
            
                std::cerr << "Error reading rectangle configuration: " << line << "\n";
                return false;
            }
            shapes.push_back(shape);
        }
        else {
            std::cerr << "Unknown configuration type: " << type << "\n";
            return false;
		}


    }
    std::cout << "Window width = " << windowConfig.width << '\n';
    std::cout << "Window height = " << windowConfig.height << "\n\n";
    std::cout << "Font filename = " << fontConfig.filename << '\n';
    std::cout << "Font character size = " << fontConfig.characterSize << '\n';
    std::cout << "Font color = (" << fontConfig.red << ", " << fontConfig.green << ", " << fontConfig.blue << ")\n\n";

	std::cout << "Shapes loaded from configuration file: " << shapes.size() << "\n";
    for (const auto& shape : shapes) {
        std::cout << "Shape type = " << shape.type << '\n';
        std::cout << "Shape name = " << shape.name << '\n';
        std::cout << "Shape position = (" << shape.positionX << ", " << shape.positionY << ")\n";
        std::cout << "Shape velocity = (" << shape.velocityX << ", " << shape.velocityY << ")\n";
        std::cout << "Shape color = (" << shape.red << ", " << shape.green << ", " << shape.blue << ")\n\n";
    }
    return true;
}

//////////////////////////////////
//            MAIN              //
//////////////////////////////////

int main() {

	WindowConfig windowConfig;
	FontConfig fontConfig;
    std::vector<ShapeConfig> shapes;

    if (!ReadConfigFile("config.txt", windowConfig, fontConfig, shapes)) {
        return 1;
    }

    // create a new window of size w*h pixels
    // top-left of the window is (0,0) and bottom-right is (w,h)
    // you will have to read these from the config file
    sf::RenderWindow window(sf::VideoMode({ windowConfig.width, windowConfig.height }), "Assignment 1");
    window.setFramerateLimit(60); // limit frame rate to 60 fps

    // initialize IMGUI and create a clock used for its internal timing
    if (!ImGui::SFML::Init(window))
    {
        std::cerr << "Could not initialize ImGui-SFML!\n";
        return 1;
    }
    sf::Clock deltaClock;

    // scale the imgui ui by a given factor, does not affect text size
    // ImGui::GetStyle().ScaleAllSizes(1.0f);

    // Scaling ImGUI to my monitor (3K)
    constexpr float uiScale = 2.0f;
    ImGui::GetStyle().ScaleAllSizes(uiScale);
    ImGui::GetIO().FontGlobalScale = uiScale;

    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Could not initialise ImGui-SFML!\n";
        return 1;
    }

    // ImGui::GetIO().FontGlobalScale = 2.0f; // this line affect text size

    // the imgui color {r, g, b} wheel requires floats from 0-1 instead of ints from 0-255
    float c[3] = { 0.0f, 1.0f, 1.0f };

    // let's make a shape that we will draw to the screen
    float circleRadius = 50;    // radius to draw the circle
    int circleSegments = 32;    // number of segments to draw the circle with
    float circleSpeedX = 1.0f;  // we will use this to move the circle later
    float circleSpeedY = 0.5f;  // you will read these values from the file
    bool drawCircle = true;     // whether to draw the circle
    bool drawText = true;       // whether to draw the text

    // create the sfml circle shape based on our parameters
    sf::CircleShape circle(circleRadius, circleSegments);   // create a circle shape with radius 50
    circle.setPosition({ 10.0f, 10.0f });                     // set the top-left position of the circle

    // let's load a font, so we can display some text
    sf::Font myFont;

    // attempt to load the font from a file
    if (!myFont.openFromFile(fontConfig.filename))
    {
        // if we can't load the font, print an error to the error console and exit
        std::cerr << "Could not load font: " << fontConfig.filename << "\n";
        ImGui::SFML::Shutdown();
        return 1;
    }

    // set up the text object that will be drawn to the screen
    sf::Text text(myFont, "Sample Text", fontConfig.characterSize);
	
    text.setFillColor(sf::Color(
        static_cast<std::uint8_t>(fontConfig.red), 
        static_cast<std::uint8_t>(fontConfig.green), 
        static_cast<std::uint8_t>(fontConfig.blue)
    ));


    // position the top-left corner of the text so that the text aligns on the bottom
    // text character size is in pixels, so move the text up from the bottom by its height
    text.setPosition({ 
        0.0f, 
        static_cast<float>(windowConfig.height) 
        - static_cast<float>(text.getCharacterSize())
        });

    // set up a character array to set the text
    char displayString[255] = "Sample Text";

    // main loop - continues for each frame while window is open
    while (window.isOpen())
    {
        // event handling
        while (const auto event = window.pollEvent())
        {
            // pass the event to imgui to be parsed
            ImGui::SFML::ProcessEvent(window, *event);

            // this event triggers whe the window is closed
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // this event is triggered when a key is pressed
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // print the key that was pressed to the console
                std::cout << "Key pressed with code = "
                    << static_cast<int>(keyPressed->code) << "\n";

                // example, what happens when x is pressed
                if (keyPressed->code == sf::Keyboard::Key::X)
                {
                    // reverse the x direction of the circle on the screen
                    circleSpeedX *= -1.0f;
                }
            }
        }

        // update imgui for this frame with the time that the last frame took
        ImGui::SFML::Update(window, deltaClock.restart());
        // ImGui::ShowDemoWindow();

        // draw the UI
        ImGui::Begin("Window title");
        ImGui::Text("Window text!");
        ImGui::Checkbox("Draw Circle", &drawCircle);
        ImGui::SameLine();
        ImGui::Checkbox("Draw Text", &drawText);
        ImGui::SliderFloat("Radius", &circleRadius, 0.0f, 300.0f);
        ImGui::SliderInt("Sides", &circleSegments, 3, 64);
        ImGui::ColorEdit3("Color Circle", c);
        ImGui::InputText("Text", displayString, 255);
        if (ImGui::Button("Set Text"))
        {
            text.setString(displayString);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Circle"))
        {
            circle.setPosition({ 0.0f, 0.0f });
        }
        ImGui::End();

        // set the circle properties, because they may have been updated with the ui
        circle.setFillColor(sf::Color(
            static_cast<std::uint8_t>(c[0] * 255.0f),
            static_cast<std::uint8_t>(c[1] * 255.0f),
            static_cast<std::uint8_t>(c[2] * 255.0f)));
        circle.setPointCount(static_cast<std::size_t>(circleSegments));
        circle.setRadius(circleRadius);

        // basic animation - move each shape if it's still in frame
        circle.move({ circleSpeedX, circleSpeedY });

        // basic rendering function calls
        window.clear();     // clear the window of anything previously drawn
        if (drawCircle)     // draw the circle if the boolean is true
        {
            window.draw(circle);
        }
        if (drawText)       // draw the text if the boolean is true
        {
            window.draw(text);
        }
        ImGui::SFML::Render(window);    // draw the ui last so it's on top
        window.display();               // call the window display function
    }

    ImGui::SFML::Shutdown();

    return 0;
}