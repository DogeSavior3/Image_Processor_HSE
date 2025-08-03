#pragma once
#include "image_processor.h"
#include <cmath>
const float REDTOGRAYCOEF = 0.299;
const float GREENTOGRAYCOEF = 0.587;
const float BLUETOGRAYCOEF = 0.114;
const int32_t SHARPENINGCOEF = 5;
const int32_t EDGEDETECTIONCOEF = 4;
const uint8_t WHITE = 255;
const uint8_t BLACK = 0;
const int32_t DEPTH = 17;
const int32_t INTENSITY = 23;
const int32_t SMOOTHCOEF = 9;

class AbstractFilter {
public:
    virtual void Process(Image& image) = 0;
};

class GrayscaleFilter : public AbstractFilter {
public:
    void Process(Image& image) override;
};

class SepiaFilter : public AbstractFilter {
public:
    void Process(Image& image) override;
};

class ContrastFilter : public AbstractFilter {
public:
    void Process(Image& image) override;
    void SetCoef(float& coef);
    float coef_;
};

class NegativeFilter : public AbstractFilter {
public:
    void Process(Image& image) override;
};

class MatrixFilter : public AbstractFilter {
public:
    std::vector<std::vector<Pixel>> matrix_image_;

    void SetMatrix(std::vector<int32_t>& row_1, std::vector<int32_t>& row_2, std::vector<int32_t>& row_3);
    void MatrixProcess(Image& image);

private:
    std::vector<std::vector<int32_t>> matrix_;
};

class SharpeningFilter : MatrixFilter {
public:
    void Process(Image& image) override;
};

class EdgeDetectionFilter : MatrixFilter {
public:
    void Process(Image& image) override;
    void SetThreshold(float& threshold);
    float threshold_;
};

class GaussianBlurFilter : MatrixFilter {
public:
    void Process(Image& image) override;
    void SetSigma(float& sigma);
    float sigma_;
};
