#pragma once

#include <vector>
#include <cstdint>
#include <cstring>

#pragma pack(push, 1)

struct ColorRGB {
    uint8_t b;
    uint8_t g;
    uint8_t r;

    constexpr bool operator==(const ColorRGB& other) const {
        return (r == other.r && g == other.g && b == other.b);
    }

    constexpr ColorRGB() : r(0), g(0), b(0) { }
    constexpr ColorRGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) { }
};

struct RGB_A {
    uint8_t g;
    uint8_t b;
    uint8_t a;
    uint8_t r;

    constexpr bool operator==(const RGB_A& other) const {
        return (r == other.r && g == other.g && b == other.b && a == other.a);
    }

    constexpr bool operator!=(const RGB_A& other) const {
        return !(*this == other);
    }

    constexpr bool operator>(const RGB_A& other) const {
        return (r > other.r && g > other.g && b > other.b);
    }

    constexpr RGB_A() : r(0), g(0), b(0), a(0) { }
    constexpr RGB_A(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) { }
};

#pragma pack(pop)

class BitMap {
private:
    std::vector<uint8_t> Bits;

public:
    int Height;
    int Width;

    BitMap(int h, int w) : Height(h), Width(w), Bits(h * w * sizeof(int)) { }

    uint8_t* GetBitData() {
        return Bits.data();
    }

    uint8_t* GetBitData(int x, int y) {
        return &Bits[(x + y * Width) * sizeof(int)];
    }

    RGB_A GetPixelRGBA(int x, int y) {
        RGB_A pixel;
        std::memcpy(&pixel, &Bits[(x + y * Width) * sizeof(int)], sizeof(RGB_A));
        return pixel;
    }

    ColorRGB GetPixelRGB(int x, int y) {
        ColorRGB pixel;
        std::memcpy(&pixel, &Bits[(x + y * Width) * sizeof(int)], sizeof(ColorRGB));
        return pixel;
    }
};
