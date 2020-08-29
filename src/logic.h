#ifndef _LOGIC_H
#define _LOGIC_H



extern unsigned long logic_lastrun;
extern int logic_localSunrise;
extern int logic_localSunset;


bool logic_setup();
void logic_loop();
void logic_trigger_recalc();




#endif