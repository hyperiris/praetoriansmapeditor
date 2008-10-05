#include "TileGraph.h"
#include "../Nodes/TextureNode.h"
#include "../Nodes/IndicesNode.h"
#include "../Nodes/TileGraphNode.h"
#include "../Nodes/TextureCoordsNode.h"
#include "../Tools/TemplatedNodeIterator.h"
#include "../Visitors/Visitor.h"

TileGraph::TileGraph()
{
  voidParent = 0;
}

void TileGraph::initialize(unsigned int nlayers)
{
  IndicesNode *inode;
  Node *node;
  unsigned int i;
  
  tileindices[0] = 4;
  tileindices[1] = 3;
  tileindices[2] = 0;
  tileindices[3] = 1;
  tileindices[4] = 2;
  tileindices[5] = 5;
  tileindices[6] = 8;
  tileindices[7] = 7;
  tileindices[8] = 6;
  tileindices[9] = 3;
  
  voidParent = txcoordsNodePool.allocate();//new TextureCoordsNode();
  nodeList.append(voidParent);
  
  for (i = 0; i < nlayers; i++)
  {
    //node = new Node();
    //inode = new IndicesNode();
    node = nodePool.allocate();
    inode = indicesNodePool.allocate();
    inode->attach(node);
    inode->setIndices(tileindices);
    layers.append(node);
    nodeList.append(node);
    nodeList.append(inode);
  }
  
  for (i = 0; i < nlayers; i++)
    attatchmentPoints.append(AttachmentPoint());
}

unsigned int TileGraph::integrate(unsigned int layer, unsigned int txid, Tuple2f *coords)
{
  TextureCoordsNode* tcNode;
  TextureNode* txNode;
  
  txNode = checkForRepeat((TextureNode*)layers(layer)->getFirstChild()->getFirstChild(), txid);
  
  if (!txNode)
  {
    //txNode = new TextureNode();
    txNode = textureNodePool.allocate();
    txNode->setID(txid);
    txNode->attach(layers(layer)->getFirstChild());
    nodeList.append(txNode);
  }
  
  //tcNode = new TextureCoordsNode();
  tcNode = txcoordsNodePool.allocate();
  tcNode->setCoords(coords);
  tcNode->attach(txNode);
  tcNode->hide();
  nodeList.append(tcNode);
  
  attatchmentPoints(layer).aggregateNodes.append(tcNode);
  
  return attatchmentPoints(layer).aggregateNodes.length() - 1;
}

void TileGraph::inspect(unsigned int layer, Visitor* visitor)
{
  visitor->visit(layers[layer]);
}

TileGraphNode* TileGraph::addModelNode(unsigned int layer, int index)
{
  TextureCoordsNode *tcNode = (-1 == index) ? voidParent : attatchmentPoints(layer).aggregateNodes(index);
  TileGraphNode *tgNode = tileNodePool.allocate();//new TileGraphNode();
  
  tgNode->attach(tcNode);
  tgNode->hide();
  tcNode->reveal();
  nodeList.append(tgNode);
  
  return tgNode;
}

void TileGraph::moveModelNode(TileGraphNode *node, unsigned int layer, int index, bool hide)
{
  TextureCoordsNode *texCoordsNode;
  
  texCoordsNode = (TextureCoordsNode*) node->getParentNode();
  node->detach();
  
  if (!texCoordsNode->getFirstChild())
    texCoordsNode->hide();
    
  if (-1 == index)
    node->attach(voidParent);
  else
  {
    texCoordsNode = attatchmentPoints(layer).aggregateNodes(index);
    texCoordsNode->reveal();
    node->attach(texCoordsNode);
  }
  
  if (hide)
    node->hide();
}

TextureNode* TileGraph::checkForRepeat(TextureNode* node, unsigned int id)
{
  if (!node)
    return 0;
    
  TemplatedNodeIterator <TextureNode> iter(node);
  while (!iter.end())
  {
    if (iter.current()->getID() == id)
      return iter.current();
    iter++;
  }
  
  return 0;
}

void TileGraph::reset()
{
  for (unsigned int i = 0; i < nodeList.length(); i++)
    nodeList(i)->reset();
    
  nodeList.clearFast();
  attatchmentPoints.clear();
  layers.clear();
  
  nodePool.flush();
  tileNodePool.flush();
  textureNodePool.flush();
  indicesNodePool.flush();
  txcoordsNodePool.flush();
}

void TileGraph::destroy()
{
  nodeList.clearFast();
  nodePool.destroy();
  tileNodePool.destroy();
  textureNodePool.destroy();
  indicesNodePool.destroy();
  txcoordsNodePool.destroy();
}

TileGraph::~TileGraph()
{
  destroy();
}