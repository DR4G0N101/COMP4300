#include <SFML/Graphics.hpp>

#include "Game.h"
#include "Vec2.h"
#include <iostream>

int main() {
    Game g("config.txt");
    g.run();

	// Test Vec2 class
    Vec2 v1(100, 100);
    Vec2 v2(200, 200);

    std::cout << "v1: " << v1.x << ", " << v1.y << "\n";
    std::cout << "v2: " << v2.x << ", " << v2.y << "\n";

    Vec2 v3 = v1 + v2;
    std::cout << "v3 = v1 + v2: " << v3.x << ", " << v3.y << "\n";

    Vec2 v4 = v1 * 2;
    std::cout << "v4 = v1 * 2: " << v4.x << ", " << v4.y << "\n";

    return 0;
}
