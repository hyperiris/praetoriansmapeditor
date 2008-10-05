/* Spline
 *
 * Copyright (C) 2003-2004, Alexander Zaprjagaev <frustum@frustum.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "Spline.h"
#include "../Parsers/XMLParser.h"
#include "../Managers/MediaPathManager.h"

Spline::Spline(const char *name,
               int   close, float tension,
               float bias, float continuity) : length(0.0)
{
  cycleLength = 5.0f;
  loadXMLOrbit(name, tension, bias, continuity);
}

bool Spline::loadXMLOrbit(const char *xmlFile, float tension,
                          float bias,          float continuity)
{
  const char*     verified   = MediaPathManager::lookUpMediaPath(xmlFile);
  XMLElement     *spline     = NULL;
  XMLStack        xmlStack;
  
  if (!verified)
    return Logger::writeErrorLog(String("Couldn't locate file -> ") + xmlFile);
    
  if (!xmlStack.loadXMLFile(verified))
    return Logger::writeErrorLog(String("Invalid XML file -> ") + xmlFile);
    
  if (!(spline = xmlStack.getChildByName("Spline")))
    return Logger::writeErrorLog("Need a Spline root in the XML file");
    
  return loadXMLOrbit(spline, tension, bias, continuity);
}

bool Spline::loadXMLOrbit(XMLElement *spline,
                          float      tension,
                          float      bias,
                          float      continuity)
{

  vector<Tuple3f>  tempVect;
  XMLElement      *child1 = NULL,
                            *child2 = NULL;
  Tuple3f          point;
  bool             close   = false;
  length = 0.0;
  
  if (spline->getName() != "Spline")
    return Logger::writeErrorLog("Need a Spline root in the XML file");
    
  if (child2 = spline->getChildByName("close"))
    close = (child2->getValue() != "false");
    
  for (size_t i = 0; i < spline->getChildrenCount(); i++)
  {
    if (!(child1 = spline->getChild(i)))
      continue;
      
    const String &token =  child1->getName();
    
    if (token == "cycle")
      setCycleLength(child1->getValuef());
      
    if (token == "Point")
    {
      point.set(0.0f, 0.0f, 0.0f);
      XMLElement::loadRX_GY_BZf(*child1, point);
      tempVect.push_back(point);
    }
  }
  
  create(tempVect, close,tension,bias,continuity);
  return true;
}

Spline::Spline(vector<Tuple3f> &points,
               int close,  float tension,
               float bias, float continuity) :length(0.0)
{
  cycleLength = 5.0f;
  create(points, close,tension,bias,continuity);
}


float Spline::getCycleLength()
{
  return cycleLength;
}

void  Spline::setCycleLength(float cycle)
{
  cycle = (cycle < 0.25f) ? 0.25f : cycle;
  cycleLength = cycle;
}

Spline::~Spline()
{
  params.clear();
}

void Spline::create(vector<Tuple3f> &points,
                    int close, float tension,
                    float bias,float continuity)
{

  params.clear();
  params.resize(points.size()*4);
  length = 0;
  
  for (size_t i = 0; i < points.size(); i++)
  {
    Tuple3f prev,
    cur,
    next;
    
    if (i == 0)
    {
      if (close)
        prev = points[points.size() - 1];
      else
        prev = points[i];
        
      cur  = points[i];
      next = points[i + 1];
      
    }
    else
      if (i == points.size() - 1)
      {
        prev = points[i - 1];
        cur = points[i];
        if (close)
          next = points[0];
        else
          next = points[i];
      }
      else
      {
        prev = points[i - 1];
        cur  = points[i];
        next = points[i + 1];
      }
    Tuple3f p0 = (cur - prev) * (1.0f + bias);
    Tuple3f p1 = (next - cur) * (1.0f - bias);
    Tuple3f r0 = (p0 + (p1 - p0) * 0.5f * (1.0f + continuity)) * (1.0f - tension);
    Tuple3f r1 = (p0 + (p1 - p0) * 0.5f * (1.0f - continuity)) * (1.0f - tension);
    params[i * 4 + 0] = cur;
    params[i * 4 + 1] = next;
    params[i * 4 + 2] = r0;
    if (i)
      params[i * 4 - 1] = r1;
    else
      params[(points.size() - 1) * 4 + 3] = r1;
      
    length += (next - cur).getLength();
  }
  
  for (size_t i = 0; i < points.size(); i++)
  {
    Tuple3f p0 = params[i * 4 + 0];
    Tuple3f p1 = params[i * 4 + 1];
    Tuple3f r0 = params[i * 4 + 2];
    Tuple3f r1 = params[i * 4 + 3];
    params[i * 4 + 0] = p0;
    params[i * 4 + 1] = r0;
    params[i * 4 + 2] = p0 *-3.0f + p1 * 3.0f - r0 * 2.0f - r1;
    params[i * 4 + 3] = p0 * 2.0f - p1 * 2.0f + r0 + r1;
  }
}

Tuple3f Spline::getLocationAtTime(float t)
{
  if (params.empty())
    return Tuple3f(0,0,0);
  t *= params.size()/cycleLength;
  t /= 4.0f;
  
  int i = (int)t;
  t -= i;
  i  = int(i % params.size()) * 4;
  float t2 = t * t,
             t3 = t2 * t;
  return params[i + 0] + params[i + 1] * t + params[i + 2] * t2 + params[i + 3] * t3;
}
