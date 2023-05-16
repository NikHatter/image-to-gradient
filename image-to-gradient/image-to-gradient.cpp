#include <iostream>
#include <sstream>

#include "image-to-gradient.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: image-to-gradient image_path output_path";
        return 1;
    }

    std::filesystem::path image_path = argv[1];
    std::filesystem::path output_path = argv[2];

    std::cout << "RGBA" << std::endl;

    auto image = ItG::load<ItG::ImageRGBA::image_t>(image_path);
    auto gradient = ItG::image_to_linear(image, 0.2f, 0.8f);

    std::stringstream gradient_css;
    gradient_css << "linear-gradient(90deg";
    for (auto& key : gradient) {
        gradient_css << ", ";
        gradient_css << "rgba("
            << key.color[0] * 255. << ", "
            << key.color[1] * 255. << ", "
            << key.color[2] * 255. << ", "
            << key.color[3] * 100 << "%) "
            << key.position * 100 << "%";
    }
    gradient_css << ")";

    std::cout << gradient_css.rdbuf() << std::endl;

    return 0;
}


