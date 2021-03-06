#include "logic.h"
#include "config.h"
#include "wifi.h"
#include "mqtt.h"
#include <TimeLib.h>
#include "dusk2dawn.h"
#include "masterScanner.h"

// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y) (((1970 + (Y)) > 0) && !((1970 + (Y)) % 4) && (((1970 + (Y)) % 100) || !((1970 + (Y)) % 400)))
static const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

unsigned long next_dd_calc;
unsigned long logic_lastrun;
int logic_localSunrise;
int logic_localSunset;

bool logic_setup()
{
    next_dd_calc = 0;
    logic_lastrun = 0;
    return true;
}

void logic_trigger_recalc()
{
    next_dd_calc = 0;
}

struct SwitchTimes
{
    unsigned long baseStartTime;
    unsigned long baseEndTime;
    unsigned long startTime;
    unsigned long endTime;
    unsigned int weekDayStartTime;
    unsigned int weekDayEndTime;
};

int makeSwitchMinutes(String time, String type)
{
    int output = 0;

    int startH, startM;
    if (type == "t")
    {
        sscanf(time.c_str(), "%2d:%2d", &startH, &startM);
        output = startM + startH * 60;
    }

    if (type == "r")
    {
        startM = atoi(time.c_str());
        output = logic_localSunrise + startM;
    }

    if (type == "s")
    {
        startM = atoi(time.c_str());
        output = logic_localSunset + startM;
    }

    return output;
}

SwitchTimes makeSwitchTimes(uint8_t Year, uint8_t Month, uint8_t Day, AutoSwitch &sw, unsigned int last_logic)
{
    SwitchTimes output = SwitchTimes();

    int i;

    //the seconds of today
    uint32_t seconds;

    // seconds from 1970 till 1 jan 00:00:00 of the given year
    seconds = Year * (SECS_PER_DAY * 365);
    for (i = 0; i < Year; i++)
    {
        if (LEAP_YEAR(i))
        {
            seconds += SECS_PER_DAY; // add extra days for leap years
        }
    }

    // add days for this year, months start from 1
    for (i = 1; i < Month; i++)
    {
        if ((i == 2) && LEAP_YEAR(Year))
        {
            seconds += SECS_PER_DAY * 29;
        }
        else
        {
            seconds += SECS_PER_DAY * monthDays[i - 1]; //monthDay array starts from 0
        }
    }
    seconds += (Day - 1) * SECS_PER_DAY;

    //compute start and end time depending on switch type
    int startM = makeSwitchMinutes(sw.start, sw.sT);
    int endM = makeSwitchMinutes(sw.end, sw.eT);

    output.baseStartTime = seconds;
    output.baseStartTime += startM * SECS_PER_MIN;
    output.weekDayStartTime = dayOfWeek(output.baseStartTime) - 1;

    output.baseEndTime = seconds;
    output.baseEndTime += endM * SECS_PER_MIN;
    output.weekDayEndTime = output.weekDayStartTime;

    if (output.baseEndTime < output.baseStartTime)
    {
        output.baseEndTime += SECS_PER_DAY;
        if (output.weekDayEndTime > 0)
        {
            output.weekDayEndTime--;
        }
        else
        {
            output.weekDayEndTime = 6;
        }
    }

    //use the basic startTime to randomize seed
    randomSeed(output.baseStartTime);

    output.startTime = output.baseStartTime;
    output.startTime += random(-60 * sw.sD, 60 * sw.sD);
    output.endTime = output.baseEndTime;
    output.endTime += random(-60 * sw.eD, 60 * sw.eD);

    while (output.endTime < output.startTime)
    {
        output.startTime = output.baseStartTime;
        output.startTime += random(-60 * sw.sD, 60 * sw.sD);
        output.endTime = output.baseEndTime;
        output.endTime += random(-60 * sw.eD, 60 * sw.eD);
    }

    if (sw.irreg)
    {
        //compute the first irregular switching
        unsigned long newSwitchOn = output.startTime;
        unsigned long newSwitchOff = output.startTime;
        unsigned long nextSwitchOn = output.startTime;
        newSwitchOff += random(60 * sw.onrange[0], 60 * sw.onrange[1]);
        newSwitchOff = min(newSwitchOff, output.endTime);

        //step the new switch on into the future
        nextSwitchOn = newSwitchOff;
        nextSwitchOn += random(60 * sw.offrange[0], 60 * sw.offrange[1]);

        //compute some irregular switching times until the next switch on is output of the time window
        while (nextSwitchOn < output.endTime && logic_lastrun > newSwitchOff)
        {
            //compute a new switch off time
            newSwitchOn = nextSwitchOn;
            newSwitchOff = nextSwitchOn;
            newSwitchOff += random(60 * sw.onrange[0], 60 * sw.onrange[1]);
            newSwitchOff = min(newSwitchOff, output.endTime);

            //step the new switch on into the future
            nextSwitchOn = newSwitchOff;
            nextSwitchOn += random(60 * sw.offrange[0], 60 * sw.offrange[1]);
        }

        output.startTime = newSwitchOn;
        output.endTime = newSwitchOff;
    }

    return output;
}

void logic_loop()
{

    unsigned long thisrun = timeClient.getEpochTime();
    thisrun += timeZoneOffset * SECS_PER_MIN;
    //add dst offsets

    uint8_t dayIdx = day();
    uint8_t monthIdx = month();
    uint8_t yearIdx = year() - 1970; //unusally we need unix years

    if (thisrun > next_dd_calc)
    {
        thisrun = timeClient.getEpochTime();
        thisrun += timeZoneOffset * SECS_PER_MIN;
        //add dst offsets

        dayIdx = day();
        monthIdx = month();
        yearIdx = year();

        Dusk2Dawn localSun(latitude, longitude, timeZoneOffset / 60.0 - daylightSaving);

        logic_localSunrise = localSun.sunrise(yearIdx, monthIdx, dayIdx, daylightSaving);
        Serial.print("localSunrise ");
        Serial.println(logic_localSunrise);
        logic_localSunset = localSun.sunset(yearIdx, monthIdx, dayIdx, daylightSaving);
        Serial.print("localSunset ");
        Serial.println(logic_localSunset);

        mqtt.publish(("Esp/" + wifiMAC + "/SUN").c_str(),
                     ("{\"sunrise\":" + String(logic_localSunrise) +
                      ", \"sunset\":" + String(logic_localSunset) +
                      ", \"now\":\"" + String((daylightSaving) ? (hour() - 1) % 24 : hour()) + ":" + (minute() < 10 ? "0" + String(minute()) : String(minute())) + "\"" +
                      ", \"offset\":" + String(timeZoneOffset) +
                      ", \"dst\":" + String(daylightSaving) + "}")
                         .c_str(),
                     true);

        yearIdx -= 1970; //make unix years
        next_dd_calc = thisrun + 3600ul;
    }

    for (unsigned int ci = 0; ci < configs.size(); ci++)
    {
        bool switched = false;
        //for (AutoSwitch sw : configs[ci].autoSwitch)
        for (unsigned int ai = 0; ai < configs[ci].autoSwitch.size(); ai++)
        {
            //check if switching mode suggests switching
            bool switchCheckNeed = false;
            if (configs[ci].autoSwitch[ai].mode == 0)
            {
                //if the switching mode is on
                switchCheckNeed = true;
            }
            else
            {
                bool online = masterScanner_online();

                //if the switching mode is at home and one device is online
                if (configs[ci].autoSwitch[ai].mode == 2 && online)
                {
                    switchCheckNeed = true;
                }

                //if the switching mode is away and now device is online
                if (configs[ci].autoSwitch[ai].mode == 3 && !online)
                {
                    switchCheckNeed = true;
                }
            }

            if (switchCheckNeed && configs[ci].autoSwitch[ai].start != "" && configs[ci].autoSwitch[ai].end != "")
            {

                //compute start and end times
                SwitchTimes times = makeSwitchTimes(yearIdx, monthIdx, dayIdx, configs[ci].autoSwitch[ai], logic_lastrun);

                //check if the start time enabled
                if (configs[ci].autoSwitch[ai].days[times.weekDayStartTime])
                {
                    //check if the time is right :)
                    if (times.startTime >= logic_lastrun && times.startTime < thisrun)
                    {
                        Serial.print(configs[ci].name);
                        Serial.print(" on at: ");
                        Serial.println(times.startTime);
                        configs[ci].state = true;
                        configs[ci].needSend = 15;
                        switched = true;
                    }
                }
                //check if the end time is enables
                if (configs[ci].autoSwitch[ai].days[times.weekDayEndTime])
                {
                    //check if the time is right to switch off
                    if (times.endTime >= logic_lastrun && times.endTime < thisrun)
                    {
                        Serial.print(configs[ci].name);
                        Serial.print(" off at: ");
                        Serial.println(times.endTime);
                        configs[ci].state = false;
                        configs[ci].needSend = 15;
                        switched = true;
                    }
                }
            }

            if (switched)
            {
                mqtt_publish(configs[ci]);
            }
        }
    }

    logic_lastrun = thisrun;
}
