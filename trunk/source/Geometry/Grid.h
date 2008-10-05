#ifndef GRID
#define GRID

class Grid
{
  public:
    Grid();
    ~Grid();
    
    void draw(void);
    void setOffsetY(float value);
    void setup(float step, float size, float major);
    
  private:
    void drawMinorGridLines();
    void drawMajorGridLines();
    
  private:
    float gridSize;
    float stepSize;
    float majorSize;
    float yOffset;
    unsigned int mingrid;
};

#endif