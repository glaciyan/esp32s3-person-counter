#pragma once

#include <heltec.h>
#include <chrono>
#include <mutex>

class UI
{
private:
    std::mutex ui_mutex;
    void drawDevices();
    void drawTimer();
    void drawBattery();

public:
    size_t wifiDevices = 0;
    size_t btDevices = 0;
    uint32_t mVolts = 0;
    std::chrono::seconds secondsTillNextUpdate;

    SSD1306Wire *display;
    UI();
    ~UI();

    void drawUI();

    void setWifiDevices(size_t value);
    void setBtDevices(size_t value);
    void setMVolts(uint32_t value);
    void setSecondsTillNextUpdate(std::chrono::seconds value);
};