#include "SoundManager.h"
#include "../fmod/inc/fmod_errors.h"
#include "../Tools/Logger.h"
#include "../FileSystem/ArchivedFile.h"
#include "../FileSystem/FileSystem.h"

FMOD::System* SoundManager::soundSystem = 0;
FMOD::Sound *SoundManager::sound = 0;
FMOD::Sound *SoundManager::song = 0;
FMOD::Channel *SoundManager::channel = 0;

bool SoundManager::initialize()
{
  unsigned int version;
  
  if (!SoundManager::errorCheck(FMOD::System_Create(&soundSystem)))
    return false;
    
  if (!SoundManager::errorCheck(soundSystem->getVersion(&version)))
    return false;
    
  if (version < FMOD_VERSION)
  {
    char strbuff[256] = {0};
    sprintf_s(strbuff, 256, "you are using an old version of FMOD %08x. This program requires %08x\n", version, FMOD_VERSION);
    return Logger::writeErrorLog(String("FMOD error: ") + strbuff);
  }
  
  if (!SoundManager::errorCheck(soundSystem->init(100, FMOD_INIT_NORMAL, 0)))
    return Logger::writeErrorLog("Could not initialize FMOD sound system");
    
  if (!SoundManager::errorCheck(soundSystem->setFileSystem(SoundManager::fileOpen, SoundManager::fileClose, SoundManager::fileRead, SoundManager::fileSeek, 2048)))
    return Logger::writeErrorLog("Could not apply FMOD file system settings");
    
  return true;
}

FMOD_RESULT F_CALLBACK SoundManager::fileOpen(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
{
  ArchivedFile* file = FileSystem::checkOut(name);
  
  if (!file)
    return FMOD_ERR_FILE_NOTFOUND;
    
  *filesize = file->size;
  *handle = file;
  
  return FMOD_OK;
}

FMOD_RESULT F_CALLBACK SoundManager::fileClose(void *handle, void *userdata)
{
  if (!handle)
    return FMOD_ERR_INVALID_PARAM;
    
  FileSystem::checkIn((ArchivedFile*)handle);
  
  return FMOD_OK;
}

FMOD_RESULT F_CALLBACK SoundManager::fileRead(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
{
  if (!handle)
    return FMOD_ERR_INVALID_PARAM;
    
  if (bytesread)
  {
    ArchivedFile* p = (ArchivedFile*) handle;
    *bytesread = p->read(buffer, sizebytes);
    
    if (*bytesread < sizebytes)
      return FMOD_ERR_FILE_EOF;
  }
  
  return FMOD_OK;
}

FMOD_RESULT F_CALLBACK SoundManager::fileSeek(void *handle, unsigned int pos, void *userdata)
{
  if (!handle)
    return FMOD_ERR_INVALID_PARAM;
    
  ArchivedFile* p = (ArchivedFile*) handle;
  p->seek(pos, SEEKA);
  
  return FMOD_OK;
}

void SoundManager::update()
{
  soundSystem->update();
}

void SoundManager::playSong(const char* path)
{
  bool isPlaying = false;
  if (channel)
    if (errorCheck(channel->isPlaying(&isPlaying)))
      if (isPlaying)
        return;
        
  if (!errorCheck(soundSystem->createStream(path, FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &song)))
  {
    Logger::writeWarningLog("Could not create song stream");
    return;
  }
  
  if (!errorCheck(soundSystem->playSound(FMOD_CHANNEL_FREE, song, false, &channel)))
  {
    Logger::writeWarningLog("Could not play song");
    return;
  }
}

void SoundManager::playSound(const char* path)
{
}

void SoundManager::stopSong()
{
  if (channel)
  {
    channel->stop();
    channel = 0;
  }
  if (song)
  {
    song->release();
    song = 0;
  }
}

bool SoundManager::errorCheck(FMOD_RESULT result)
{
  if (result != FMOD_OK)
    return Logger::writeErrorLog(String("FMOD error: ") + FMOD_ErrorString(result));
    
  return true;
}

void SoundManager::destroy()
{
  if (song)
    song->release();
  soundSystem->close();
  soundSystem->release();
}