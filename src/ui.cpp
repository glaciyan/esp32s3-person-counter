#include "ui.h"
#include <sstream>

UI::UI(SSD1306Wire *disp) : display(disp)
{
}

UI::~UI()
{
}

void UI::drawUI()
{
    display->clear();
    display->setFont(ArialMT_Plain_16);
    std::stringstream ss{};
    ss << wifiDevices << " MACs";
    display->drawString(0, 0, ss.str().c_str());
    display->display();
}
