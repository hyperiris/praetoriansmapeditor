#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "../Parsers/XMLParser.h"
#include "FPSCounter.h"
#include "String.h"

class Benchmark : public FPSCounter
{
  private:
    String  logFilePath;
    float   framesCounter,
    averageFPS,
    duration,
    minFPS,
    maxFPS;
    bool   enabled;
    
  public:
    Benchmark(const char* logFilePath = "Results.xml");
    
    void    setLogFilePath(const char* logFilePath);
    const   String & getLogFilePath() const;
    void    setDuration(float);
    float   getDuration();
    
    bool    loadXMLFile(const char* xmlPath);
    bool    loadXMLSettings(XMLElement *element);
    bool    exportResults();
    bool    running();
    virtual void  markFrameEnd();
    
    float  getAverageFrames();
    float  getTotalFrames();
    float  getMinFPS();
    float  getMaxFPS();
    
    void   setEnabled(bool);
    bool   isEnabled();
};
#endif