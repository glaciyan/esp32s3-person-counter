#include <Arduino.h>
#include <string>
#include <sstream>
#include <cstdio>
#include "heltec.h"
#include <RadioLib.h>
#include "esp_wifi.h"
#include <unordered_map>
#include <ctime>
#include <mutex>
#include <thread>
#include "esp_pthread.h"
#include "ui.h"

#define EXAMPLE_ESP_WIFI_SSID "esp32-wifi"
#define EXAMPLE_ESP_WIFI_PASS "AB54ADADNdfry832372DBNAORuBmacbe"
#define EXAMPLE_ESP_WIFI_CHANNEL 5
#define EXAMPLE_MAX_STA_CONN 6

const auto CollectionTime = std::chrono::minutes(5);

std::unordered_map<std::string, int> nearby_wifi_devices{};
std::mutex nearby_wifi_devices_mutex;

UI ui;

bool myesp_error_check(esp_err_t err)
{
  if (err != ESP_OK)
  {
    Serial.println(esp_err_to_name(err));
    return false;
  }

  return true;
}

String mac_to_string(uint8_t mac[])
{
  String out = "";
  out = out + mac[0] + ":" + mac[1] + ":" + mac[2] + ":" + mac[3] + ":" + mac[4] + ":" + mac[5];
  return out;
}

std::string mac_to_stdstring(uint8_t mac[])
{
  std::stringstream out{};
  out << mac[0] << ":" << mac[1] << ":" << mac[2] << ":" << mac[3] << ":" << mac[4] << ":" << mac[5];
  return out.str();
}

void processWifiDevices()
{
  while (true)
  {
    nearby_wifi_devices_mutex.lock();
    size_t devices = nearby_wifi_devices.size();
    nearby_wifi_devices.clear();
    nearby_wifi_devices_mutex.unlock();

    Serial.print("Devices: ");
    Serial.println(devices);
    std::this_thread::sleep_for(CollectionTime);
  }
}

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  if (event_id == WIFI_EVENT_AP_PROBEREQRECVED)
  {
    wifi_event_ap_probe_req_rx_t *event = (wifi_event_ap_probe_req_rx_t *)event_data;
    String message = "Probe - Strenght: ";
    message = message + event->rssi + " MAC: " + mac_to_string(event->mac);
    Serial.println(message.begin());

    nearby_wifi_devices_mutex.lock();
    nearby_wifi_devices.insert({mac_to_stdstring(event->mac), event->rssi});
    nearby_wifi_devices_mutex.unlock();
  }
}

std::thread p1;

void setup()
{
  // init
  Serial.begin(115200);
  Serial.println("Hello!");

  // init display
  Heltec.display->init();
  Heltec.display->setFont(ArialMT_Plain_10);
  ui = UI{Heltec.display};
  ui.drawUI();

  // wifi
  if (!myesp_error_check(esp_netif_init()))
    return;

  if (!myesp_error_check(esp_event_loop_create_default()))
    return;

  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  if (!myesp_error_check(esp_wifi_init(&wifi_init_config)))
    return;

  if (!myesp_error_check(esp_wifi_set_event_mask(0)))
    return;

  if (!myesp_error_check(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, nullptr, nullptr)))
    return;

  wifi_config_t ap_config = {};
  strcpy((char *)ap_config.ap.ssid, EXAMPLE_ESP_WIFI_SSID);
  strcpy((char *)ap_config.ap.password, EXAMPLE_ESP_WIFI_PASS);
  ap_config.ap.ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID);
  ap_config.ap.channel = EXAMPLE_ESP_WIFI_CHANNEL;
  ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
  ap_config.ap.max_connection = EXAMPLE_MAX_STA_CONN;
  // ap_config.ap.ssid_hidden = 1;
  wifi_config_t wifi_config{ap_config};

  if (!myesp_error_check(esp_wifi_set_mode(WIFI_MODE_AP)))
    return;
  if (!myesp_error_check(esp_wifi_set_config(WIFI_IF_AP, &wifi_config)))
    return;
  if (!myesp_error_check(esp_wifi_start()))
    return;

  String message{"wifi_init_softap finished."};
  message = message + " SSID: " + EXAMPLE_ESP_WIFI_SSID;
  Serial.println(message);

  auto cfg = esp_pthread_get_default_config();
  esp_pthread_set_cfg(&cfg);
  p1 = std::thread{processWifiDevices};
}

int num = 0;

void loop()
{
}