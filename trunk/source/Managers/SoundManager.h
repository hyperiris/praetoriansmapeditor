#ifndef SOUNDMANAGER
#define SOUNDMANAGER

#include "../fmod/inc/fmod.hpp"

class SoundManager
{
  public:
    static bool initialize();
    static void destroy();
    static void update();
    
    static void playSong(const char* path);
    static void playSound(const char* path);
    
    static void stopSong();
    
    static FMOD_RESULT F_CALLBACK fileOpen(const char *name,
                                           int unicode,
                                           unsigned int *filesize,
                                           void **handle,
                                           void **userdata);
                                           
    static FMOD_RESULT F_CALLBACK fileClose(void *handle,
                                            void *userdata);
                                            
    static FMOD_RESULT F_CALLBACK fileRead(void *handle,
                                           void *buffer,
                                           unsigned int sizebytes,
                                           unsigned int *bytesread,
                                           void *userdata);
                                           
    static FMOD_RESULT F_CALLBACK fileSeek(void *handle,
                                           unsigned int pos,
                                           void *userdata);
                                           
  private:
    static bool errorCheck(FMOD_RESULT result);
    
  private:
    static FMOD::System   *soundSystem;
    static FMOD::Sound    *song;
    static FMOD::Sound    *sound;
    static FMOD::Channel  *channel;
};

#endif