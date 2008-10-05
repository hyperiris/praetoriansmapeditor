#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include "../Tools/String.h"

template <class T>
class MediaInfo
{
  public:
    MediaInfo(const char* mediaPath_ = 0, T mediaArg = 0)
    {
      userCount = 1;
      mediaPath = mediaPath_;
      media     = mediaArg;
    }
    
    MediaInfo(const MediaInfo &copy)
    {
      this->operator=(copy);
    }
    
    MediaInfo &operator = (const MediaInfo &copy)
    {
      if (this != &copy)
      {
        userCount = copy.userCount;
        mediaPath = copy.mediaPath;
        media     = copy.media;
      }
      return *this;
    }
    
    void setMediaPath(const char *mediaPath_)
    {
      mediaPath = mediaPath_;
    }
    
    const String &getMediaPath() const
    {
      return mediaPath;
    }
    
    void increaseUserCount()
    {
      userCount++;
    }
    
    void decreaseUserCount()
    {
      userCount--;
    }
    
    unsigned int getUserCount()
    {
      return userCount;
    }
    
    T &getMedia()
    {
      return media;
    }
    
    void  setMedia(T &mediaArg)
    {
      media = mediaArg;
    }
    
    bool operator > (const MediaInfo &info)
    {
      return mediaPath > info.mediaPath;
    }
    
    bool operator < (const MediaInfo &info)
    {
      return mediaPath < info.mediaPath;
    }
    
    bool operator == (const MediaInfo &info)
    {
      return (mediaPath == info.mediaPath) &&
             (userCount == info.userCount) &&
             (media     == info.media);
    }
    
    bool operator != (const MediaInfo &info)
    {
      return !this->operator == (info);
    }
    
  protected:
    unsigned int  userCount;
    String        mediaPath;
    T             media;
};

#endif