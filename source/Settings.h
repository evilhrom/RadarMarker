#pragma once

#include <CRGBA.h>

class Settings {
public: 
    static bool removeWhenNearby;
    static uint size;
    static CRGBA color;

    static void read();
};