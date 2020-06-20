
#include "main.h"

#include "setup.h"
#include "radio.h"
#include "config.h"
#include "wifi.h"
#include "mqtt.h"
#include "logic.h"
#include "masterScanner.h"

DynamicJsonDocument doc(3000);

void setup()
{
  Serial.begin(115200);

  wifi_next_ota_check = 0;
  wifi_next_timezone_update = 0;

  if (config_setup() &&
      radio_setup() &&
      wifi_setup() &&
      mqtt_setup() &&
      masterScanner_setup() &&
      logic_setup())
  {
    Serial.println("Init Successfull");
    update_config();
  }
  else
  {
    Serial.println("Init Setup Acces Point");
    run_setup();
  }
}

int interval = 0;
void loop()
{
  radio_loop();
  wifi_loop();
  masterScanner_loop();
  if (interval < 10 || interval % 50 == 0)
  {
    logic_loop();
  }
  delay(2);
  interval++;
}