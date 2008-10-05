#ifndef TEXTUREDGRID
#define TEXTUREDGRID

#include "../Appearance/Texture.h"
#include "../Containers/ocarray.h"

class TexturedGrid
{
  public:
    TexturedGrid();
    ~TexturedGrid();
    
  public:
    void draw();
    void loadTexture(const char *filename);
    void setOffsetY(float value);
    void setup(unsigned int size);
    void applyGaussianDistribution(float strength, float radius);
    
  private:
    void setupIndexField();
    void setupColorField();
    void setupVertexField();
    void setupTexCoordField();
    
  private:
    Texture texture;
    unsigned int gridSize;
    float yOffset;
    
    //Tuple3f* vertexField;
    //Tuple2f* txCoordField;
    //Tuple3ub* colorField;
    //unsigned int* indexField;
    
    Array <Tuple3f> vertexField;
    Array <Tuple2f> txCoordsField;
    Array <Tuple3ub> colorField;
    Array <unsigned int> indexField;
};

#endif
