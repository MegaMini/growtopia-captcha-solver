#ifndef RTTEX_H_
#define RTTEX_H_

#include <fstream>
#include <memory>
#include "BitMap.h"

struct RTTEXINFO {
    int Height;
    int Width;
    int Format;
    int RealHeight;
    int RealWidth;
    bool useAlpha;
    bool isCompressed;
    short Flags;
    int MipMapCount;
};

struct RTTEX {
private:
    std::unique_ptr<BitMap> bitMap;

public:
    RTTEXINFO Info;

    RTTEX(const char* File) {
        std::ifstream stream(File, std::ios_base::in | std::ios::binary);
        if (!stream) {
            throw std::runtime_error("Failed to open file");
        }

        stream.seekg(8, std::ios_base::beg);
        stream.read(reinterpret_cast<char*>(&Info), sizeof(Info));
        bitMap = std::make_unique<BitMap>(Info.Height, Info.Width);

        stream.seekg(88, std::ios_base::cur);
        uint8_t* Bits = reinterpret_cast<uint8_t*>(bitMap->GetBitData());

        for (int y = Info.Height - 1; y >= 0; --y) {
            for (int x = 0; x < Info.Width; ++x) {
                if (Info.useAlpha) {
                    RGB_A rgba;
                    stream.read(reinterpret_cast<char*>(&rgba), sizeof(rgba));
                    int index = (x + y * Info.Width) * sizeof(int);
                    Bits[index + 0] = rgba.b;
                    Bits[index + 1] = rgba.g;
                    Bits[index + 2] = rgba.r;
                    Bits[index + 3] = rgba.a;
                } else {
                    ColorRGB rgb;
                    stream.read(reinterpret_cast<char*>(&rgb), sizeof(rgb));
                    int index = (x + y * Info.Width) * sizeof(int);
                    Bits[index + 0] = rgb.b;
                    Bits[index + 1] = rgb.g;
                    Bits[index + 2] = rgb.r;
                    Bits[index + 3] = 0xFF; // Alpha channel to 255 (opaque)
                }
            }
        }
    }

    BitMap* GetMap() {
        return bitMap.get();
    }
};

#endif
