#include "filters.h"

Image::Image() {
}

int32_t Image::GetHeight() const {
    return height_;
}
int32_t Image::GetWidth() const {
    return width_;
}

Pixel Image::GetPixel(size_t row, size_t pixel) {
    return image_[row][pixel];
}
void Image::SetPixelFl(size_t& row, size_t& pixel, float& r, float& g, float& b) {
    image_[row][pixel].SetColorFl(r, g, b);
}

void Image::SetPixel(size_t& row, size_t& pixel, uint8_t& r, uint8_t& g, uint8_t& b) {
    image_[row][pixel].SetColor(r, g, b);
}

void Image::SetPixel32(size_t& row, size_t& pixel, int32_t& r, int32_t& g, int32_t& b) {
    image_[row][pixel].SetColor32(r, g, b);
}

void Image::Crop(int32_t& width, int32_t& height) {
    if (width <= width_ && width > 0) {
        width_ = width;
        info_header_.width = width;
    }
    if (height <= height_ && height > 0) {
        height_ = height;
        info_header_.height = height;
    }

    offset_ = (PIXELS_ALIGNMENT - width_ * static_cast<int32_t>(PIXEL_SIZE) % PIXELS_ALIGNMENT) % PIXELS_ALIGNMENT;
    file_header_.Size = sizeof(TFileHeader) + sizeof(TInfoHeader) + width_ * height_ * PIXEL_SIZE;
    std::reverse(image_.begin(), image_.end());
    image_.resize(height_);
    std::reverse(image_.begin(), image_.end());
}

void Pixel::SetColorFl(float& r, float& g, float& b) {
    r = std::max(static_cast<float>(0), std::min(r, BYTEMAXIMUMVALUEFL));
    g = std::max(static_cast<float>(0), std::min(g, BYTEMAXIMUMVALUEFL));
    b = std::max(static_cast<float>(0), std::min(b, BYTEMAXIMUMVALUEFL));
    R = static_cast<uint8_t>(r);
    G = static_cast<uint8_t>(g);
    B = static_cast<uint8_t>(b);
}

void Pixel::SetColor(uint8_t& r, uint8_t& g, uint8_t& b) {
    r = std::max(static_cast<uint8_t>(0), std::min(r, BYTEMAXIMUMVALUE));
    g = std::max(static_cast<uint8_t>(0), std::min(g, BYTEMAXIMUMVALUE));
    b = std::max(static_cast<uint8_t>(0), std::min(b, BYTEMAXIMUMVALUE));
    R = r;
    G = g;
    B = b;
}

void Pixel::SetColor32(int32_t& r, int32_t& g, int32_t& b) {
    r = std::max(0, std::min(r, static_cast<int32_t>(BYTEMAXIMUMVALUE)));
    g = std::max(0, std::min(g, static_cast<int32_t>(BYTEMAXIMUMVALUE)));
    b = std::max(0, std::min(b, static_cast<int32_t>(BYTEMAXIMUMVALUE)));
    R = static_cast<uint8_t>(r);
    G = static_cast<uint8_t>(g);
    B = static_cast<uint8_t>(b);
}

void Image::Read(const std::string& input_path) {

    std::ifstream input(input_path, std::ios_base::binary);

    if (!input.is_open()) {
        throw(std::runtime_error("Failed to open " + input_path));
    }

    char header_read[sizeof(TFileHeader)];
    input.read(header_read, sizeof(TFileHeader));

    if (header_read[0] != 'B' || header_read[1] != 'M') {
        throw(std::runtime_error("The specified path is not a bitmap image.\n"));
    }

    TFileHeader* file_header_ptr = reinterpret_cast<TFileHeader*>(header_read);
    if (file_header_ptr == nullptr) {
        throw(std::runtime_error("Failed to cast\n"));
    }
    file_header_ = *file_header_ptr;

    char info_read[sizeof(TInfoHeader)];
    input.read(info_read, sizeof(TInfoHeader));
    TInfoHeader* file_info_ptr = reinterpret_cast<TInfoHeader*>(info_read);
    if (file_info_ptr == nullptr) {
        throw(std::runtime_error("Failed to cast\n"));
    }
    info_header_ = *file_info_ptr;

    width_ = info_header_.width;
    height_ = info_header_.height;
    offset_ = (PIXELS_ALIGNMENT - width_ * static_cast<int32_t>(PIXEL_SIZE) % PIXELS_ALIGNMENT) % PIXELS_ALIGNMENT;
    image_.resize(height_, std::vector<Pixel>(width_));

    for (size_t row = 0; row < height_; ++row) {
        for (size_t pixel = 0; pixel < width_; ++pixel) {
            char colors[PIXEL_SIZE];
            input.read(colors, PIXEL_SIZE);
            image_[row][pixel].R = static_cast<uint8_t>(colors[2]);
            image_[row][pixel].G = static_cast<uint8_t>(colors[1]);
            image_[row][pixel].B = static_cast<uint8_t>(colors[0]);
        }
        input.ignore(offset_);
    }
    input.close();
}

void Image::Write(const std::string& output_path) {

    std::ofstream output(output_path, std::ios::binary);

    if (!output.is_open()) {
        throw(std::runtime_error("Failed to create " + output_path));
    }

    char* output_header_ptr = reinterpret_cast<char*>(&file_header_);

    if (output_header_ptr == nullptr) {
        throw(std::runtime_error("Failed to cast\n"));
    }

    output.write(output_header_ptr, sizeof(TFileHeader));

    char* output_info_ptr = reinterpret_cast<char*>(&info_header_);

    if (output_info_ptr == nullptr) {
        throw(std::runtime_error("Failed to cast\n"));
    }

    output.write(output_info_ptr, sizeof(TInfoHeader));

    for (size_t row = 0; row < height_; ++row) {
        for (size_t pixel = 0; pixel < width_; ++pixel) {
            uint8_t red_channel = image_[row][pixel].R;
            uint8_t green_channel = image_[row][pixel].G;
            uint8_t blue_channel = image_[row][pixel].B;
            uint8_t color[] = {blue_channel, green_channel, red_channel};
            char* rgb_ptr = reinterpret_cast<char*>(color);
            if (rgb_ptr == nullptr) {
                throw(std::runtime_error("Failed to cast\n"));
            }
            output.write(rgb_ptr, PIXEL_SIZE);
        }
        output.write(reinterpret_cast<char*>(pad_), offset_);
    }
    output.close();
}

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cerr << "not enough arguments\n";
        return 2;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    std::vector<TParams> arguments;

    for (int i = 3; i < argc; ++i) {
        std::string filter = argv[i];
        if (filter == "-crop") {
            if (i + 2 >= argc) {
                std::cerr << "not enough arguments for -crop\n";
                return 2;
            }
            arguments.emplace_back(TParams{
                .Filter = EFilterType::Crop,
                .Param1 = std::stoi(argv[i + 1]),
                .Param2 = std::stoi(argv[i + 2]),
            });
        } else if (filter == "-gs") {
            arguments.emplace_back(TParams{
                .Filter = EFilterType::Grayscale,
            });
        } else if (filter == "-sepia") {
            arguments.emplace_back(TParams{
                .Filter = EFilterType::Sepia,
            });
        } else if (filter == "-neg") {
            arguments.emplace_back(TParams{
                .Filter = EFilterType::Negative,
            });
        } else if (filter == "-sharp") {
            arguments.emplace_back(TParams{
                .Filter = EFilterType::Sharpening,
            });
        } else if (filter == "-edge") {
            if (i + 1 >= argc) {
                std::cerr << "not enough arguments for -edge\n";
                return 2;
            }
            arguments.emplace_back(TParams{
                .Filter = EFilterType::EdgeDetection,
                .Param3 = std::stof(argv[i + 1]),
            });
        } else if (filter == "-blur") {
            if (i + 1 >= argc) {
                std::cerr << "not enough arguments for -blur\n";
                return 2;
            }
            arguments.emplace_back(TParams{
                .Filter = EFilterType::GaussianBlur,
                .Param3 = std::stof(argv[i + 1]),
            });
        } else if (filter == "-cr") {
            if (i + 1 >= argc) {
                std::cerr << "not enough arguments for -cr\n";
                return 2;
            }
            arguments.emplace_back(TParams{
                .Filter = EFilterType::Contrast,
                .Param3 = std::stof(argv[i + 1]),
            });
        } else if (filter == "-vintage") {
            arguments.emplace_back(TParams{
                .Filter = EFilterType::Contrast,
                .Param3 = VINTAGECOEF,
            });
            arguments.emplace_back(TParams{
                .Filter = EFilterType::Sepia,
            });
        }
    }

    Image curr_image;

    try {
        curr_image.Read(input_file);
    } catch (std::runtime_error& e) {
        std::cerr << e.what();
        return 2;
    }

    for (auto filter : arguments) {
        if (filter.Filter == EFilterType::Crop) {
            curr_image.Crop(filter.Param1, filter.Param2);
        } else if (filter.Filter == EFilterType::Grayscale) {
            GrayscaleFilter grayscale;
            grayscale.Process(curr_image);
        } else if (filter.Filter == EFilterType::Sepia) {
            GrayscaleFilter grayscale;
            grayscale.Process(curr_image);
            SepiaFilter sepia;
            sepia.Process(curr_image);
        } else if (filter.Filter == EFilterType::Negative) {
            NegativeFilter negative;
            negative.Process(curr_image);
        } else if (filter.Filter == EFilterType::Sharpening) {
            SharpeningFilter sharpening;
            sharpening.Process(curr_image);
        } else if (filter.Filter == EFilterType::EdgeDetection) {
            EdgeDetectionFilter edge_detection;
            edge_detection.SetThreshold(filter.Param3);
            edge_detection.Process(curr_image);
        } else if (filter.Filter == EFilterType::GaussianBlur) {
            GaussianBlurFilter gaussian_blur;
            gaussian_blur.SetSigma(filter.Param3);
            gaussian_blur.Process(curr_image);
        } else if (filter.Filter == EFilterType::Contrast) {
            ContrastFilter contrast;
            contrast.SetCoef(filter.Param3);
            contrast.Process(curr_image);
        }
    }

    try {
        curr_image.Write(output_file);
    } catch (std::runtime_error& e) {
        std::cerr << e.what();
        return 2;
    }

    return 0;
}
