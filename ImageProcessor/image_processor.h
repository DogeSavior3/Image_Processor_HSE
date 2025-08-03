#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

const int32_t PIXELS_ALIGNMENT = 4;
const int32_t PIXEL_SIZE = 3;
const float BYTEMAXIMUMVALUEFL = 255;
const uint8_t BYTEMAXIMUMVALUE = 255;
const float VINTAGECOEF = 1.2;

struct Pixel {
    uint8_t R;
    uint8_t G;
    uint8_t B;

    void SetColorFl(float& r, float& g, float& b);
    void SetColor(uint8_t& r, uint8_t& g, uint8_t& b);
    void SetColor32(int32_t& r, int32_t& g, int32_t& b);
};

enum class EFilterType {
    Crop,
    Grayscale,
    Sepia,
    Negative,
    Contrast,
    Sharpening,
    EdgeDetection,
    GaussianBlur,
};

struct TParams {
    EFilterType Filter;
    int32_t Param1;
    int32_t Param2;
    float Param3;
};

#pragma pack(push, 1)

struct TFileHeader {
    uint16_t HeaderField;
    uint32_t Size;
    uint32_t Reserved;
    uint32_t Offset;
};

struct TInfoHeader {
    uint32_t Size; /* Header size in bytes      */
    int32_t width;
    int32_t height;       /* Width and height of image */
    uint16_t planes;      /* Number of colour planes   */
    uint16_t bits;        /* Bits per pixel            */
    uint32_t compression; /* Compression type          */
    uint32_t imagesize;   /* Image size in bytes       */
    int32_t xresolution;
    int32_t yresolution;       /* Pixels per meter          */
    uint32_t ncolours;         /* Number of colours         */
    uint32_t importantcolours; /* Important colours         */
};

#pragma pack(pop)

class Image {
public:
    Image();

    void Read(const std::string& input_path);
    void Write(const std::string& output_path);
    int32_t GetHeight() const;
    int32_t GetWidth() const;
    Pixel GetPixel(size_t row, size_t pixel);
    void SetPixelFl(size_t& row, size_t& pixel, float& r, float& g, float& b);
    void SetPixel(size_t& row, size_t& pixel, uint8_t& r, uint8_t& g, uint8_t& b);
    void SetPixel32(size_t& row, size_t& pixel, int32_t& r, int32_t& g, int32_t& b);
    void Crop(int32_t& width, int32_t& height);

private:
    int32_t width_;
    int32_t height_;
    int32_t offset_;
    TFileHeader file_header_;
    TInfoHeader info_header_;
    uint8_t pad_[3] = {0, 0, 0};
    std::vector<std::vector<Pixel>> image_;
};
