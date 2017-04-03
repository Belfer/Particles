#include "engine.hpp"

#include "gui/imgui.h"
#include "gui/imgui-SFML.h"

int Engine::init(Game* game, const std::string& respath)
{
    srand(time(NULL));

    // Create the main window
    sf::ContextSettings settings;
    settings.majorVersion = GL_MAJOR;
    settings.minorVersion = GL_MINOR;
    settings.depthBits = GL_DEPTHBITS;
    settings.antialiasingLevel = GL_AALEVEL;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), WIN_TITLE, WIN_STYLE, settings);
    window.setFramerateLimit(TARGET_FPS);
    window.resetGLStates();

    // Configure game
    game->config(&window);

    // Add default view
    game->views.emplace_back(sf::View(sf::FloatRect(0, 0, CAM_WIDTH, CAM_HEIGHT)));
    game->views[0].setCenter(0, 0);

#if PROC_GUI
    ImGui::SFML::Init(window);
#endif

    // Initialize game
    if(!game->init(respath)) return EXIT_FAILURE;

    loop(game, window);

    return EXIT_SUCCESS;
}

void Engine::loop(Game* game, sf::RenderWindow& window)
{
    // Create a clock to track the elapsed time
    sf::Clock clock;
    sf::Time elapsed;

#if PROC_FIXED
    float frameTime = 1.f / TARGET_FPS;
    float accumulator = 0.f;
    float t = 0.f;
#endif

    // Start the game loop
    while(window.isOpen())
    {
        elapsed = clock.restart();

        // Process events
        sf::Event event;
        while(window.pollEvent(event))
        {
            // Close window: exit
            if(event.type == sf::Event::Closed)
                window.close();

#if PROC_GUI
            ImGui::SFML::ProcessEvent(event);
#endif

            // Game input
            game->input(event);
        }

#if PROC_FIXED
        accumulator += elapsed.asSeconds();
        while (accumulator >= frameTime) {
            t += frameTime;
            accumulator -= frameTime;

            // Fixed update
            game->fixed(t, frameTime);
        }
#endif

#if PROC_GUI
        ImGui::SFML::Update(window, elapsed);
#endif

        // Update game
        game->update(elapsed);

        // Clear screen
        window.clear();

#if PROC_PRE_DRAW
        // Pre-Draw game
        game->pre_draw();
#endif

        // Draw scene
        for(size_t i=0; i<game->views.size(); ++i)
        {
            // Draw game
            game->draw(game->views[i]);
        }

#if PROC_POST_DRAW
        // Post-Draw game
        game->post_draw();
#endif

#if PROC_GUI
        ImGui::Render();
#endif

        // Update the window
        window.display();
    }

	// Clean resources
    game->clean();

#if PROC_GUI
    ImGui::SFML::Shutdown();
#endif
}
