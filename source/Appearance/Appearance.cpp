#include "Appearance.h"

Appearance::Appearance() : IOXMLObject("Appearance")
{
  shadersEnabled = true;
  colorEnabled   = false;
  material       = NULL;
  color.set(1,1,1,0);
  type = MATERIAL_NONE;
  depthMask = true;
}

Appearance::Appearance(const Appearance &app) : IOXMLObject("Appearance")
{
  if (this != &app)
    this->operator=(app);
}

Appearance &Appearance::operator =(const Appearance &app)
{
  if (this != &app)
  {
    destroy();
    
    for (int i = 0; i < MAX_TEX_UNITS; i++)
      if (app.textures[i].getID())
        textures[i] = app.textures[i];
        
    shadersEnabled  = app.shadersEnabled;
    colorEnabled    = app.colorEnabled;
    blendAttr       = app.blendAttr;
    alphaAttr       = app.alphaAttr;
    shaders         = app.shaders;
    color           = app.color;
    type            = app.type;
    
    if (app.material)
      setMaterial(*app.material);
  }
  return *this;
}

bool Appearance::loadXMLSettings(XMLElement *element)
{
  XMLElement  *child        = NULL;
  int          textureIndex = 0;
  
  if (!isSuitable(element))
    return false;
    
  if (child = element->getChildByName("description"))
    return  IOXMLObject::loadXMLFile(child->getValuec());
    
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if (!(child = element->getChild(i)))
      continue;
      
    const String &childName = child->getName();
    textureIndex = clamp(textureIndex, 0, MAX_TEX_UNITS-1);
    
    if (childName == "Material")
    {
      if (!material)
        material = new Material();
        
      material->loadXMLSettings(child);
      continue;
    }
    
    if (childName == "Texture")
    {
      if (textures[textureIndex].loadXMLSettings(child))
        textureIndex++;
      continue;
    }
    
    if (childName == "AlphaAttributes")
    {
      alphaAttr.loadXMLSettings(child);
      continue;
    }
    
    if (childName == "BlendAttributes")
    {
      blendAttr.loadXMLSettings(child);
      continue;
    }
    
    if (childName == "Color")
    {
      XMLElement::loadRX_GY_BZ_AWf(*child, color);
      continue;
    }
    
    if (childName == "colorEnabled")
    {
      enableColor(child->getValue() == "true");
      continue;
    }
    
    if (childName == "shadersEnabled")
    {
      enableShaders(child->getValue() == "true");
      continue;
    }
  }
  return true;
}

bool Appearance::exportXMLSettings(ofstream &xmlFile)
{
  if (!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export Appearance to XML: file not ready.");
    
  xmlFile << "  <Appearance colorEnabled   = \"" << (colorEnabled  ? "true" : "false") << "\"\n"
  << "              shadersEnabled = \"" << (shadersEnabled? "true" : "false") << "\">\n";
  
  xmlFile << "    <Color r = \""  << color.x << "\" "
  << "g = \""  << color.y << "\" "
  << "b = \""  << color.z << "\" "
  << "a = \""  << color.w << "\"/>\n";
  
  alphaAttr.exportXMLSettings(xmlFile);
  blendAttr.exportXMLSettings(xmlFile);
  
  if (material)
    material->exportXMLSettings(xmlFile);
    
  for (int i = 0; i < MAX_TEX_UNITS; i++)
    if (textures[i].getID())
      textures[i].exportXMLSettings(xmlFile);
      
  xmlFile << "  </Appearance>\n";
  return true;
}

void Appearance::setTexture(int index, Texture &texture)
{
  if (index < 0 || index > MAX_TEX_UNITS - 1)
    return;
    
  textures[index] = texture;
}

Texture *Appearance::getTexture(int index)
{
  return (index < 0) || (index >= MAX_TEX_UNITS) ? NULL : &textures[index];
}

void Appearance::setBlendAttributes(const BlendAttributes &attri)
{
  blendAttr = attri;
}

BlendAttributes &Appearance::getBlendAttributes()
{
  return blendAttr;
}

void Appearance::setAlphaAttributes(const AlphaAttributes &attri)
{
  alphaAttr = attri;
}

AlphaAttributes &Appearance::getAlphaAttributes()
{
  return alphaAttr;
}

void      Appearance::setMaterial(const Material &mat)
{
  if (!material)
    material = new Material(mat);
  else
    material->operator =(mat);
}
Material *Appearance::getMaterial()
{
  return material;
}

void  Appearance::setColor(const float* c)
{
  color.set(c[0], c[1], c[2], c[3]);
}
void  Appearance::setColor(float r, float g, float b, float a)
{
  color.set(r,g,b,a);
}
const Tuple4f &Appearance::getColor() const
{
  return color;
}

void Appearance::setShaders(Shaders &shadersArg)
{
  shaders = shadersArg;
}
Shaders &Appearance::getShaders()
{
  return shaders;
}

void Appearance::enableShaders(bool enabled)
{
  shadersEnabled = enabled;
}
void Appearance::enableColor(bool enabled)
{
  colorEnabled   = enabled;
}

bool Appearance::shadersOn()
{
  return shadersEnabled;
}

bool Appearance::colorOn()
{
  return colorEnabled;
}

int Appearance::getType()
{
  return type;
}

void Appearance::setType(int flags)
{
  type = flags;
}

void Appearance::enableDepthMask(bool e)
{
  depthMask = e;
}

bool Appearance::depthMaskEnabled()
{
  return depthMask;
}

bool Appearance::blendOn()
{
  return blendAttr.isSet();
}

bool Appearance::alphaOn()
{
  return alphaAttr.isSet();
}

void Appearance::destroy()
{
  for (int i = 0; i < MAX_TEX_UNITS; i++)
    textures[i].destroy();
    
  deleteObject(material);
  shaders.destroy();
  shadersEnabled  = false;
}

Appearance::~Appearance()
{
  destroy();
}
