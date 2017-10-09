#include <Arduino.h>
#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>

//  This object parses received characters
//  into the gps.fix() data structure
static NMEAGPS gps;

//  Define a set of GPS fix information.  It will
//  hold on to the various pieces as they are received from
//  an RMC sentence.  It can be used anywhere in your sketch.
static gps_fix fix;

//  This function gets called about once per second, during the GPS
//  quiet time.  It's the best place to do anything that might take
//  a while: print a bunch of things, write to SD, send an SMS, etc.
//
//  By doing the "hard" work during the quiet time, the CPU can get back to
//  reading the GPS chars as they come in, so that no chars are lost.
static void doSomeWork()
{
  trace_all(DEBUG_PORT, gps, fix);
}

//  This is the main GPS parsing loop.
static void GPSloop()
{
  while (gps.available(gpsPort))
  {
    fix = gps.read();
    doSomeWork();
  }
}

void setup()
{
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print(F("NMEA.INO: started\n"));
  DEBUG_PORT.print(F("  fix object size = "));
  DEBUG_PORT.println(sizeof(gps.fix()));
  DEBUG_PORT.print(F("  gps object size = "));
  DEBUG_PORT.println(sizeof(gps));
  DEBUG_PORT.println(F("Looking for GPS device on " GPS_PORT_NAME));

#ifndef NMEAGPS_RECOGNIZE_ALL
#error You must define NMEAGPS_RECOGNIZE_ALL in NMEAGPS_cfg.h!
#endif

#ifdef NMEAGPS_INTERRUPT_PROCESSING
#error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

#if !defined(NMEAGPS_PARSE_GGA) & !defined(NMEAGPS_PARSE_GLL) & \
    !defined(NMEAGPS_PARSE_GSA) & !defined(NMEAGPS_PARSE_GSV) & \
    !defined(NMEAGPS_PARSE_RMC) & !defined(NMEAGPS_PARSE_VTG) & \
    !defined(NMEAGPS_PARSE_ZDA) & !defined(NMEAGPS_PARSE_GST)

  DEBUG_PORT.println(F("\nWARNING: No NMEA sentences are enabled: no fix data will be displayed."));

#else
  if (gps.merging == NMEAGPS::NO_MERGING)
  {
    DEBUG_PORT.print(F("\nWARNING: displaying data from "));
    DEBUG_PORT.print(gps.string_for(LAST_SENTENCE_IN_INTERVAL));
    DEBUG_PORT.print(F(" sentences ONLY, and only if "));
    DEBUG_PORT.print(gps.string_for(LAST_SENTENCE_IN_INTERVAL));
    DEBUG_PORT.println(F(" is enabled.\n"
                         "  Other sentences may be parsed, but their data will not be displayed."));
  }
#endif

  DEBUG_PORT.print(F("\nGPS quiet time is assumed to begin after a "));
  DEBUG_PORT.print(gps.string_for(LAST_SENTENCE_IN_INTERVAL));
  DEBUG_PORT.println(F(" sentence is received.\n"
                       "  You should confirm this with NMEAorder.ino\n"));

  trace_header(DEBUG_PORT);
  DEBUG_PORT.flush();

  gpsPort.begin(9600);
}

void loop()
{
  GPSloop();
}