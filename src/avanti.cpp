/*
Plugin for P-180 Avanti II for X-Plane
ilias@tselios.com - All rights reserved - 2023.

Scope to understand how work with c/c++ and produce a working plugin for X-Plane

*/

//  #define XPLM400 = 1;  // This example requires SDK4.0
#if APL
#if defined(_MACH_)
#include <Carbon/Carbon.h>
#endif
#endif
#if IBM
#include <windows.h>
#endif
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "units_conv.h"
#include "../SDK/CHeaders/XPLM/XPLMDataAccess.h"
#include "../SDK/CHeaders/XPLM/XPLMProcessing.h"
#include "../SDK/CHeaders/XPLM/XPLMPlugin.h"
#include "../SDK/CHeaders/XPLM/XPLMUtilities.h"
#include "../SDK/CHeaders/XPLM/XPLMDefs.h"

// Log buffer
char myValue_buffer[256];

//  Add dataref to DRE message
#define MSG_ADD_DATAREF 0x01000000

//  Our custom dataref
static XPLMDataRef altInMeters = NULL;
static XPLMDataRef pilotAltFeet = NULL;

//  Our custom dataref's value
float   altInMetersValue;
float   altInFeet;

// float   calc_altimeter(float elapsedMe, float elapsedSim, int counter, void* refcon );
float   RegaltInMetersInDRE(float elapsedMe, float elapsedSim, int counter, void * refcon);  //  Declare callback to register dataref

float   getAltVal(void* inRefcon);
void    setAltVal(void* inRefcon, float outValue);


PLUGIN_API int XPluginStart(
         char *        outName,
         char *        outSig,
         char *        outDesc)
{

 // Plugin Info
     strcpy(outName, "P-180 Avanti II");
     strcpy(outSig, "tselios.acft.avanti");
     strcpy(outDesc, "Aircraft Plugin for Avanti");

 //  Create our custom integer dataref
 altInMeters = XPLMRegisterDataAccessor("avanti/FM/pilotAltMeters",
                    xplmType_Float,         // The types we support
                    1,                      // Writable
                    NULL, NULL,             // Integer accessors
                    getAltVal, setAltVal,   // Float accessors
                    NULL, NULL,             // Doubles accessors
                    NULL, NULL,             // Int array accessors
                    NULL, NULL,             // Float array accessors
                    NULL, NULL,             // Raw data accessors
                    NULL, NULL);            // Refcons not used

// Find and intialize our Counter dataref
    altInMeters = XPLMFindDataRef ("avanti/FM/pilotAltMeters");
    pilotAltFeet = XPLMFindDataRef("sim/cockpit2/gauges/indicators/altitude_ft_pilot");
    altInFeet = XPLMGetDataf(pilotAltFeet);
    XPLMSetDataf(altInMeters, altInFeet * 0.3048); 


//time stamp and print to Log.txt

    time_t now = 0;
    tm *ltm = localtime(&now);

    snprintf(myValue_buffer, 256, "%d-%02d-%02d %02d:%02d:%02d [P-180 Avanti II]: Plugin loaded!\n", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    XPLMDebugString(myValue_buffer);

//register the callback
XPLMRegisterFlightLoopCallback(RegaltInMetersInDRE, 1, NULL);   // This FLCB will register our custom dataref in DRE
// XPLMRegisterFlightLoopCallback(calc_altimeter, 1, NULL);  
XPLMPluginID PluginID = XPLMFindPluginBySignature("xplanesdk.examples.DataRefEditor");

units_conv_init();

if (PluginID != XPLM_NO_PLUGIN_ID)
{
    XPLMSendMessageToPlugin(PluginID, MSG_ADD_DATAREF, (void*)"avanti/FM/pilotAltMeters");
    XPLMSendMessageToPlugin(PluginID, MSG_ADD_DATAREF, (void*)"avanti/gauges/pilot/plt_airspeed_kmh");
}
return 1;
}


PLUGIN_API void     XPluginStop(void)
{
     XPLMUnregisterDataAccessor(altInMeters);
     XPLMUnregisterFlightLoopCallback(RegaltInMetersInDRE, NULL);	 //  Don't forget to unload this callback.
//      XPLMUnregisterFlightLoopCallback(calc_altimeter,  NULL);
     
}

PLUGIN_API void XPluginDisable(void)
{
//     XPLMUnregisterFlightLoopCallback(calc_altimeter, NULL);	 //  Don't forget to unload this callback.
}

PLUGIN_API int XPluginEnable(void)
{
    return 1;
}


PLUGIN_API void XPluginReceiveMessage(XPLMPluginID    inFromWho,
                                      long             inMessage,
                                      void *           inParam)
{
}


float getAltVal(void* inRefcon)
{
    return altInMetersValue;
}

void setAltVal(void* inRefcon, float inValue)
{
    altInMetersValue = inValue;
}

 //  This single shot FLCB registers our custom dataref in DRE
 float RegaltInMetersInDRE(float elapsedMe, float elapsedSim, int counter, void * refcon)
 {
    float p_alt = XPLMGetDataf(pilotAltFeet);
    float alt_conv = (p_alt * 0.3048);
    XPLMSetDataf(altInMeters, alt_conv);
    
    units_conv_update();

     return 0.1f;

 }
