#ifndef FRAMEINFO
#define FRAMEINFO

class FrameInfo
{
  public:
    FrameInfo(float interval, float fps, int width, int height)
    {
      m_Fps       = fps;
      m_Width     = width;
      m_Height    = height;
      m_Interval  = interval;
    }
    
    int m_Width;
    int m_Height;
    
    float m_Interval;
    float m_Fps;
};

#endif
