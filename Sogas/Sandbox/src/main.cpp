#include <engine/engine.h>

int main()
{
    std::unique_ptr<sogas::Engine> engine = std::make_unique<sogas::Engine>();

    engine->init();
    engine->run();
    engine->shutdown();

    return 0;
}
