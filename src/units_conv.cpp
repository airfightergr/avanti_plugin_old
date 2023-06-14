/*units conversion file*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 #include "XPLMPlugin.h"
 #include "XPLMDisplay.h"
 #include "XPLMGraphics.h"
 #include "XPLMProcessing.h"
 #include "XPLMDataAccess.h"
 #include "XPLMMenus.h"
 #include "XPLMUtilities.h"

#define KNOTS_TO_KMH = 1.852;

float   get_plt_speed(void* inRefcon);
void    set_plt_speed(void* inRefcon, float outValue);

float speedInKm;
float pspeed;
    
XPLMDataRef plt_airspeed_kmh = NULL;
XPLMDataRef plt_airspeed_knots = NULL;

    void units_conv_init()
    {

        plt_airspeed_kmh = XPLMRegisterDataAccessor("avanti/gauges/pilot/plt_airspeed_kmh",
                    xplmType_Float,         // The types we support
                    1,                      // Writable
                    NULL, NULL,             // Integer accessors
                    get_plt_speed, set_plt_speed,   // Float accessors
                    NULL, NULL,             // Doubles accessors
                    NULL, NULL,             // Int array accessors
                    NULL, NULL,             // Float array accessors
                    NULL, NULL,             // Raw data accessors
                    NULL, NULL);            // Refcons not used

    plt_airspeed_knots = XPLMFindDataRef("sim/cockpit2/gauges/indicators/airspeed_kts_pilot");
    plt_airspeed_kmh = XPLMFindDataRef("avanti/gauges/pilot/plt_airspeed_kmh");
    pspeed = XPLMGetDataf(plt_airspeed_knots);
    XPLMSetDataf(plt_airspeed_kmh, pspeed * 1.835);

    }

    float get_plt_speed( void* inRefcon)
    {
        return speedInKm;
    }

    void set_plt_speed(void* inRefcon, float inValue)
    {
        speedInKm = inValue;
    }

    void units_conv_update()
    {
        float p_spd = XPLMGetDataf(plt_airspeed_knots);
        float speed_conv = (p_spd * 1.835);
        XPLMSetDataf(plt_airspeed_kmh, speed_conv);
    }