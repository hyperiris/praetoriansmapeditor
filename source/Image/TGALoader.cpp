#include "Image.h"

bool Image::loadTGA(const char* filename)
{
  unsigned char uncompressed8BitTGAHeader[12]= {0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0},
      uncompressedTGAHeader[12]    = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                     compressedTGAHeader[12]      = {0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                                     
  unsigned char TGAcompare[12];           //Used to compare TGA header
  
  FILE * file = fopen(filename, "rb");    //Open the TGA file
  
  if (file==NULL)                         //Does the file exist?
    return Logger::writeErrorLog(String("Could not find file -> ") + filename);
    
    
  //read the header
  fread(TGAcompare, 1, sizeof(TGAcompare), file);
  fclose(file);
  
  if (!memcmp(uncompressedTGAHeader, TGAcompare, sizeof(uncompressedTGAHeader)))
  {
    return loadUncompressedTrueColorTGA(filename);
  }
  else if (!memcmp(compressedTGAHeader, TGAcompare, sizeof(compressedTGAHeader)))
  {
    return loadCompressedTrueColorTGA(filename);
  }
  else if (!memcmp(uncompressed8BitTGAHeader, TGAcompare, sizeof(uncompressed8BitTGAHeader)))
  {
    return loadUncompressed8BitTGA(filename);
  }
  else
    return Logger::writeErrorLog(String("Unrecognized TGA format -> ") + filename);
    
  return false;
}

bool Image::loadUncompressed8BitTGA(const char * filename)
{
  unsigned char   TGAHeader[12]={0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};
  unsigned char   TGAcompare[12];           //Used to compare TGA header
  unsigned char   header[6];              //First 6 useful bytes of the header
  
  FILE * file = fopen(filename, "rb");        //Open the TGA file
  
  if (file == NULL)               //Does the file exist?
    return Logger::writeErrorLog(String("Could not find file at -> ") + filename);
    
  if (fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)|| //Are there 12 bytes to read?
      memcmp(TGAHeader, TGAcompare, sizeof(TGAHeader))!=0 ||          //Is the header correct?
      fread(header, 1, sizeof(header), file)!=sizeof(header))   //Read next 6 bytes
  {
    fclose(file);               //If anything else failed, close the file
    return Logger::writeErrorLog(String("Could not process file at -> ") + filename);
  }
  
  //save data into class member variables
  setWidth(header[1]*256+header[0]);
  setHeight(header[3]*256+header[2]);
  setComponentsCount(header[4]/8);
  
  if (width  <=0 || //if width <=0
      height <=0 || //or height<=0
      header[4] != 8)    //bpp not 8
  {
    fclose(file);                     //close the file
    return Logger::writeErrorLog(String("The height or width is less than zero, or the TGA is not 8 bpp -> ") + filename);
  }
  
  setFormat(GL_RGB);
  setInternalFormat(GL_RGB8);
  
  //make space for palette
  unsigned char * palette=new unsigned char[256*3];
  if (!palette)
    return Logger::writeErrorLog("Unable to allocate memory for palette");
    
    
  //load the palette
  fread(palette, 256*3, 1, file);
  
  //allocate space for color indices
  unsigned char * indices=new unsigned char[width*height];
  if (!indices)
    return Logger::writeErrorLog("Unable to allocate memory for indices");
    
    
  //load indices
  fread(indices, 1, width*height, file);
  
  //close the file
  fclose(file);
  
  if (dataBuffer)
    deleteArray(dataBuffer);
    
  //allocate space for the image data
  dataBuffer = new GLubyte[width*height*3];
  
  if (!dataBuffer)
  {
    fclose(file);
    return Logger::writeErrorLog(String("Unable to allocate memory for ->") + filename);
  }
  
  //calculate the color values
  for (int currentRow=0; currentRow<height; currentRow++)
  {
    for (int i=0; i<width; i++)
    {
      dataBuffer[(currentRow*width+i)*3+0]=palette[indices[currentRow*width+i]*3+2];
      dataBuffer[(currentRow*width+i)*3+1]=palette[indices[currentRow*width+i]*3+1];
      dataBuffer[(currentRow*width+i)*3+2]=palette[indices[currentRow*width+i]*3+0];//BGR
    }
  }
  return true;
}

bool Image::loadUncompressedTrueColorTGA(const char * filename)
{
  unsigned char TGAheader[12]={0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //Uncompressed TGA header
  unsigned char TGAcompare[12];           //Used to compare TGA header
  unsigned char header[6];              //First 6 useful bytes of the header
  unsigned int  imageSize;              //Stores Image size when in RAM
  
  FILE * file = fopen(filename, "rb");        //Open the TGA file
  
  if (file == NULL)               //Does the file exist?
    return Logger::writeErrorLog(String("Could not load file at -> ") + filename);
    
  if (fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||  //Are there 12 bytes to read?
      memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0 ||          //Is the header correct?
      fread(header, 1, sizeof(header), file)!=sizeof(header))   //Read next 6 bytes
  {
    fclose(file);               //If anything else failed, close the file
    return Logger::writeErrorLog(String("Could not process file at -> ") + filename);
  }
  
  //save data into class member variables
  setWidth(header[1]*256+header[0]);           //determine the image width
  setHeight(header[3]*256+header[2]);            //determine image height
  setComponentsCount(header[4]/3);
  
  if (width <=0 ||                     //if width <=0
      height<=0 ||                      //or height<=0
      (header[4] !=24 && header[4]!=32))                   //bpp not 24 or 32
  {
    fclose(file);                     //close the file
    return Logger::writeErrorLog(String("The height or width is less than zero, or the TGA is not 24 bpp -> ") + filename);
  }
  
  //set format
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
  
  imageSize = width*height*getComponentsCount();
  
  if (dataBuffer)
    deleteArray(dataBuffer);
    
  dataBuffer = new GLubyte[imageSize];
  
  if (dataBuffer==NULL)                    //Does the storage memory exist?
  {
    fclose(file);
    return Logger::writeErrorLog(String("Unable to allocate memory for image ->") + filename);
  }
  
  //read in the image data
  fread(dataBuffer, 1, imageSize, file);
  fclose(file);
  return true;
}

//load a compressed TGA texture (24 or 32 bpp)
bool Image::loadCompressedTrueColorTGA(const char * filename)
{
  unsigned char TGAheader[12]={0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};  //Compressed TGA header
  unsigned char TGAcompare[12];           //Used to compare TGA header
  unsigned char header[6];              //First 6 useful bytes of the header
  unsigned int  bytesPerPixel;            //bytes per pixel
  unsigned int  imageSize;              //Stores Image size when in RAM
  
  FILE * file = fopen(filename, "rb");        //Open the TGA file
  
  if (file == NULL)               //Does the file exist?
    return Logger::writeErrorLog(String("Could not load file at -> ") + filename);
    
    
  if (fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||  //Are there 12 bytes to read?
      memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0 ||          //Is the header correct?
      fread(header, 1, sizeof(header), file)!=sizeof(header))   //Read next 6 bytes
  {
    fclose(file);               //If anything else failed, close the file
    return Logger::writeErrorLog(String("Could not process file at -> ") + filename);
  }
  
  //save data into class member variables
  setWidth(header[1]*256+header[0]);            //determine the image width
  setHeight(header[3]*256+header[2]);            //determine image height
  setComponentsCount(header[4]/8);
  bytesPerPixel = getComponentsCount();
  
  if (width  <=0 ||                     //if width <=0
      height <=0 ||                      //or height<=0
      (header[4] !=24 && header[4] !=32))                   //bpp not 24 or 32
  {
    fclose(file);                     //close the file
    return Logger::writeErrorLog(String("The height or width is less than zero, or the TGA is not 24 bpp -> ") + filename);
  }
  
  //set format
  if (header[4] == 24)
  {
    setFormat(GL_RGB);
    setInternalFormat(GL_RGB8);
  }
  else
  {
    setFormat(GL_RGBA);
    setInternalFormat(GL_RGBA8);
  }
  
  imageSize = width*height*getComponentsCount();
  
  if (dataBuffer)
    deleteArray(dataBuffer);
  dataBuffer = new GLubyte[imageSize];
  
  if (!dataBuffer)                        //Does the storage memory exist?
  {
    fclose(file);
    return Logger::writeErrorLog(String("Unable to allocate memory for image -> ") + filename);
  }
  
  //read in the image data
  int pixelCount  = width*height;
  int currentPixel= 0;
  int currentByte = 0;
  GLubyte *colorBuffer = new GLubyte[getComponentsCount()];
  
  do
  {
    unsigned char chunkHeader=0;
    
    if (!fread(&chunkHeader, sizeof(unsigned char), 1, file))
    {
      if (file)
        fclose(file);
      deleteArray(dataBuffer);
      return Logger::writeErrorLog("Could not read RLE chunk header");
    }
    
    if (chunkHeader<128) //Read raw color values
    {
      chunkHeader++;
      
      for (short counter=0; counter<chunkHeader; counter++)
      {
        if (fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
        {
          if (file)
            fclose(file);
          deleteArray(colorBuffer);
          deleteArray(dataBuffer);
          return Logger::writeErrorLog(String("Could not read image data -> ") + filename);
        }
        
        //transfer pixel color to data (swapping r and b values)
        dataBuffer[currentByte]   = colorBuffer[2];
        dataBuffer[currentByte+1] = colorBuffer[1];
        dataBuffer[currentByte+2] = colorBuffer[0];
        
        if (bytesPerPixel==4)
          dataBuffer[currentByte+3]=colorBuffer[3];
          
        currentByte+=bytesPerPixel;
        currentPixel++;
        
        if (currentPixel > pixelCount)
        {
          if (file)
            fclose(file);
          deleteArray(colorBuffer);
          deleteArray(dataBuffer);
          return Logger::writeErrorLog("Too many pixels read");
        }
      }
    }
    else  //chunkHeader>=128
    {
      chunkHeader-=127;
      
      if (fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
      {
        if (file)
          fclose(file);
        deleteArray(colorBuffer);
        deleteArray(dataBuffer);
        return Logger::writeErrorLog(String("Unable to read image data -> ") + filename);
      }
      
      for (short counter=0; counter<chunkHeader; counter++)
      {
        //transfer pixel color to data (swapping r and b values)
        dataBuffer[currentByte]   = colorBuffer[2];
        dataBuffer[currentByte+1] = colorBuffer[1];
        dataBuffer[currentByte+2] = colorBuffer[0];
        
        if (bytesPerPixel==4)
          dataBuffer[currentByte+3]=colorBuffer[3];
          
        currentByte+=bytesPerPixel;
        currentPixel++;
        
        if (currentPixel > pixelCount)
        {
          if (file)
            fclose(file);
          deleteArray(colorBuffer);
          deleteArray(dataBuffer);
          return Logger::writeErrorLog("Too many pixels read");
        }
      }
    }
  }
  while (currentPixel<pixelCount);
  
  fclose(file);
  deleteArray(colorBuffer);
  return true;
}
