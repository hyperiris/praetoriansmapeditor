#ifndef IMAGEEXT
#define IMAGEEXT

#include "Image.h"

class ImageExt : public Image
{
  public:
    ImageExt(const char* path = 0);
    ImageExt(const ImageExt &image);
    ImageExt &operator=(const ImageExt &image);
    ~ImageExt();
    
  private:
    bool loadUncompressed8Bit(const char *filename);
    bool loadCompressedTrueColor(const char *filename);
    bool loadUncompressedTrueColor(const char *filename);
    
    bool loadPackedTGA(const char* p);
    bool loadPackedDDS(const char* p);
    bool loadPackedPTX(const char* p);
    
  protected:
    bool loadSupportedFormat(const char* p);
};

#endif