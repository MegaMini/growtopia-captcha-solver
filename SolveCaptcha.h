#include "RTTEX.h"
#include "variant2.hpp"
#include "rtparam.hpp"
#include <urlmon.h>
#include <vector>
#include <chrono>

#pragma comment(lib, "Urlmon.lib")

using namespace std::chrono;

struct Vector2 {
    float x, y;
    Vector2(int x, int y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
};

float GetAnswer(RTTEX& Image) {
    const RGB_A WhiteColor(255, 255, 255, 255);
    const int threshold = 50;

    for (int Y = 0; Y < Image.Info.RealHeight; ++Y) {
        for (int X = 0; X < Image.Info.RealWidth; ++X) {
            if (Image.GetMap()->GetPixelRGBA(X, Y) == WhiteColor) {
                int WhitePixelCount = 1;
                for (int i = 1; i < 60 && X + i < Image.Info.Width; ++i) {
                    if (Image.GetMap()->GetPixelRGBA(X + i, Y) == WhiteColor) {
                        if (++WhitePixelCount > threshold) {
                            return static_cast<float>(X) / Image.Info.Width;
                        }
                    }
                }
            }
        }
    }
    return 0.0f;
}

void MakeBrighter(RGB_A& toChange) {
    auto Brighten = [](uint8_t& channel) {
        channel = std::min(255, static_cast<int>(channel + ((-166.f * channel / 255.f) + 166.f)));
    };
    Brighten(toChange.r);
    Brighten(toChange.g);
    Brighten(toChange.b);
}

BitMap* MakeSquare(RTTEX& FROM, const Vector2& Size = Vector2(50, 50), bool makeBright = true) {
    auto* Map = new BitMap(Size.x, Size.y);
    int* Bits = Map->GetBitData();
    BitMap* FileMap = FROM.GetMap();
    int xStart = 24;
    int yStart = 0;

    for (int y = 0; y < FROM.Info.RealHeight && !yStart; ++y) {
        for (int x = 0; x < FROM.Info.RealWidth; ++x) {
            if (FileMap->GetPixelRGBA(x, y).r == 255) {
                yStart = 20 + y;
                break;
            }
        }
    }

    for (int Y_POS = yStart; Y_POS < yStart + 50; ++Y_POS) {
        for (int X_POS = xStart; X_POS < xStart + 50; ++X_POS) {
            RGB_A rgba = FileMap->GetPixelRGBA(X_POS, Y_POS);
            if (makeBright)
                MakeBrighter(rgba);
            *Bits++ = *reinterpret_cast<int*>(&rgba);
        }
    }

    return Map;
}

bool isNearEquation(const RGB_A& Original, const RGB_A& Color) {
    return std::abs(Original.r - Color.r) < 3 &&
           std::abs(Original.g - Color.g) < 3 &&
           std::abs(Original.b - Color.b) < 3;
}

float AnswerByEquation(RTTEX& image, BitMap* Square) {
    if (!Square) return 0.0f;

    BitMap* ImageMap = image.GetMap();

    for (int Y = 0; Y < image.Info.RealHeight; ++Y) {
        for (int X = 0; X < image.Info.RealWidth; ++X) {
            if (isNearEquation(ImageMap->GetPixelRGBA(X, Y), Square->GetPixelRGBA(12, 12))) {
                int MatchCount = 0;
                bool matched = true;
                for (int y = 12; y < Square->Height - 12 && matched; ++y) {
                    for (int x = 12; x < Square->Width - 12; ++x) {
                        if (!isNearEquation(ImageMap->GetPixelRGBA(X + x - 12, Y + y - 12), Square->GetPixelRGBA(x, y))) {
                            matched = false;
                            break;
                        }
                        if (++MatchCount > 200) {
                            printf("[CAPTCHA]: Solved Piece Location (%d, %d)\n", X, Y);
                            return static_cast<float>(X - 28) / 512;
                        }
                    }
                }
            }
        }
    }
    return 0.0f;
}

std::string SolveCaptcha(variant_t& variant) {
    auto start = high_resolution_clock::now();

    rtvar parse = rtvar::parse(variant.get_string());
    std::vector<std::string> Values = parse.find("add_puzzle_captcha")->m_values;
    std::string DownloadLink = "https://" + Values[2] + "/" + Values[0];
    std::string PuzzlePieceLink = "https://" + Values[2] + "/" + Values[1];
    std::string FilePath = "captcha" + Values[3] + ".rttex";
    std::string PuzzleFilePath = "piece" + Values[3] + ".rttex";

    char Dir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, Dir);

    std::string ExactFilePath = std::string(Dir) + "\\" + FilePath;
    std::string ExactPuzzlePath = std::string(Dir) + "\\" + PuzzleFilePath;

    printf("[CAPTCHA]: Downloading From: %s\n", DownloadLink.c_str());

    if (SUCCEEDED(URLDownloadToFile(NULL, DownloadLink.c_str(), ExactFilePath.c_str(), 0, NULL))) {
        printf("[CAPTCHA]: Download succeeded.\n");
        RTTEX Image(FilePath.c_str());

        float Answer = GetAnswer(Image);

        if (Answer == 0.0f) {
            printf("[CAPTCHA]: Downloading From: %s\n", PuzzlePieceLink.c_str());
            if (SUCCEEDED(URLDownloadToFile(NULL, PuzzlePieceLink.c_str(), ExactPuzzlePath.c_str(), 0, NULL))) {
                printf("[CAPTCHA]: Downloaded Puzzle Piece.\n");
                auto m_start = high_resolution_clock::now();
                RTTEX PuzzlePiece(PuzzleFilePath.c_str());
                BitMap* Square = MakeSquare(PuzzlePiece);
                Answer = AnswerByEquation(Image, Square);
                auto m_end = high_resolution_clock::now();
                if (Answer != 0.0f) {
                    printf("[CAPTCHA]: Matched Pattern (in %.2f milliseconds)!\n", duration<double, std::milli>(m_end - m_start).count());
                }
                delete Square;
            }
        }

        std::remove(FilePath.c_str());
        std::remove(PuzzleFilePath.c_str());

        auto end = high_resolution_clock::now();

        printf("[CAPTCHA]: Downloaded & Solved in %.2f milliseconds.\n", duration<double, std::milli>(end - start).count());
        return "action|dialog_return\ndialog_name|puzzle_captcha_submit\ncaptcha_answer|" + std::to_string(Answer) + "|CaptchaID|" + Values[3];
    }

    printf("[CAPTCHA]: File couldn't download.\n");
    std::remove(FilePath.c_str());
    return "";
}
