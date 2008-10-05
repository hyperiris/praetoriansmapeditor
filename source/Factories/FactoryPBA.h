#ifndef FACTORYPBA
#define FACTORYPBA

class TransformGroup;
class Appearance;

enum MeshType
{
  SHAPE_RIGID
};

bool loadPBA(TransformGroup* parent, const char* filename);
void setAlphaBlendingState(Appearance* appearance, int state);

#endif