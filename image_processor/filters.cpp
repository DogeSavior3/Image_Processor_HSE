#include "filters.h"

void GrayscaleFilter::Process(Image& image) {
    for (size_t row = 0; row < image.GetHeight(); ++row) {
        for (size_t pixel = 0; pixel < image.GetWidth(); ++pixel) {
            const Pixel& temp_p = image.GetPixel(row, pixel);
            float gray = REDTOGRAYCOEF * static_cast<float>(temp_p.R) + GREENTOGRAYCOEF * static_cast<float>(temp_p.G) +
                         BLUETOGRAYCOEF * static_cast<float>(temp_p.B);
            image.SetPixelFl(row, pixel, gray, gray, gray);
        }
    }
}

void SepiaFilter::Process(Image& image) {
    for (size_t row = 0; row < image.GetHeight(); ++row) {
        for (size_t pixel = 0; pixel < image.GetWidth(); ++pixel) {
            const Pixel& temp_p = image.GetPixel(row, pixel);
            int32_t sepia_r = static_cast<int32_t>(temp_p.R) + (DEPTH * 2);
            int32_t sepia_g = static_cast<int32_t>(temp_p.G) - DEPTH;
            int32_t sepia_b = static_cast<int32_t>(temp_p.B) - INTENSITY * 2;
            image.SetPixel32(row, pixel, sepia_r, sepia_g, sepia_b);
        }
    }
}

void ContrastFilter::Process(Image& image) {
    for (size_t row = 0; row < image.GetHeight(); ++row) {
        for (size_t pixel = 0; pixel < image.GetWidth(); ++pixel) {
            const Pixel& temp_p = image.GetPixel(row, pixel);
            int32_t contrasted_r = static_cast<int32_t>(static_cast<float>(temp_p.R) * coef_);
            int32_t contrasted_g = static_cast<int32_t>(static_cast<float>(temp_p.G) * coef_);
            int32_t contrasted_b = static_cast<int32_t>(static_cast<float>(temp_p.B) * coef_);
            image.SetPixel32(row, pixel, contrasted_r, contrasted_g, contrasted_b);
        }
    }
}

void ContrastFilter::SetCoef(float& coef) {
    coef_ = coef;
}

void NegativeFilter::Process(Image& image) {
    for (size_t row = 0; row < image.GetHeight(); ++row) {
        for (size_t pixel = 0; pixel < image.GetWidth(); ++pixel) {
            Pixel temp_p = image.GetPixel(row, pixel);
            temp_p.R = BYTEMAXIMUMVALUE - temp_p.R;
            temp_p.G = BYTEMAXIMUMVALUE - temp_p.G;
            temp_p.B = BYTEMAXIMUMVALUE - temp_p.B;
            image.SetPixel(row, pixel, temp_p.R, temp_p.G, temp_p.B);
        }
    }
}

void MatrixFilter::SetMatrix(std::vector<int32_t>& row_1, std::vector<int32_t>& row_2, std::vector<int32_t>& row_3) {
    matrix_.emplace_back(row_1);
    matrix_.emplace_back(row_2);
    matrix_.emplace_back(row_3);
}

void MatrixFilter::MatrixProcess(Image& image) {
    matrix_image_.resize(image.GetHeight());
    for (int32_t row = 0; row < image.GetHeight(); ++row) {
        for (int32_t pixel = 0; pixel < image.GetWidth(); ++pixel) {
            size_t row1 = static_cast<size_t>(std::max(0, row - 1));
            size_t row2 = static_cast<size_t>(row);
            size_t row3 = static_cast<size_t>(std::min(image.GetHeight() - 1, row + 1));
            size_t pixel1 = static_cast<size_t>(std::max(0, pixel - 1));
            size_t pixel2 = static_cast<size_t>(pixel);
            size_t pixel3 = static_cast<size_t>(std::min(image.GetWidth() - 1, pixel + 1));

            int32_t temp_r =
                std::min(static_cast<int32_t>(BYTEMAXIMUMVALUE),
                         std::max(0, matrix_[0][0] * static_cast<int32_t>(image.GetPixel(row1, pixel1).R) +
                                         matrix_[0][1] * static_cast<int32_t>(image.GetPixel(row1, pixel2).R) +
                                         matrix_[0][2] * static_cast<int32_t>(image.GetPixel(row1, pixel3).R) +

                                         matrix_[1][0] * static_cast<int32_t>(image.GetPixel(row2, pixel1).R) +
                                         matrix_[1][1] * static_cast<int32_t>(image.GetPixel(row2, pixel2).R) +
                                         matrix_[1][2] * static_cast<int32_t>(image.GetPixel(row2, pixel3).R) +

                                         matrix_[2][0] * static_cast<int32_t>(image.GetPixel(row3, pixel1).R) +
                                         matrix_[2][1] * static_cast<int32_t>(image.GetPixel(row3, pixel2).R) +
                                         matrix_[2][2] * static_cast<int32_t>(image.GetPixel(row3, pixel3).R)));
            int32_t temp_g =
                std::min(static_cast<int32_t>(BYTEMAXIMUMVALUE),
                         std::max(0, matrix_[0][0] * static_cast<int32_t>(image.GetPixel(row1, pixel1).G) +
                                         matrix_[0][1] * static_cast<int32_t>(image.GetPixel(row1, pixel2).G) +
                                         matrix_[0][2] * static_cast<int32_t>(image.GetPixel(row1, pixel3).G) +

                                         matrix_[1][0] * static_cast<int32_t>(image.GetPixel(row2, pixel1).G) +
                                         matrix_[1][1] * static_cast<int32_t>(image.GetPixel(row2, pixel2).G) +
                                         matrix_[1][2] * static_cast<int32_t>(image.GetPixel(row2, pixel3).G) +

                                         matrix_[2][0] * static_cast<int32_t>(image.GetPixel(row3, pixel1).G) +
                                         matrix_[2][1] * static_cast<int32_t>(image.GetPixel(row3, pixel2).G) +
                                         matrix_[2][2] * static_cast<int32_t>(image.GetPixel(row3, pixel3).G)));
            int32_t temp_b =
                std::min(static_cast<int32_t>(BYTEMAXIMUMVALUE),
                         std::max(0, matrix_[0][0] * static_cast<int32_t>(image.GetPixel(row1, pixel1).B) +
                                         matrix_[0][1] * static_cast<int32_t>(image.GetPixel(row1, pixel2).B) +
                                         matrix_[0][2] * static_cast<int32_t>(image.GetPixel(row1, pixel3).B) +

                                         matrix_[1][0] * static_cast<int32_t>(image.GetPixel(row2, pixel1).B) +
                                         matrix_[1][1] * static_cast<int32_t>(image.GetPixel(row2, pixel2).B) +
                                         matrix_[1][2] * static_cast<int32_t>(image.GetPixel(row2, pixel3).B) +

                                         matrix_[2][0] * static_cast<int32_t>(image.GetPixel(row3, pixel1).B) +
                                         matrix_[2][1] * static_cast<int32_t>(image.GetPixel(row3, pixel2).B) +
                                         matrix_[2][2] * static_cast<int32_t>(image.GetPixel(row3, pixel3).B)));
            Pixel matrix_use_result;
            matrix_use_result.SetColor32(temp_r, temp_g, temp_b);
            matrix_image_[row].emplace_back(matrix_use_result);
        }
    }
}

void SharpeningFilter::Process(Image& image) {
    std::vector<int32_t> row1 = {0, -1, 0};
    std::vector<int32_t> row2 = {-1, SHARPENINGCOEF, -1};
    std::vector<int32_t> row3 = {0, -1, 0};
    SetMatrix(row1, row2, row3);
    MatrixProcess(image);
    for (size_t row = 0; row < image.GetHeight(); ++row) {
        for (size_t pixel = 0; pixel < image.GetWidth(); ++pixel) {
            image.SetPixel(row, pixel, matrix_image_[row][pixel].R, matrix_image_[row][pixel].G,
                           matrix_image_[row][pixel].B);
        }
    }
}

void EdgeDetectionFilter::SetThreshold(float& threshold) {
    threshold_ = threshold;
}

void EdgeDetectionFilter::Process(Image& image) {
    std::vector<int32_t> row1 = {0, -1, 0};
    std::vector<int32_t> row2 = {-1, EDGEDETECTIONCOEF, -1};
    std::vector<int32_t> row3 = {0, -1, 0};
    SetMatrix(row1, row2, row3);
    GrayscaleFilter grayscale;
    grayscale.Process(image);
    MatrixProcess(image);
    uint8_t white = WHITE;
    uint8_t black = BLACK;
    for (size_t row = 0; row < image.GetHeight(); ++row) {
        for (size_t pixel = 0; pixel < image.GetWidth(); ++pixel) {
            if (matrix_image_[row][pixel].R > static_cast<uint8_t>(threshold_ * BYTEMAXIMUMVALUEFL)) {
                image.SetPixel(row, pixel, white, white, white);
            } else {
                image.SetPixel(row, pixel, black, black, black);
            }
        }
    }
}

void GaussianBlurFilter::SetSigma(float& sigma) {
    sigma_ = sigma;
}

void GaussianBlurFilter::Process(Image& image) {
    for (size_t row = 1; row < image.GetHeight() - 1; ++row) {
        for (size_t pixel = 1; pixel < image.GetWidth() - 1; ++pixel) {
            int32_t r = 0;
            int32_t g = 0;
            int32_t b = 0;
            r = (image.GetPixel(row - 1, pixel - 1).R + image.GetPixel(row - 1, pixel).R +
                 image.GetPixel(row - 1, pixel + 1).R + image.GetPixel(row, pixel - 1).R +
                 image.GetPixel(row, pixel).R + image.GetPixel(row, pixel + 1).R +
                 image.GetPixel(row + 1, pixel - 1).R + image.GetPixel(row + 1, pixel).R +
                 image.GetPixel(row + 1, pixel + 1).R) /
                SMOOTHCOEF;
            g = (image.GetPixel(row - 1, pixel - 1).G + image.GetPixel(row - 1, pixel).G +
                 image.GetPixel(row - 1, pixel + 1).G + image.GetPixel(row, pixel - 1).G +
                 image.GetPixel(row, pixel).G + image.GetPixel(row, pixel + 1).G +
                 image.GetPixel(row + 1, pixel - 1).G + image.GetPixel(row + 1, pixel).G +
                 image.GetPixel(row + 1, pixel + 1).G) /
                SMOOTHCOEF;
            b = (image.GetPixel(row - 1, pixel - 1).B + image.GetPixel(row - 1, pixel).B +
                 image.GetPixel(row - 1, pixel + 1).B + image.GetPixel(row, pixel - 1).B +
                 image.GetPixel(row, pixel).B + image.GetPixel(row, pixel + 1).B +
                 image.GetPixel(row + 1, pixel - 1).B + image.GetPixel(row + 1, pixel).B +
                 image.GetPixel(row + 1, pixel + 1).B) /
                SMOOTHCOEF;
            image.SetPixel32(row, pixel, r, g, b);
        }
    }
}
