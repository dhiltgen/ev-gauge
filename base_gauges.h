
#ifdef __AVR__
#include <SPI.h>
#define puts Serial.print
// TODO build conversion algo so we only store 16bit and convert to 24bit
static const uint16_t BLACK = 0x0000;
static const uint16_t GREY = 0xBDF7;
static const uint16_t DARK_GREY = 0x7BEF;
static const uint16_t BLUE = 0x001F;
static const uint16_t RED = 0xF800;
static const uint16_t YELLOW = 0xFFE0;
static const uint16_t WHITE = 0xFFFF;
#else
#define puts puts
#define uint16_t u_int16_t
// TODO - The colors aren't right... (24 bits required)
static const uint16_t BLACK = 0x000000;
static const uint16_t GREY = 0xBEBEBE;
static const uint16_t DARK_GREY = 0x696969;
static const uint16_t BLUE = 0x0000FF;
static const uint16_t RED = 0xDC143C;
static const uint16_t YELLOW = 0xFFD700;
static const uint16_t WHITE = 0xFFFFFF;
#endif

typedef struct
{
    short int x;
    short int y;
    unsigned short int size;

    // Ranges - set div large enough to retain accuracy with integer math
    short int div; // Divide all values by this to get real unit currentValue
    short int minValue;
    short int maxValue;
    short int lowWarn;
    short int highWarn;

    // TODO refactor warnings to be a list, possibly with callback function when crossed
    unsigned int lowWarnColor;
    unsigned int highWarnColor;
    unsigned int tickColor;

    // Unit character for numeric display
    char unit;

    const char *label;
} GaugeConfiguration;

class Line
{
public:
    short int x1;
    short int y1;
    short int x2;
    short int y2;
    unsigned int color;
    short int currentValue;
    Line(
        short int x1,
        short int y1,
        short int x2,
        short int y2,
        unsigned int color,
        short int currentValue);

    virtual void Draw() = 0;
};

class points
{
public:
    short int x[8];
    short int y[8];
    unsigned short int len;
};

class ValueMask
{
public:
    float radians;
    points p;
    ValueMask() { radians = 0.0, p.len = 0; }
};

class BaseGauge
{
public:
    BaseGauge(GaugeConfiguration cfg);
    void UpdateValue(short int currentValue);
    virtual void Draw() = 0;
    virtual Line *MakeLine(
        short int x1,
        short int y1,
        short int x2,
        short int y2,
        unsigned int color,
        short int currentValue) = 0;
    GaugeConfiguration cfg; // TODO - should be protected

    static GaugeConfiguration *GetFullConfiguration(int *len);

protected:
    void init(); // Wrap up initialization, calls virtual functions in derived classes
    ValueMask getValueMask(float radians);

    // Current reading for the gauge
    short int currentValue;

    // General sizes and coordinates
    short int x; // Actual X after padding (upper left)
    short int y; // Actual Y after padding (upper left)
    short int centerX;
    short int centerY;

    unsigned short int size; // Actual size after padding (width and height)
    short int range;         // Range of values for visual display on gauge (max - min)
    short int thickness;     // How thick the gauge arc is
    short int pad;           // Outer edge padding and (some internal padding)
    short int innerRadius;
    short int outerRadius;
    short int innerRadiusTick;
    short int outerRadiusTick; // TODO - maybe this just gets replaced with outerRadius

    // Font information
    short int targetValueFontHeight;
    short int targetLabelFontHeight;
    short int targetRangeFontHeight;
    char readingText[64];
    char labelText[64];
    char rangeText[4][8];
    short int rangeOrigin[4][2];
    short int maxRangeTextLengthOffset;
    float startRadians = M_PI / 180.0 * 225;
    float endRadians = M_PI / 180.0 * -45;
    float currentReadingRadians;
    float lowWarnRadians;
    float highWarnRadians;

    Line *capLine;
    Line *lowWarnLine;
    Line *highWarnLine;
    Line **ticks;
    unsigned short int tickCount;

    // Mask Coordinates
    ValueMask currentReadingMask;
};
