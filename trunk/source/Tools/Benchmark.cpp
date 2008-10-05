#include "../Managers/MediaPathManager.h"
#include "../Tools/Logger.h"
#include "../Math/MathUtils.h"
#include "Benchmark.h"

Benchmark::Benchmark(const char* logFilePath) : FPSCounter()
{
  setLogFilePath(logFilePath);
  framesCounter  =   0.0f;
  averageFPS     =   0.0f;
  duration       =  10.0f;
  enabled        =   true;
  minFPS         = 10.0e5;
  maxFPS         =   0.0f;
}

bool Benchmark::loadXMLFile(const char* xmlPath)
{
  const char *verified = MediaPathManager::lookUpMediaPath(xmlPath);
  
  if (!verified)
    return Logger::writeErrorLog(String("Couldn't locate the Benchmark XML file at <") +
                                 xmlPath + "> even with a look up");
  XMLElement  *child     = NULL;
  XMLStack     benchStack;
  
  if (!benchStack.loadXMLFile(verified))
    return Logger::writeErrorLog(String("Invalid XML file -> ") + verified);
    
  if (!(child = benchStack.getChildByName("Benchmark")))
    return Logger::writeErrorLog("Need a <Benchmark> tag in the XML file");
    
  return loadXMLSettings(child);
}

bool   Benchmark::loadXMLSettings(XMLElement *element)
{
  if (!element)
    return  Logger::writeErrorLog("NULL Benchmark node");
    
  if (element->getName() != "Benchmark")
    return Logger::writeErrorLog("Need a <Benchmark> tag in the XML Node");
    
  XMLElement  *child     = NULL,
                           *subChild  = NULL;
                           
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if (!(child = element->getChild(i)))
      continue;
      
    const String &childName = child->getName();
    
    if (childName == "logFilePath")
      setLogFilePath(child->getValuec());
      
    if (childName == "duration")
      setDuration(child->getValuef());
      
    if (childName == "enabled")
      setEnabled((child->getValue() == "true"));
  }
  
  return true;
}

bool Benchmark::exportResults()
{
  if (!logFilePath)
    return Logger::writeErrorLog("Cannot export Benchmark results -> NULL log file path");
    
  ofstream exporter(logFilePath);
  
  exporter << "<BMResults  framesCount = \"" << framesCounter << "\"\n"
  << "            elapsedTime = \"" << elapsedTime   << "\"\n"
  << "            duration    = \"" << duration      << "\"\n"
  << "            averageFPS  = \"" << averageFPS    << "\"\n"
  << "            minFPS      = \"" << minFPS        << "\"\n"
  << "            maxFPS      = \"" << maxFPS        << "\"/>\n";
  
  exporter.close();
  return true;
}

void Benchmark::setLogFilePath(const char* lFilePath)
{
  logFilePath = lFilePath ? lFilePath : logFilePath;
}

const String &Benchmark::getLogFilePath() const
{
  return logFilePath;
}

void Benchmark::markFrameEnd()
{

  if (frameStart)
  {
    frameInterval = Timer::getElapsedTimeSeconds(frameStart);
    tickCounter  += frameInterval;
    elapsedTime  += frameInterval;
    internalFPS++;
    
    if (tickCounter >= 1.0f)
    {
      fps          = internalFPS/tickCounter;
      internalFPS  = 0.0f;
      tickCounter  = 0.0f;
    }
  }
  
  framesCounter += 1.0f;
  if (elapsedTime >= duration || !enabled)
    return;
    
  if (fps && elapsedTime)
  {
    averageFPS = framesCounter/elapsedTime;
    minFPS     = (fps < minFPS) ? fps : minFPS;
    maxFPS     = (fps > maxFPS) ? fps : maxFPS;
  }
}

void   Benchmark::setEnabled(bool on)
{
  enabled = on;
}

bool   Benchmark::isEnabled()
{
  return enabled;
}

bool Benchmark::running()
{
  return enabled ? (elapsedTime < duration) : true;
}

void Benchmark::setDuration(float dur)
{
  duration = clamp(dur, 1.0f, 3600.0f);
}

float  Benchmark::getAverageFrames()
{
  return averageFPS;
}
float  Benchmark::getTotalFrames()
{
  return framesCounter;
}
float  Benchmark::getDuration()
{
  return duration;
}
float  Benchmark::getMinFPS()
{
  return minFPS;
}
float  Benchmark::getMaxFPS()
{
  return maxFPS;
}
