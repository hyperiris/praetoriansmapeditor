#ifndef LOGGER_H
#define LOGGER_H

#include "../Extensions/GLee.h"
#include "../Parsers/XMLParser.h"
#include "../Parsers/IOXMLObject.h"

#define deleteObject(A){ if(A){ delete   A; A = 0; } }
#define deleteArray(A) { if(A){ delete[] A; A = 0; } }

#define MAX_TEX_UNITS 8

class Logger
{
  public:
    static void writeImmidiateInfoLog(const String &info);
    static void writeFatalErrorLog(const String &logString);
    static bool writeHighlightLog(const String &info);
    static bool writeWarningLog(const String &error);
    static bool writeErrorLog(const String &error);
    static void writeInfoLog(const String &info);
    static void initialize(const char* logfilename = NULL);
    static void enableHTML();
    static void finalize();
    static void flush();
    
  private:
    static std::vector<String> logStrings;
    static String logPath;
    static bool htmlFormat;
};

#endif