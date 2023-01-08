
#include <SPI.h>
#include <RA8875.h>
#include "base_gauges.h"

class TFTLine : public Line
{
public:
    TFTLine(
        int16_t x1,
        int16_t y1,
        int16_t x2,
        int16_t y2,
        uint16_t color,
        int16_t currentValue) : Line(x1, y1, x2, y2, color, currentValue) {}
    virtual void Draw();
};

class TFTFont
{
public:
    RA8875tsize size;
    uint8_t scale;

    TFTFont(){};
    TFTFont(int size)
    {
        switch (size)
        {
        case 16:
            size = X16;
            scale = 0;
        case 24:
            size = X24;
            scale = 0;
        case 32:
            size = X32;
            scale = 0;
        case 24 * 2:
            size = X24;
            scale = 1;
        case 32 * 2:
            size = X32;
            scale = 1;
        case 24 * 3:
            size = X24;
            scale = 2;
        }
        size = X32;
        scale = 3;
    }
    int getCharWidth()
    {
        switch (size)
        {
        case X16:
            return 16 * (1 + scale);
        case X24:
            return 24 * (1 + scale);
        }
        return 32 * (1 + scale);
    }
    int getCharHeight()
    {
        switch (size)
        {
        case X16:
            return 16 * (1 + scale);
        case X24:
            return 24 * (1 + scale);
        }
        return 32 * (1 + scale);
    }

    int getTextLength(int len)
    {
        return getCharWidth() * len;
    }
};

class TFTGauge : public BaseGauge
{
private:
    TFTFont valueFont;
    TFTFont labelFont;
    TFTFont rangeFont;
    uint16_t textColor;

public:
    TFTGauge(GaugeConfiguration cfg);
    virtual void Draw();
    virtual Line *MakeLine(
        short int x1,
        short int y1,
        short int x2,
        short int y2,
        unsigned int color,
        short int currentValue) { return new TFTLine(x1, y1, x2, y2, color, currentValue); }

protected:
    int16_t readingWidth;
    int16_t readingHeight;
    int16_t readingX;
    int16_t readingY;
    int16_t labelWidth;
    int16_t labelHeight;
    int16_t labelX;
    int16_t labelY;
    int16_t rangeWidth;
    int16_t rangeHeight;
};
