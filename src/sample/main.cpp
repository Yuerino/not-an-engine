#include "core/App.hpp"
#include "sample/BasicScene.hpp"

int main() {
    nae::App app{};
    app.setActiveScene<nae::BasicScene>();
    app.run();

    return EXIT_SUCCESS;
}
