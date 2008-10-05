#include "Logger.h"
#include "Timer.h"
#include <time.h>
using namespace std;

vector<String> Logger::logStrings;
String Logger::logPath;
bool Logger::htmlFormat = false;

void Logger::initialize(const char* logfilename)
{
  if (htmlFormat)
  {
    time_t rawtime;
    tm timeinfo;
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);
    char stringBuffer[26];
    char am_pm[] = "AM";
    if (timeinfo.tm_hour > 12)
      strcpy_s(am_pm, sizeof(am_pm), "PM");
    if (timeinfo.tm_hour > 12)
      timeinfo.tm_hour -= 12;
    if (timeinfo.tm_hour == 0)
      timeinfo.tm_hour = 12;
    asctime_s(stringBuffer, 26, &timeinfo);
    sprintf_s(stringBuffer, 26, "%.19s %s", stringBuffer, am_pm);
    
    logStrings.push_back("<html>\n");
    logStrings.push_back("<head><style type=\"text/css\">\n");
    logStrings.push_back("table { border: 1px solid rgb(100,100,100); text-align: left; border-collapse: collapse; border-spacing: 0px; width:100%;}\n");
    logStrings.push_back("th {background: rgb(140, 208, 211); text-align: center; color: rgb(63, 63, 63);}\n");
    
    logStrings.push_back("body {\n");
    logStrings.push_back("background-color: rgb(63, 63, 63);\n");
    logStrings.push_back("color: rgb(220, 220, 204);\n");
    logStrings.push_back("}\n");
    
    logStrings.push_back("tr {\n");
    logStrings.push_back("border-top:  1px solid rgb(100,100,100); \n");
    logStrings.push_back("text-indent: 10px; \n");
    logStrings.push_back("font-family: Verdana, sans-serif, Arial; \n");
    logStrings.push_back("font-weight: normal; \n");
    logStrings.push_back("font-size: 11px; \n");
    logStrings.push_back("}\n");
    
    logStrings.push_back("tr.error {\n");
    logStrings.push_back("background-color: rgb(255, 0, 255); \n");
    logStrings.push_back("color: rgb(63, 63, 63);\n");
    logStrings.push_back("font-weight: bold; \n");
    logStrings.push_back("}\n");
    
    logStrings.push_back("tr.highlight {\n");
    logStrings.push_back("background-color: rgb(50, 205, 50); \n");
    logStrings.push_back("color: rgb(63, 63, 63);\n");
    logStrings.push_back("font-weight: bold; \n");
    logStrings.push_back("}\n");
    
    logStrings.push_back("tr.warning {\n");
    logStrings.push_back("background-color: rgb(255, 255, 0); \n");
    logStrings.push_back("color: rgb(63, 63, 63);\n");
    logStrings.push_back("font-weight: bold; \n");
    logStrings.push_back("}\n");
    
    logStrings.push_back("tr.fatal {\n");
    logStrings.push_back("background-color: rgb(128, 0, 64); \n");
    logStrings.push_back("color: rgb(63, 63, 63);\n");
    logStrings.push_back("font-weight: bold; \n");
    logStrings.push_back("}\n");
    
    logStrings.push_back("</style></head>\n");
    logStrings.push_back("<html><body>\n");
    logStrings.push_back("<table cellspacing=\"0\">\n");
    logStrings.push_back(String("<tr><th>Log messages : ") + stringBuffer + "</th></tr>\n");
    
    logPath = !logfilename ? "Log.html" : logfilename;
  }
  else
    logPath = !logfilename ? "Log.txt" : logfilename;
    
  ofstream logFile(logPath);
  logFile.close();
}

void Logger::flush()
{
  if (!logPath.getLength() || !logStrings.size())
    return;
    
  ofstream logFile(logPath, ios::app);
  
  for (size_t t = 0; t < logStrings.size(); t++)
    logFile << logStrings[t];
    
  logStrings.clear();
  logFile.close();
}

void Logger::writeImmidiateInfoLog(const String &info)
{
  if (info.getLength())
  {
    if (htmlFormat)
      logStrings.push_back(String("<tr><td>") + info + "</td></tr>\n");
    else
      logStrings.push_back(String("<+>") + info + "\n");
    flush();
  }
}

void Logger::writeInfoLog(const String &info)
{
  if (htmlFormat)
    logStrings.push_back(String("<tr><td>") + info + "</td></tr>\n");
  else
    logStrings.push_back(String("<+>") + info + "\n");
  if (logStrings.size() >= 10)
    flush();
}

bool Logger::writeErrorLog(const String &info)
{
  if (info.getLength())
  {
    if (htmlFormat)
      logStrings.push_back(String("<tr class=\"error\"><td>") + info + "</td></tr>\n");
    else
      logStrings.push_back(String("<!>") + info + "\n");
    flush();
  }
  return false;
}

bool Logger::writeWarningLog(const String &info)
{
  if (info.getLength())
  {
    if (htmlFormat)
      logStrings.push_back(String("<tr class=\"warning\"><td>") + info + "</td></tr>\n");
    else
      logStrings.push_back(String("<!>") + info + "\n");
    flush();
  }
  return false;
}

bool Logger::writeHighlightLog(const String &info)
{
  if (info.getLength())
  {
    if (htmlFormat)
      logStrings.push_back(String("<tr class=\"highlight\"><td>") + info + "</td></tr>\n");
    else
      logStrings.push_back(String("<!>") + info + "\n");
    flush();
  }
  return false;
}

void Logger::writeFatalErrorLog(const String &info)
{
  if (info.getLength())
  {
    if (htmlFormat)
      logStrings.push_back(String("<tr class=\"fatal\"><td>") + info + "</td></tr>\n");
    else
      logStrings.push_back(String("<X>") + info + "\n");
      
    flush();
  }
  finalize();
  exit(1);
}

void Logger::enableHTML()
{
  htmlFormat = true;
}

void Logger::finalize()
{
  if (htmlFormat)
  {
    logStrings.push_back("</table>\n");
    logStrings.push_back("</body></html>\n");
  }
  
  flush();
}
