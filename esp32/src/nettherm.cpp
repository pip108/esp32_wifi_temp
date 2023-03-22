#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Ticker.h>
#include <Filter.h>

#define NTC10K
#include "nettherm.h"

char body_buf[128];

HTTPClient http;
ExponentialFilter<float> filtered_temp_in(10, 0);
ExponentialFilter<float> filtered_temp_out(10, 0);

void setup()
{
    Serial.begin(115200);

    pinMode(temp_pin_1, INPUT);
    pinMode(temp_pin_2, INPUT);

    setup_wifi();
}

uint32_t last_update;
uint16_t value_in, value_out;
float r_out, ln_out, temp_out, r_in, ln_in, temp_in;
void loop()
{
    value_in = analogRead(temp_pin_1);
    delay(1);
    value_out = analogRead(temp_pin_2);

    r_in = r2 * (4095.0 / (float)value_in - 1.0);
    r_out = r2 * (4095.0 / (float)value_out - 1.0);
    ln_in = log(r_in);
    ln_out = log(r_out);
    temp_in = (1.0 / (c1 + c2 * ln_in + c3 * ln_in * ln_in * ln_in)) - 273.15;
    temp_out = (1.0 / (c1 + c2 * ln_out + c3 * ln_out * ln_out * ln_out)) - 273.15;
    filtered_temp_in.Filter(temp_in);
    filtered_temp_out.Filter(temp_out);

    if (millis() - last_update > 1000)
    {
        last_update = millis();
        post_temp(sensor_1, filtered_temp_in.Current());
        post_temp(sensor_2, filtered_temp_out.Current());
    }

    delay(10);
}

void setup_wifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(hostname);
    WiFi.begin(wifi_ssid, wifi_pw);

    int wifi_status;
    while ((wifi_status = WiFi.status()) != WL_CONNECTED)
    {
        Serial.println(get_wifi_status(wifi_status));
        delay(1000);
    }

    Serial.print("WiFi connected, IP: ");

    Serial.println(WiFi.localIP());
}

void post_temp(const char *sensor, float value)
{
    http.begin(sink_server);
    snprintf(body_buf, sizeof(body_buf), json_format_str, sensor, value);
    http.addHeader("Content-Type", "application/json");
    Serial.println(body_buf);
    int response_code = http.POST(body_buf);
    if (response_code != 200)
    {
        Serial.print("HTTP ");
        Serial.println(response_code);
    }
    http.end();
}

const char *get_wifi_status(int status)
{
    switch (status)
    {
    case WL_IDLE_STATUS:
        return "WL_IDLE_STATUS";
    case WL_SCAN_COMPLETED:
        return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL:
        return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED:
        return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
        return "WL_CONNECTION_LOST";
    case WL_CONNECTED:
        return "WL_CONNECTED";
    case WL_DISCONNECTED:
        return "WL_DISCONNECTED";
    }
    return "UNKNOWN";
}
