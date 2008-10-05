#ifndef IMAGEFORMATS
#define IMAGEFORMATS

#ifndef FREE
#define FREE(p) { if(p) { free(p); p=NULL; } }
#endif

#define DDS_FOURCC  0x00000004
#define DDS_RGB     0x00000040
#define DDS_RGBA    0x00000041
#define DDS_VOLUME  0x00200000
#define DDS_CUBEMAP 0x00000200

#define DDS_MAGIC ('D'|('D'<<8)|('S'<<16)|(' '<<24))
#define DDS_DXT1 ('D'|('X'<<8)|('T'<<16)|('1'<<24))
#define DDS_DXT3 ('D'|('X'<<8)|('T'<<16)|('3'<<24))
#define DDS_DXT5 ('D'|('X'<<8)|('T'<<16)|('5'<<24))

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

typedef struct
{
  unsigned short col0, col1;
  unsigned char row[4];
} DXTColorBlock_t;

typedef struct
{
  unsigned short row[4];
} DXT3AlphaBlock_t;

typedef struct
{
  unsigned char alpha0, alpha1;
  unsigned char row[6];
} DXT5AlphaBlock_t;

typedef struct
{
  unsigned long Size;
  unsigned long Flags;
  unsigned long Height;
  unsigned long Width;
  unsigned long PitchLinearSize;
  unsigned long Depth;
  unsigned long MipMapCount;
  unsigned long Reserved1[11];
  unsigned long pfSize;
  unsigned long pfFlags;
  unsigned long pfFourCC;
  unsigned long pfRGBBitCount;
  unsigned long pfRMask;
  unsigned long pfGMask;
  unsigned long pfBMask;
  unsigned long pfAMask;
  unsigned long Caps1;
  unsigned long Caps2;
  unsigned long Reserved2[3];
} DDS_Header_t;

void Swap(void *byte1, void *byte2, int size);
void flipDXT1Blocks(DXTColorBlock_t *Block, int NumBlocks);
void flipDXT3Blocks(DXTColorBlock_t *Block, int NumBlocks);
void flipDXT5Alpha(DXT5AlphaBlock_t *Block);
void flipDXT5Blocks(DXTColorBlock_t *Block, int NumBlocks);
void flip(unsigned char *image, int width, int height, int size, int format);

#endif