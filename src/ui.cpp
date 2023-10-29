#include "ui.h"
#include <sstream>

UI::UI() : secondsTillNextUpdate(std::chrono::seconds{0})
{
}

UI::~UI()
{
}

void UI::drawDevices()
{
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_16);

    auto devices = wifiDevices + btDevices;

    {
        std::string outstr = std::to_string(devices) + (devices == 1 ? " Device" : " Devices");
        display->drawString(0, 0, outstr.c_str());
    }

    display->setFont(ArialMT_Plain_10);

    int detailOffset = 20;

    {
        std::string outstr = std::to_string(wifiDevices) + " WiFi";
        display->drawString(0, detailOffset, outstr.c_str());
    }

    {

        std::string outstr = std::to_string(btDevices) + " BT";
        display->drawString(0, 10 + detailOffset, outstr.c_str());
    }
}

void UI::drawTimer()
{
    display->setTextAlignment(TEXT_ALIGN_RIGHT);

    auto seconds = secondsTillNextUpdate.count();
    auto minutes = seconds / 60;
    seconds = seconds % 60;

    std::stringstream ss{};
    ss << minutes << ":";
    if (seconds < 10)
        ss << "0" << seconds;
    else
        ss << seconds;

    std::string outstr = ss.str();
    display->drawString(128, 0, outstr.c_str());
}

void UI::drawBattery()
{
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    std::string outstr = std::to_string(mVolts) + "mV";
    display->drawString(128, 12, outstr.c_str());
}

void UI::drawUI()
{
    ui_mutex.lock();
    display->clear();

    drawDevices();
    drawTimer();
    drawBattery();

    display->display();
    ui_mutex.unlock();
}

void UI::setWifiDevices(size_t value)
{
    ui_mutex.lock();
    wifiDevices = value;
    ui_mutex.unlock();
}

void UI::setBtDevices(size_t value)
{
    ui_mutex.lock();
    btDevices = value;
    ui_mutex.unlock();
}

void UI::setMVolts(uint32_t value)
{
    ui_mutex.lock();
    mVolts = value;
    ui_mutex.unlock();
}

void UI::setSecondsTillNextUpdate(std::chrono::seconds value)
{
    ui_mutex.lock();
    secondsTillNextUpdate = value;
    ui_mutex.unlock();
}
