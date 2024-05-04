#include "Settings.h"

#include "plugin.h"

bool Settings::removeWhenNearby = false;
bool Settings::drawMarker = false;
uint Settings::size = 4;
CRGBA Settings::color(255, 0, 0, 255);

void Settings::read() {
    plugin::config_file config(PLUGIN_PATH("RadarMarkerVC.ini"));

    removeWhenNearby = config["remove_when_nearby"].asBool(true);
    drawMarker = config["draw_marker"].asBool(false);
    size = config["size"].asInt(4);
    color = config["color"].asRGBA(CRGBA(255, 0, 0, 255));
}