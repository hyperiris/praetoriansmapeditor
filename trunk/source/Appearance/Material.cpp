#include "Material.h"
#include "../Managers/MediaPathManager.h"

Material::Material() : MatLight(), IOXMLObject("Material")
{
  shininess   =   64;
}

Material::Material(const Material &mat) : MatLight() , IOXMLObject("Material")
{
  this->operator =(mat);
}

Material  &Material::operator = (const Material& copy)
{
  if (this != &copy)
  {
    shininess = copy.shininess;
    specular  = copy.specular;
    emissive  = copy.emissive;
    diffuse   = copy.diffuse;
    ambient   = copy.ambient;
  }
  return *this;
}

void Material::bindMaterial()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT , ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE , diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

bool Material::loadXMLSettings(XMLElement *element)
{
  XMLElement  *child     = NULL;
  
  if (!isSuitable(element))
    return false;
    
  if (child = element->getChildByName("description"))
    return  IOXMLObject::loadXMLFile(child->getValuec());
    
  for (size_t i = 0; i < element->getChildrenCount(); i++)
  {
    if (!(child = element->getChild(i)))
      continue;
      
    const String &childName = child->getName();
    
    if (childName == "shininess")
    {
      setShininess(child->getValuef());
      continue;
    }
    
    if (childName == "Ambient")
    {
      Tuple4f attributes;
      XMLElement::loadRX_GY_BZ_AWf(*child, attributes);
      setAmbient(attributes);
      continue;
    }
    
    if (childName == "Diffuse")
    {
      Tuple4f attributes;
      XMLElement::loadRX_GY_BZ_AWf(*child, attributes);
      setDiffuse(attributes);
      continue;
    }
    
    if (childName == "Emissive")
    {
      Tuple4f attributes;
      XMLElement::loadRX_GY_BZ_AWf(*child, attributes);
      setEmissive(attributes);
      continue;
    }
    
    if (childName == "Specular")
    {
      Tuple4f attributes;
      XMLElement::loadRX_GY_BZ_AWf(*child, attributes);
      setSpecular(attributes);
      continue;
    }
  }
  return true;
}

bool Material::exportXMLSettings(ofstream &xmlFile)
{
  if (!xmlFile.is_open())
    return Logger::writeErrorLog("Cannot export Material to XML: file not ready.");
    
  String  matAttr[4]   = { "Ambient ",  "Diffuse ", "Emissive", "Specular" };
  Tuple4f attributes;
  xmlFile << "  <Material shininess = \"" << getShininess() << "\"> \n";
  
  for (int i = 0; i < 4; i++)
  {
    switch (i)
    {
      case 0:
        attributes = getAmbient();
        break;
      case 1:
        attributes = getDiffuse();
        break;
      case 2:
        attributes = getEmissive();
        break;
      case 3:
        attributes = getSpecular();
        break;
    }
    if (attributes)
    {
      xmlFile << "    <"    << matAttr[i]   << " ";
      xmlFile << " r = \""  << attributes.x << "\" "
      << " g = \""  << attributes.y << "\" "
      << " b = \""  << attributes.z << "\" "
      << " a = \""  << attributes.w << "\"/>\n";
    }
  }
  
  xmlFile << "  </Material>\n";
  return true;
}

void Material::setShininess(const float s)
{
  shininess = s;
}

void Material::setEmissive(const Tuple4f &v)
{
  emissive = v;
}

void Material::setEmissive(const float r, const float g,
                           const float b, const float a)
{
  emissive.set(r, g, b, a);
}