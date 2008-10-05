#include "ImageExt.h"
#include "ImageFormats.h"
#include "../FileSystem/FileSystem.h"
#include "../FileSystem/ArchivedFile.h"
#include "../Managers/MediaPathManager.h"

ImageExt::ImageExt(const char *path) : Image(path)
{}

ImageExt::ImageExt(const ImageExt &image)
{
  Image::operator=(image);
}

ImageExt &ImageExt::operator=(const ImageExt & copy)
{
  if (this != &copy)
    Image::operator=(copy);
  return *this;
}

bool ImageExt::loadSupportedFormat(const char* p)
{
  if (Image::loadSupportedFormat(p))
    return true;
    
  if (isTGA(p))
    return loadPackedTGA(p);
    
  if (isDDS(p))
    return loadPackedDDS(p);
    
  if (findString(p, "ptx") || findString(p, "PTX"))
    return loadPackedPTX(p);
    
  return Logger::writeErrorLog(String("Could not load ") + p);
}

bool ImageExt::loadPackedDDS(const char* p)
{
  ArchivedFile *file;
  DDS_Header_t dds;
  unsigned long magic, bits = 0;
  int size;
  int sizeOfBuffer;
  
  if (!(file = FileSystem::checkOut(p)))
    return Logger::writeErrorLog(String("Could not locate file -> ") + p);
    
  file->read(&magic, 4);
  
  if (magic != DDS_MAGIC)
  {
    FileSystem::checkIn(file);
    return Logger::writeErrorLog(String("Unrecognized DDS file -> ") + p);
  }
  
  file->read(&dds, sizeof(DDS_Header_t));
  
  if (dds.pfFlags & DDS_FOURCC)
  {
    switch (dds.pfFourCC)
    {
      case DDS_DXT1:
        setInternalFormat(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
        setComponentsCount(4);
        bits = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
        
      case DDS_DXT3:
        setInternalFormat(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
        setComponentsCount(4);
        bits = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
        
      case DDS_DXT5:
        setInternalFormat(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
        setComponentsCount(4);
        bits = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
        
      default:
        FileSystem::checkIn(file);
        return Logger::writeErrorLog(String("Unrecognized DDS four cc -> ") + p);
    }
  }
  else if (dds.pfFlags == DDS_RGBA && dds.pfRGBBitCount == 32)
  {
    bits = 32;
    setFormat(GL_BGRA);
    setComponentsCount(4);
    setInternalFormat(GL_RGBA);
  }
  else if (dds.pfFlags == DDS_RGB  && dds.pfRGBBitCount == 32)
  {
    bits = 32;
    setFormat(GL_BGRA);
    setComponentsCount(4);
    setInternalFormat(GL_RGBA);
  }
  else if (dds.pfFlags == DDS_RGB  && dds.pfRGBBitCount == 24)
  {
    bits = 24;
    setFormat(GL_BGR);
    setComponentsCount(3);
    setInternalFormat(GL_RGB);
  }
  else
  {
    FileSystem::checkIn(file);
    return Logger::writeErrorLog(String("Usupported DDS format -> ") + p);
  }
  
  setWidth(unsigned short(dds.Width));
  setHeight(unsigned short(dds.Height));
  
  if ((dds.Caps2 & DDS_VOLUME) && (dds.Depth > 0))
    setDepth(unsigned short(dds.Depth));
    
  if ((bits == 32) || (bits == 24))
    size = getWidth() * getHeight() * (bits / 8);
  else
  {
    if (bits == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
      size = ((getWidth() + 3) / 4) * ((getHeight() + 3) / 4) * 8;
    else
      size = ((getWidth() + 3) / 4) * ((getHeight() + 3) / 4) * 16;
  }
  
  sizeOfBuffer = size * (!getDepth() ? 1 : getDepth());
  GLubyte *buff = new GLubyte[sizeOfBuffer];
  
  if (getDepth())
    for (int c = 0; c < getDepth(); c++)
      file->read(buff + c * size, size);
  else
  {
    file->read(buff, size);
    flip(buff, getWidth(), getHeight(), size, bits);
  }
  
  setDataBuffer(buff);
  setPath(p);
  
  FileSystem::checkIn(file);
  
  return true;
}

bool ImageExt::loadPackedTGA(const char* p)
{
  unsigned char uncompressed8BitTGAHeader[12] = {0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};
  unsigned char uncompressedTGAHeader[12]     = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char compressedTGAHeader[12]       = {0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char TGAcompare[12];
  bool success = false;
  ArchivedFile *file;
  
  if (file = FileSystem::checkOut(p))
  {
    setPath(p);
    file->read(TGAcompare, 12);
    FileSystem::checkIn(file);
    
    if (!memcmp(uncompressedTGAHeader, TGAcompare, 12))
    {
      success = loadUncompressedTrueColor(p);
    }
    else if (!memcmp(compressedTGAHeader, TGAcompare, 12))
    {
      success = loadCompressedTrueColor(p);
    }
    else if (!memcmp(uncompressed8BitTGAHeader, TGAcompare, 12))
    {
      success = loadUncompressed8Bit(p);
    }
  }
  
  return success;
}

bool ImageExt::loadUncompressed8Bit(const char *filename)
{
  unsigned char TGAcompare[12]; //Used to compare TGA header
  unsigned char header[6]; //First 6 useful bytes of the header
  
  ArchivedFile *file;
  Tuple2i dim;
  
  file = FileSystem::checkOut(filename);
  
  file->seek(sizeof(TGAcompare), SEEKD);
  file->read(header, sizeof(header));
  
  dim.x = header[1] * 256 + header[0];
  dim.y = header[3] * 256 + header[2];
  
  setWidth(dim.x);
  setHeight(dim.y);
  setComponentsCount(header[4] / 8);
  setFormat(GL_RGB);
  setInternalFormat(GL_RGB8);
  
  if (dim.x <= 0 || dim.y <= 0 || header[4] != 8)
  {
    FileSystem::checkIn(file);
    return false;
  }
  
  unsigned char *palette = new unsigned char[256 * 3];
  unsigned char *indices = new unsigned char[dim.x * dim.y];
  GLubyte *buff = new GLubyte[dim.x * dim.y * 3];
  
  file->read(palette, 256 * 3);
  file->read(indices, dim.x * dim.y);
  
  FileSystem::checkIn(file);
  
  //calculate the color values
  for (int currentRow = 0; currentRow < dim.y; currentRow++)
    for (int i = 0; i < dim.x; i++)
    {
      buff[(currentRow*dim.x+i)*3+0] = palette[indices[currentRow*dim.x+i] * 3 + 2];
      buff[(currentRow*dim.x+i)*3+1] = palette[indices[currentRow*dim.x+i] * 3 + 1];
      buff[(currentRow*dim.x+i)*3+2] = palette[indices[currentRow*dim.x+i] * 3 + 0];//BGR
    }
    
  setDataBuffer(buff);
  
  deleteArray(buff);
  deleteArray(palette);
  deleteArray(indices);
  
  return true;
}

bool ImageExt::loadCompressedTrueColor(const char *filename)
{
  return false;
}

bool ImageExt::loadUncompressedTrueColor(const char *filename)
{
  unsigned char header[6]; //First 6 useful bytes of the header
  unsigned int imageSize; //Stores Image size when in RAM
  ArchivedFile *file;
  Tuple2i dim;
  
  file = FileSystem::checkOut(filename);
  
  file->seek(12, SEEKD);
  file->read(header, 6);
  
  dim.x = header[1] * 256 + header[0];
  dim.y = header[3] * 256 + header[2];
  
  setWidth(dim.x);
  setHeight(dim.y);
  setComponentsCount(header[4] / 3);
  
  if (header[4] == 24)
  {
    setFormat(GL_BGR);
    setInternalFormat(GL_RGB8);
  }
  else
  {
    setFormat(GL_BGRA);
    setInternalFormat(GL_RGBA8);
  }
  
  if (dim.x <= 0 || dim.y <= 0 || (header[4] != 24 && header[4] != 32))
  {
    FileSystem::checkIn(file);
    return false;
  }
  
  imageSize = dim.x * dim.y * getComponentsCount();
  
  GLubyte *buff = new GLubyte[imageSize];
  
  file->read(buff, imageSize);
  
  FileSystem::checkIn(file);
  
  setDataBuffer(buff);
  
  deleteArray(buff);
  
  return true;
}

bool ImageExt::loadPackedPTX(const char* p)
{
  ArchivedFile *file;
  GLubyte *dataBuffer;
  unsigned int width;
  unsigned int height;
  unsigned int bpp;
  unsigned int components;
  
  if (!(file = FileSystem::checkOut(p)))
    return Logger::writeErrorLog(String("Could not check out archived item -> ") + p);
    
  file->seek(6, SEEKD);
  file->read(&width, 4);
  file->read(&height, 4);
  file->read(&bpp, 4);
  
  components = bpp / 8;
  
  dataBuffer = new GLubyte[width * height * components];
  memset(dataBuffer, 0, width * height * components);
  
  file->read(dataBuffer, width * height * components);
  
  FileSystem::checkIn(file);
  
  setWidth(width);
  setHeight(height);
  if (bpp == 24)
  {
    setFormat(GL_BGR);
    setInternalFormat(GL_RGB8);
  }
  else
  {
    setFormat(GL_BGRA);
    setInternalFormat(GL_RGBA8);
  }
  setComponentsCount(components);
  setDataBuffer(dataBuffer);
  setPath(p);
  
  deleteArray(dataBuffer);
  
  return true;
}

ImageExt::~ImageExt()
{}
