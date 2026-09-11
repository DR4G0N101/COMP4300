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

	bool foundWindow = false;
	bool foundFont = false;
    
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
            foundWindow = true;
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
			foundFont = true;
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

    if (!foundWindow) {
		std::cerr << "Window configuration not found in file: " << filename << "\n";
		return false;
    }
	if (!foundFont) {
		std::cerr << "Font configuration not found in file: " << filename << "\n";
        return false;
	}
    if(windowConfig.width == 0 || windowConfig.height == 0) {
        std::cerr << "Window width and height must be greater than 0\n";
        return false;
	}
    if(shapes.empty()) {
        std::cerr << "No shapes found in configuration file: " << filename << "\n";
        return false;
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

    // ImGui::GetIO().FontGlobalScale = 2.0f; // this line affect text size

    // let's load a font, so we can display some text
    sf::Font myFont;
    sf::CircleShape circle;
	sf::RectangleShape rectangle;

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

                if (keyPressed->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
            }
        }

        // update imgui for this frame with the time that the last frame took
        ImGui::SFML::Update(window, deltaClock.restart());
        // ImGui::ShowDemoWindow();

        // draw the UI
        ImGui::Begin("Shape Editor");
        
		ImGui::Text("Loaded shapes: %zu", shapes.size());

        ImGui::End();

        // basic rendering function calls
        window.clear();     // clear the window of anything previously drawn

        for (const ShapeConfig& shape : shapes) {
            const sf::Color shapeColor(
                static_cast<std::uint8_t>(shape.red),
                static_cast<std::uint8_t>(shape.green),
                static_cast<std::uint8_t>(shape.blue)
			);

            if (shape.type == "Circle") {
				circle.setRadius(shape.radius);
                circle.setPosition({ shape.positionX, shape.positionY });
				circle.setFillColor(shapeColor);
                window.draw(circle);
            }
			else if (shape.type == "Rectangle") {
				rectangle.setSize({ shape.width, shape.height });
				rectangle.setPosition({ shape.positionX, shape.positionY });
				rectangle.setFillColor(shapeColor);
				window.draw(rectangle);
            }
        }

        ImGui::SFML::Render(window);    // draw the ui last so it's on top
        window.display();               // call the window display function
    }

    ImGui::SFML::Shutdown();

    return 0;
}