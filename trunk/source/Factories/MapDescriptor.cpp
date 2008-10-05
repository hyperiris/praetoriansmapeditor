#include "MapDescriptor.h"
#include "../Parsers/XMLParser.h"

MapDescriptor::MapDescriptor()
{
  mapProjectName = "defaultproject";
  mapMinimap = "defaultminimap.TGA";
  mapElevation.set(2.98f, -0.75f);
}

bool MapDescriptor::loadXMLSettings(XMLElement *node)
{
  XMLElement *subchild, *attrib;
  Tuple2i players;
  
  if (subchild = node->getChildByName("Objetos"))
    if (attrib = subchild->getChildByName("name"))
      mapObjects = attrib->getValuec();
      
  if (subchild = node->getChildByName("Logica"))
    if (attrib = subchild->getChildByName("name"))
      mapLogic = attrib->getValuec();
      
  if (subchild = node->getChildByName("Colores"))
    if (attrib = subchild->getChildByName("name"))
      mapColors = attrib->getValuec();
      
  if (subchild = node->getChildByName("Visual"))
    if (attrib = subchild->getChildByName("name"))
      mapVisual = attrib->getValuec();
      
  if (subchild = node->getChildByName("Texturas"))
    if (attrib = subchild->getChildByName("name"))
      mapTextures = attrib->getValuec();
      
  if (subchild = node->getChildByName("Praderas"))
    if (attrib = subchild->getChildByName("name"))
      mapMeadows = attrib->getValuec();
      
  if (subchild = node->getChildByName("Agua"))
    if (attrib = subchild->getChildByName("name"))
      mapWaters = attrib->getValuec();
      
  if (subchild = node->getChildByName("Minimap"))
    if (attrib = subchild->getChildByName("name"))
      mapMinimap = attrib->getValuec();
      
  if (subchild = node->getChildByName("Mode"))
    if (attrib = subchild->getChildByName("name"))
      mapMode = attrib->getValuec();
      
  if (subchild = node->getChildByName("Camara"))
  {
    if (attrib = subchild->getChildByName("Posicion"))
      XMLElement::loadRX_GY_BZf(*attrib, camposition);
      
    if (attrib = subchild->getChildByName("Mira"))
      XMLElement::loadRX_GY_BZf(*attrib, camfocus);
  }
  
  if (attrib = node->getChildByName("Size"))
    XMLElement::loadRX_GYi(*attrib, mapDimensions);
    
  if (subchild = node->getChildByName("Elevation"))
  {
    if (attrib = subchild->getChildByName("max"))
      mapElevation.x = attrib->getValuef();
      
    if (attrib = subchild->getChildByName("min"))
      mapElevation.y = attrib->getValuef();
  }
  
  if (attrib = node->getChildByName("MaxPlayers"))
  {
    XMLElement::loadRX_GYi(*attrib, players);
    maxPlayers = players.x;
  }
  
  return true;
}

MapDescriptor::~MapDescriptor()
{
}
