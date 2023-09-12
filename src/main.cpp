#include "core/App.hpp"
#include "core/util.hpp"
#include "scene/BasicScene.hpp"

int main() {
    nae::util::fragShaderPath = "../shader/shader.frag.spv";
    nae::util::vertShaderPath = "../shader/shader.vert.spv";

    nae::App app{};
    app.setActiveScene<nae::BasicScene>();
    app.run();

    return EXIT_SUCCESS;
}
