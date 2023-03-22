#ifndef _NETTHERM_H_
#define _NETTHERM_H_

#include <Arduino.h>

const uint8_t temp_pin_1 = GPIO_NUM_32;
const uint8_t temp_pin_2 = GPIO_NUM_33;

void setup_wifi();
const char *get_wifi_status(int);
void post_temp(const char * sensor_id, float temp);

const char *wifi_ssid = "";
const char *wifi_pw = "";
const char *hostname = "";
const char *sink_server = "http://10.99.99.105:80/temp";
const char *json_format_str = "{\"sensor\":\"%s\",\"value\":%.2f}";

const char *sensor_1 = "up";
const char *sensor_2 = "down";

const uint8_t window_size = 5;

#ifdef AT503
#undef NTC10K
const float r2 = 56000;
const float c1 = 0.8933682248e-03;
const float c2 = 2.139902159e-04;
const float c3 = 1.147294320e-07;
#endif

#ifdef NTC10K
const float r2 = 10000;
const float c1 = 1.009249522e-03;
const float c2 = 2.378405444e-04;
const float c3 = 2.019202697e-07;
#endif

#endif