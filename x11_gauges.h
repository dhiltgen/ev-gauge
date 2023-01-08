#ifndef __AVR__

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xft/Xft.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>
#include <math.h>
#include <map>
#include "base_gauges.h"

class XLine: public Line {
public:
    XLine(
        short int x1,
        short int y1,
        short int x2,
        short int y2,
        unsigned long color,
        short int currentValue) : Line(x1,y1,x2,y2,color,currentValue){}
    virtual void Draw();
};

class XGauge: public BaseGauge {
private:
    XftFont* valueFont;// TODO - these leak!
    XftFont* labelFont;
    XftFont* rangeFont; 
    XGlyphInfo extents;
    XftDraw *draw;
    XftColor textColor;
    Visual *visual;
    Colormap cmap;
public:
    XGauge(GaugeConfiguration cfg);
    virtual void Draw();
    virtual Line* MakeLine(
        short int x1,
        short int y1,
        short int x2,
        short int y2,
        unsigned int color,
        short int currentValue) {return new XLine(x1,y1,x2,y2,color,currentValue);}

protected:
    short int readingWidth;
    short int readingHeight;
    short int readingX;
    short int readingY;
    short int labelWidth;
    short int labelHeight;
    short int labelX;
    short int labelY;
    short int rangeWidth;
    short int rangeHeight;
};
#endif