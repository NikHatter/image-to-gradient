#include <iostream>
#include <sstream>

#include "gradient.hpp"
#include "image/boost_image.hpp"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: image-to-gradient image_path output_path";
        return 1;
    }

    std::filesystem::path image_path = argv[1];
    std::filesystem::path output_path = argv[2];

    std::cout << "RGBA" << std::endl;

    using namespace ItG;

    auto image = Image::gil::load<Image::gil::RGBA>(image_path);
    auto view = boost::gil::view(image);
    auto linear = Image::gil::get_linear< Gradient::LinearRGBA >(view, 0.0f, 0.5f, 1.0f, 0.5f);

    auto gradient = Gradient::from_gradient<Gradient::Operator::MaxDifference, Gradient::Strategy::Approximate>(linear);

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
