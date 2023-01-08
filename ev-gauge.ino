// TODO - start with a naive impl that redraws everyting and just toggles layers
//        see what maximum refresh rate is on the Uno
//        If it's unacceptable, then refactor to just overlap the minimum set of
//        graphics.
//
//        The 800x480 display does not support layers :-(
//

#include <SPI.h>
#include <RA8875.h>

// Arduino DUE,Arduino mega2560,Arduino UNO
#define RA8875_INT 4
#define RA8875_CS 10

#define RA8875_RESET 9

#include <unistd.h>
#include <stdlib.h>
// #include <cstdio>
#include <math.h>
#include "ev-gauge.h"
#include "can.h"

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);
uint16_t fgColor = GREY;
uint16_t bgColor = BLACK;

void setup()
{
    // setActiveWindow may allow manipulating cord system (looks like default is correct though)
    Serial.begin(9600);
    // while (!Serial) {;}
    Serial.println("RA8875 Gauge start");

    tft.begin(RA8875_800x480);
    //   tft.useLayers(true);//turn on layers
    //   tft.writeTo(L1);//write colored bars to layer 1
    //   //create ONCE all colored bars
    //   // TODO - not set up yet...
    //   drawHbar(10,10,100,100);
    //   drawHbar(10,30,50,50);
    //   drawHbar(10,50,80,80);
    //   drawHbar(10,70,80,80);
    //   drawHbar(10,90,80,80);
    //   drawHbar(10,110,80,80);
    //   drawHbar(10,130,80,80);
    //   drawHbar(10,150,80,80);
    //   drawHbar(10,170,80,80);
    //   drawHbar(10,190,80,80);
    //   drawHbar(10,210,80,80);
    //   drawHbar(10,230,80,80);
    //   tft.writeTo(L2);//from this point we write on layer 2
    //   tft.layerEffect(AND);//apply AND effect between layer 1 and 2
    // TODO Toggle between LAYER1 and LAYER2

    can_setup();
}

void TFTLine::Draw()
{
    tft.drawLine(x1, y1, x2, y2, color);
}

TFTGauge::TFTGauge(GaugeConfiguration cfg) : BaseGauge(cfg)
{
    printf("In TFTGauge default ctor %s\n", cfg.label);
    init();
    valueFont = TFTFont(targetValueFontHeight);
    labelFont = TFTFont(targetLabelFontHeight);
    rangeFont = TFTFont(targetRangeFontHeight);

    readingWidth = valueFont.getCharWidth() * strlen(readingText);
    readingHeight = valueFont.getCharHeight();
    readingX = (x + size / 2) - (readingWidth / 2);
    readingY = (y + size / 2) + (readingHeight / 2);

    // Determine text label
    labelWidth = labelFont.getCharWidth() * strlen(this->cfg.label);
    labelHeight = labelFont.getCharHeight();
    labelX = (x + size / 2) - (labelWidth / 2);
    labelY = (y + size - labelWidth) + (labelHeight / 2) + pad;

    // Determine text range
    rangeWidth = rangeFont.getCharWidth() * strlen(rangeText[maxRangeTextLengthOffset]);
    rangeHeight = rangeFont.getCharHeight();
    rangeOrigin[0][0] = (x);
    rangeOrigin[0][1] = (y + size - rangeWidth / 2 + pad / 2);
    rangeOrigin[1][0] = (x);
    rangeOrigin[1][1] = (y + rangeWidth / 2);
    rangeOrigin[2][0] = (x + size - rangeWidth / 2); // XXX width's don't quite seem rght
    rangeOrigin[2][1] = (y + rangeWidth / 2);
    rangeOrigin[3][0] = (x + size - rangeWidth / 2); // XXX width's don't quite seem rght
    rangeOrigin[3][1] = (y + size - rangeWidth / 2 + pad / 2);
}

void TFTGauge::Draw()
{

    // Draw current reading
    uint16_t fillColor = 0;
    if (currentValue < cfg.lowWarn && cfg.lowWarn != cfg.minValue)
    {
        fillColor = cfg.lowWarnColor;
    }
    else if (currentValue > cfg.highWarn && cfg.highWarn != cfg.maxValue)
    {
        fillColor = cfg.highWarnColor;
    }
    else
    {
        fillColor = fgColor;
    }
    // Bounding box for debugging
    // tft.drawRect(cfg.x,cfg.y,cfg.size,cfg.size, fgColor);
    // printf("Cfg Bounds: %d,%d x %d,%d size %d\n", cfg.x, cfg.y, cfg.x+cfg.size, cfg.y+cfg.size, cfg.size);
    // printf("Effective Bounds: %d,%d x %d,%d with size %d pad %d\n", x, y, x+size, y+size, size, pad);
    // tft.drawRect(x,y,size,size, fgColor);

    tft.fillCircle(centerX, centerY, outerRadius, fillColor);
    tft.fillCircle(centerX, centerY, innerRadius, bgColor);
    points p = currentReadingMask.p;
    for (short int i = 2; i < p.len; i++)
    {
        // TODO possible optimization if we need a performance boost - jump by i+=3
        tft.fillTriangle(
            p.x[i - 2],
            p.y[i - 2],
            p.x[i - 1],
            p.y[i - 1],
            p.x[i],
            p.y[i],
            bgColor);
    }

    // Draw the outline for the gauge
    tft.drawCircle(centerX, centerY, outerRadius, cfg.tickColor);
    tft.drawCircle(centerX, centerY, innerRadius, cfg.tickColor);

    // Clear the bottom of the gauge
    tft.fillTriangle(
        cfg.x + cfg.size / 2,
        cfg.y + cfg.size / 2,
        cfg.x,
        cfg.y + cfg.size,
        cfg.x + cfg.size,
        cfg.y + cfg.size,
        bgColor);

    // Range labels first (wont overlap)
    tft.changeMode(TEXT);
    tft.setTextColor(textColor);
    tft.setFontSize(rangeFont.size);
    tft.setFontScale(rangeFont.scale);
    for (int i = 0; i < 4; i++)
    {
        tft.setCursor(rangeOrigin[i][0], rangeOrigin[i][1]);
        tft.print(rangeText[i]);
    }
    tft.changeMode(GRAPHIC);

    // Show the bounding rectangle
    // tft.drawRect(labelX,labelY-labelHeight,labelWidth,labelHeight, fgColor);

    // Clear out any prior readings
    tft.fillRect(readingX, readingY - readingHeight, readingWidth, readingHeight, bgColor);

    // Cap the gauge
    capLine->Draw();

    // Tick marks
    for (short int i = 0; i < tickCount; i++)
    {
        ticks[i]->Draw();
    }

    // Warning marks
    if (lowWarnLine)
    {
        lowWarnLine->Draw();
    }
    if (highWarnLine)
    {
        highWarnLine->Draw();
    }

    // Place text currentValue in the center
    tft.changeMode(TEXT);
    tft.setTextColor(textColor);
    tft.setFontSize(valueFont.size);
    tft.setFontScale(valueFont.scale);
    tft.setCursor(readingX, readingY);
    tft.print(readingText);
}

void loop()
{
    tft.fillScreen(BLACK);
    int len;
    GaugeConfiguration *cfgs = BaseGauge::GetFullConfiguration(&len);
    printf("Got %d cfgs\n", len);
    TFTGauge *gauges[len];
    for (int i = 0; i < len; i++)
    {
        gauges[i] = new TFTGauge(cfgs[i]);
    }
    // TODO - should actually gather REAL data from CAN bus
    for (float v = 0; v <= 1; v += 0.01)
    {
        for (short int i = 0; i < sizeof(gauges) / sizeof(gauges[0]); i++)
        {
            gauges[i]->UpdateValue((short int)(gauges[i]->cfg.maxValue - gauges[i]->cfg.minValue) * v + gauges[i]->cfg.minValue);
            gauges[i]->Draw();
        }
        delay(10);
    }
    can_loop();
}
