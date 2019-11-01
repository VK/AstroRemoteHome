
#include "main.h"

#include "setup.h"
#include "radio.h"
#include "config.h"
#include "wifi.h"
#include "mqtt.h"
#include "logic.h"



DynamicJsonDocument doc(5000);

void setup()
{
  Serial.begin(115200);


  if (config_setup() &&
      radio_setup() &&
      wifi_setup() &&
      mqtt_setup() &&
      logic_setup())
  {
    Serial.println("Init Successfull");
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
  if (interval < 10 || interval % 50 == 0)
  {
    logic_loop();
  }
  delay(2);
  interval++;
}