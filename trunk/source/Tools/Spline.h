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

#ifndef SPLINE_H
#define SPLINE_H

#include "../Math/MathUtils.h"
#include "../Tools/Logger.h"

class Spline
{
  public:
    Spline()
    {
      length = 0.0;
    }
    
    Spline(const char *xmlPath,
           int   close      =   0,
           float tension    = 0.0,
           float bias       = 0.0,
           float continuity = 0.0);
           
    Spline(vector<Tuple3f> &points,
           int   close       = 0,
           float tension    = 0.0,
           float bias       = 0.0,
           float continuity = 0.0);
    ~Spline();
    
    Tuple3f getLocationAtTime(float t);
    
    bool    loadXMLOrbit(const char *xmlPath,
                         float tension    = 0.0,
                         float bias       = 0.0,
                         float continuity = 0.0);
                         
    bool    loadXMLOrbit(XMLElement *splineNode,
                         float tension    = 0.0,
                         float bias       = 0.0,
                         float continuity = 0.0);
                         
    float   getCycleLength();
    void    setCycleLength(float cycle);
    
  protected:
  
    void create(vector<Tuple3f> &points,
                int close,
                float tension,
                float bias,
                float continuity);
                
    vector<Tuple3f> params;
    float           length,
    cycleLength;
};

#endif
