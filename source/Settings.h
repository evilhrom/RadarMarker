#pragma once

#include <CRGBA.h>

class Settings {
public: 
    static bool removeWhenNearby;
    static bool drawMarker;
    static uint size;
    static CRGBA color;

    static void read();
};