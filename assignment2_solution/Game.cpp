#include "Game.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <random>
#include <cmath>


Game::Game(const std::string &config) {
    init(config);
}

void Game::init(const std::string &path) {
    // TODO: read in config file here
    //       use the pre made PlayerConfig, EnemyConfig, BulletConfig variables

    // set up default window parameters
    m_window.create(sf::VideoMode({1280u, 720u}), "Assignment 2");
    m_window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(m_window))
    {
        throw std::runtime_error("Failed to initialize ImGui-SFML");
    }

    // scale the imgui ui and text size by 2
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    spawnPlayer();
}

void Game::run() {
    // TODO:
    // - add pause functionality in here
    // - some systems should function while paused (rendering)
    // - some systems shouldn't (movement / input)
    while (m_running && m_window.isOpen())
    {
        // update the entity manager
        m_entities.update();

        // required update call to imgui
        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sGUI();
        sRender();

        // increment the current frame
        // may need to be moved when pause implemented
        m_currentFrame++;
    }

    ImGui::SFML::Shutdown();
}

void Game::setPaused(bool paused) {
    // TODO
}

// respawn the player in the middle of the screen
void Game::spawnPlayer() {

    // We create every entity by calling EntityManager.addEntity(tag)
    // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
    auto entity = m_entities.addEntity("player");

    const auto windowSize = m_window.getSize();

    const Vec2 centre{
        static_cast<float>(windowSize.x) / 2.0f,
        static_cast<float>(windowSize.y) / 2.0f,
    };

    constexpr float shapeRadius = 32.0f;
    constexpr float collisionRadius = 32.0f;
    constexpr int vertices = 8;

    entity->cTransform = std::make_shared<CTransform>(
        centre, 
        Vec2(0.0f, 0.0f), 
        0.0f
    );

    // The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness 4
    entity->cShape = std::make_shared<CShape>(shapeRadius,
        vertices,
        sf::Color(10, 10, 10),
        sf::Color(255, 0, 0),
        4.0f
    );

    entity->cCollision = std::make_shared<CCollision>(collisionRadius);

    // Add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // Since we want this Entity to be our player, set our Game's player variable to be this Entity
    // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy() {

    static std::mt19937 randomEngine{
        std::random_device{}()
    };

    constexpr float radius = 32.0f;
    constexpr float minimumSpeed = 2.0f;
    constexpr float maximumSpeed = 5.0f;
    constexpr float pi = 3.14159265359f;

    const auto windowSize = m_window.getSize();

    const float windowWidth =
        static_cast<float>(windowSize.x);

    const float windowHeight =
        static_cast<float>(windowSize.y);

    std::uniform_real_distribution<float> xDistribution(
        radius,
        windowWidth - radius
    );

    std::uniform_real_distribution<float> yDistribution(
        radius,
        windowHeight - radius
    );

    std::uniform_real_distribution<float> speedDistribution(
        minimumSpeed,
        maximumSpeed
    );

    std::uniform_real_distribution<float> angleDistribution(
        0.0f,
        2.0f * pi
    );

    std::uniform_int_distribution<int> vertexDistribution(3, 8);
    std::uniform_int_distribution<int> colorDistribution(50, 255);

    const Vec2 position{
        xDistribution(randomEngine),
        yDistribution(randomEngine)
    };

    const float speed =
        speedDistribution(randomEngine);

    const float angle =
        angleDistribution(randomEngine);

    const Vec2 velocity{
        std::cos(angle) * speed,
        std::sin(angle) * speed
    };

    const int vertices =
        vertexDistribution(randomEngine);

    const sf::Color fillColor{
        static_cast<std::uint8_t>(colorDistribution(randomEngine)),
        static_cast<std::uint8_t>(colorDistribution(randomEngine)),
        static_cast<std::uint8_t>(colorDistribution(randomEngine))
    };

    auto enemy = m_entities.addEntity("enemy");

    enemy->cTransform = std::make_shared<CTransform>(
        position,
        velocity,
        0.0f
    );

    enemy->cShape = std::make_shared<CShape>(
        radius,
        vertices,
        fillColor,
        sf::Color::White,
        2.0f
    );

    enemy->cCollision =
        std::make_shared<CCollision>(radius);

    enemy->cScore =
        std::make_shared<CScore>(vertices * 100);

    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
    // TODO: spawn small enemies at the location of the input enemy e

    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the vertices of the original enemy
    // - set each small enemy to the same color as the original, half the size
    // - small enemies are worth double points of the original enemy
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> shooter, const Vec2 &target) {
    // TODO: implement the spawning of a bullet which travels toward target
    // - bullet speed is given as a scalar speed
    // - you must set the velocity by using formula in notes

    constexpr float bulletSpeed = 20.0f;
    constexpr float bulletRadius = 10.0f;
    constexpr int bulletVertices = 20;
    constexpr int bulletLifespan = 90;

    const Vec2 direction = target - shooter->cTransform->pos;
    const float distance = direction.length();

    // Avoid division by zero if the mouse is exactly at the player's centre
    if (distance == 0.0f) { return; }

    const Vec2 normalizeDirection = direction / distance;

    const Vec2 velocity = normalizeDirection * bulletSpeed;

    auto bullet = m_entities.addEntity("bullet");

    bullet->cTransform = std::make_shared<CTransform>(
        shooter->cTransform->pos,
        velocity,
        0.0f
    );

    bullet->cShape = std::make_shared<CShape>(
        bulletRadius,
        bulletVertices,
        sf::Color::White,
        sf::Color::Red,
        2.0f
    );

    bullet->cCollision = std::make_shared<CCollision>(bulletRadius);
    bullet->cLifespan = std::make_shared<CLifespan>(bulletLifespan);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
    // TODO: implement your own special weapon
}

void Game::sMovement() {
    
    constexpr float playerSpeed = 8.0f;

    for (const auto& entity : m_entities.getEntities()) {
        if (!entity->isActive() || !entity->cTransform) { continue; }

        // Only entities with the CInput are controller by the player.
        if (entity->cInput) {

            Vec2 direction(0.0f, 0.0f);

            if (entity->cInput->up) { direction.y -= 1.0f; }
            if (entity->cInput->down) { direction.y += 1.0f; }
            if (entity->cInput->left) { direction.x -= 1.0f; }
            if (entity->cInput->right) { direction.x += 1.0f; }
            if (direction.length() > 0.0f) { direction /= direction.length(); }

            entity->cTransform->velocity = direction * playerSpeed;
        }
        entity->cTransform->pos += entity->cTransform->velocity;

        if (entity->cInput && entity->cCollision) {
            const auto windowSize = m_window.getSize();
            const float radius = entity->cCollision->radius;

            const float windowWidth = static_cast<float>(windowSize.x);
            const float windowHeight = static_cast<float>(windowSize.y);

            entity->cTransform->pos.x = std::clamp(entity->cTransform->pos.x, radius, windowWidth - radius);

            entity->cTransform->pos.y = std::clamp(entity->cTransform->pos.y, radius, windowHeight - radius);
        }

        if (entity->tag() == "enemy" && entity->cCollision) {

            const auto windowSize = m_window.getSize();
            const float windowWidth = static_cast<float>(windowSize.x);
            const float windowHeight = static_cast<float>(windowSize.y);
            const float radius = entity->cCollision->radius;
            auto& position = entity->cTransform->pos;
            auto& velocity = entity->cTransform->velocity;

            if (position.x - radius <= 0.0f) {
                position.x = radius;
                velocity.x = std::abs(velocity.x);
            } else if (position.x + radius >= windowWidth) {
                position.x = windowWidth - radius;
                velocity.x = -std::abs(velocity.x);
            } if (position.y - radius <= 0.0f) {
                position.y = radius;
                velocity.y = std::abs(velocity.y);
            } else if (position.y + radius >= windowHeight) {
                position.y = windowHeight - radius;
                velocity.y = -std::abs(velocity.y);
            }
        }
    }
}

void Game::sLifespan() {
    // TODO: implement all lifespan functionality
    //
    // for all entities
    // - if entity has no lifespan component, skip it
    // - if entity has > 0 remaining lifespan, subtract 1
    // - if it has lifespan and is alive scale its alpha channel properly
    // - if it has lifespan and its time is up destroy the entity
}

void Game::sCollision() {
    for (const auto& bullet : m_entities.getEntities("bullet")) {
        if (!bullet->isActive() || !bullet->cTransform || !bullet->cCollision) { continue; }

        for (const auto& enemy : m_entities.getEntities("enemy")) {
            if (!enemy->isActive() || !enemy->cTransform || !enemy->cCollision) { continue; }

            const float distance = bullet->cTransform->pos.dist(enemy->cTransform->pos);

            const float collisionDistance = bullet->cCollision->radius + enemy->cCollision->radius;

            if (distance < collisionDistance) {
                bullet->destroy();
                enemy->destroy();

                if (enemy->cScore) { m_score += enemy->cScore->score; }

                std::cout << "Score: " << m_score << '\n';

                break;
            }

            if (!m_player ||
                !m_player->isActive() ||
                !m_player->cTransform ||
                !m_player->cCollision)
            {
                return;
            }

            for (const auto& enemy : m_entities.getEntities("enemy")) {
                if (!enemy->isActive() || !enemy->cTransform || !enemy->cCollision) { continue; }

                const float distance = m_player->cTransform->pos.dist(enemy->cTransform->pos);

                const float collisionDistance = m_player->cCollision->radius + enemy->cCollision->radius;

                if (distance < collisionDistance) {
                    enemy->destroy();
                    m_player->destroy();

                    spawnPlayer();

                    break;
                }
            }
        }
    }
}

void Game::sEnemySpawner() {
    // TODO: code which implements enemy spawning should go here

    constexpr int spawnInterval = 60;

    const int framesSinceLastSpawn = m_currentFrame - m_lastEnemySpawnTime;

    if (framesSinceLastSpawn >= spawnInterval) {
        spawnEnemy();
    }
}

void Game::sGUI() {
    ImGui::Begin("Geometry Wars");

    ImGui::Text("Stuff Goes Here");

    ImGui::End();
}

void Game::sRender() {
    // TODO: change the code below to draw ALL of the entities
    // sample drawing of the player Entity that we have created
    m_window.clear(sf::Color::Black);

    for (const auto& entity : m_entities.getEntities()) {
        if (!entity->isActive()) { continue; }
        if (!entity->cTransform || !entity->cShape) { continue; }

        entity->cTransform->angle += 1.0f;

        entity->cShape->circle.setPosition({
            entity->cTransform->pos.x,
            entity->cTransform->pos.y
            }
        );

        entity->cShape->circle.setRotation(
            sf::degrees(entity->cTransform->angle)
        );

        m_window.draw(entity->cShape->circle);
    }

    // draw the ui last
    ImGui::SFML::Render(m_window);

    m_window.display();
}

void Game::sUserInput() {
    // TODO: handle user input here
    // note that you should only be setting the player's input component variables here
    // you shold not implement the player's movement logic here
    // the movement system will read the variables you set in this functioin

    while (const auto event = m_window.pollEvent())
    {
        // pass the event to imgui to be parsed
        ImGui::SFML::ProcessEvent(m_window, *event);

        // this event triggers when the window is closed
        if (event->is<sf::Event::Closed>())
        {
            m_running = false;
            m_window.close();
        }

        // this event is triggered when a key is pressed
        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->code) {
                case sf::Keyboard::Key::W:
                    std::cout << "W Key Pressed\n";
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::Key::S:
                    std::cout << "S Key Pressed\n";
                    m_player->cInput->down = true;
                    break;

                case sf::Keyboard::Key::A:
                    std::cout << "A Key Pressed\n";
                    m_player->cInput->left = true;
                    break;

                case sf::Keyboard::Key::D:
                    std::cout << "D Key Pressed\n";
                    m_player->cInput->right = true;
                    break;

                default:
                    break;
            }
        }

        // this event is triggered when a key is released
        if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            switch (keyReleased->code) {
                case sf::Keyboard::Key::W:
                    std::cout << "W Key Released\n";
                    m_player->cInput->up = false;
                    break;

                case sf::Keyboard::Key::S:
                    std::cout << "S Key Released\n";
                    m_player->cInput->down = false;
                    break;

                case sf::Keyboard::Key::A:
                    std::cout << "A Key Released\n";
                    m_player->cInput->left = false;
                    break;

                case sf::Keyboard::Key::D:
                    std::cout << "D Key Released\n";
                    m_player->cInput->right = false;
                    break;

                default:
                    break;
            }

        }

        if (const auto *mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            // this line ignores mouse events if ImGui is the thing being clicked
            if (ImGui::GetIO().WantCaptureMouse)
            { continue; }

            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                std::cout << "Left Mouse Button Clicked at(" << mouseButtonPressed->position.x
                          << ", " << mouseButtonPressed->position.y << ")\n";
                
                // spawnBullet
                const Vec2 target{
                    static_cast<float>(mouseButtonPressed->position.x),
                    static_cast<float>(mouseButtonPressed->position.y)
                };
                spawnBullet(m_player, target);

            }

            if (mouseButtonPressed->button == sf::Mouse::Button::Right) {
                std::cout << "Right Mouse Button Clicked at(" << mouseButtonPressed->position.x
                          << ", " << mouseButtonPressed->position.y << ")\n";
                // TODO: call special weapon here
            }
        }
    }
}

// void collisions()
// {
// for (auto b : m_entities.getEntities("bullet"))
// for (auto e : m_entities.getEntities("enemy"))
//     if (Physics::CheckCollision(b, e))
//     {
//     b->destroy();
//     e->destroy();
//     }
// }
