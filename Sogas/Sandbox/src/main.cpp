
#include <iostream>
#include <vector>

#include <engine/engine.h>

int main()
{
    std::unique_ptr<sogas::engine::Engine> engine = std::make_unique<sogas::engine::Engine>();

    engine->init();
    engine->run();
    engine->shutdown();

    return 0;
}