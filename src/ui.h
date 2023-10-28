#pragma once

#include <heltec.h>

class UI {
private:
    SSD1306Wire* display;
    int32_t wifiDevices = 0;

public:
    UI(SSD1306Wire* disp);
    ~UI();

    void drawUI();
};