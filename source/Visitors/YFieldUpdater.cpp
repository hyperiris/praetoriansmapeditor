#include "YFieldUpdater.h"
#include "../Nodes/SpatialIndexNode.h"
#include "../Nodes/SpatialIndexCell.h"
//#include "../Nodes/StaticModelNode.h"
#include "../Tools/NodeIterator.h"
#include "../Controllers/TileController.h"
//#include "../Controllers/ModelController.h"
#include "../Geometry/Ray3D.h"
#include "../Math/Intersector.h"
#include "../Kernel/Gateway.h"

YFieldUpdater::YFieldUpdater()
{
}

void YFieldUpdater::visit(SpatialIndexCell *cell)
{
  NodeIterator iter(cell->getFirstChild());
  while (!iter.end())
  {
    iter.current()->accept(this);
    iter++;
  }
}

void YFieldUpdater::visit(TransformGroupNode *node)
{
  //TileController* controller;
  //Intersector intersector;
  //Ray3D ray;
  //Tuple4f packet;
  //Tuple3f position;
  //
  //position = node->getController()->getModelPosition();
  //controller = Gateway::getTileGraphController(Tuple2f(position.x, position.z));
  //
  //if (controller)
  //{
  //  ray.setOrigin(Tuple3f(position.x, 1000, position.z));
  //  ray.setDestination(Tuple3f(position.x, -1000, position.z));
  //  packet = intersector.intersectQuad(&ray, controller->getVertices(), 9, TRIANGLE_FAN);
  //  node->getController()->translate(Tuple3f(packet.x, packet.y + node->getController()->getJointTransform(0).y + 0.05f, packet.z));
  //}
}
