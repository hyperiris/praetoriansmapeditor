#include "Archive.h"
#include "../zlib/zlib.h"
#include "../Math/MathUtils.h"
#include "../Tools/Logger.h"

enum ZipState
{
  ENCRYPTED,
  NOT_ENCRYPTED,
  NOT_RECOGNIZED
};

Archive::Archive(const char* name)
{
  archivename = name;
}

bool Archive::open(const char* path)
{
  ZipDirectory  zd;
  ZipEntry      zde;
  ArchivedFile  cendir;
  ArchivedFile  dirent;
  unsigned int  sig;
  unsigned char entryname[256];
  
  if (!archiveMap.createMapping(path))
    return false;
    
  //CENTRAL DIRECTORY
  archiveMap.createView("cendir", archiveMap.getFileSize() - 22, 22, &cendir);
  
  cendir.read(&sig, 4);
  
  if (NOT_RECOGNIZED == (state = verifyZip(sig)))
  {
    archiveMap.close();
    return false;
  }
  
  cendir.seek(4, SEEKD);
  
  cendir.read(&zd.zipentrcnt, 2);
  cendir.read(&zd.zipentrtot, 2);
  cendir.read(&zd.zipdirsize, 4);
  cendir.read(&zd.zipdirdata, 4);
  
  if (ENCRYPTED == state)
  {
    decryptulong(zd.zipdirsize);
    decryptulong(zd.zipdirdata);
    decryptushort(zd.zipentrcnt);
    decryptushort(zd.zipentrtot);
  }
  
  if (zd.zipentrcnt == 0)
  {
    archiveMap.close();
    return false;
  }
  
  archiveMap.unmapView(&cendir);
  
  //DIRECTORY ENTRIES
  archiveMap.createView("zipdirent", zd.zipdirdata, zd.zipdirsize, &dirent);
  
  for (int i = 0; i < zd.zipentrcnt; i++)
  {
    dirent.seek(10, SEEKD);
    dirent.read(&zde.zipcomp, 2);
    dirent.seek(8, SEEKD);
    dirent.read(&zde.zipsize, 4);
    dirent.read(&zde.zipuncmp, 4);
    dirent.read(&zde.zipfnln, 2);
    dirent.read(&zde.zipxtraln, 2);
    dirent.seek(6, SEEKD);
    dirent.read(&zde.zipattr, 4);
    dirent.read(&zde.zipdata, 4);
    
    if (ENCRYPTED == state)
    {
      decryptulong(zde.zipsize);
      decryptulong(zde.zipdata);
      decryptulong(zde.zipattr);
      decryptulong(zde.zipuncmp);
      decryptushort(zde.zipcomp);
      decryptushort(zde.zipfnln);
      decryptushort(zde.zipxtraln);
    }
    
    dirent.read(entryname, zde.zipfnln);
    
    if (ENCRYPTED == state)
      for (int j = 0; j < zde.zipfnln; j++)
        decryptuchar(entryname[j]);
        
    entryname[zde.zipfnln] = '\0';
    
    zde.zipname = ICString((char*)entryname);
    
    if ((zde.zipattr & ZipEntry::FILE) && !(zde.zipattr & ZipEntry::DIRECTORY))
      entries.insertKeyAndValue(zde.zipname, zde);
      
    if (zde.zipattr & ZipEntry::DIRECTORY)
      directories.append(zde.zipname);
      
    dirent.seek(zde.zipxtraln, SEEKD);
  }
  
  archiveMap.unmapView(&dirent);
  
  return true;
}

bool Archive::findEntry(const char* name, ZipEntry* entry)
{
  if (!name || !entry)
    return false;
    
  if (entries.findValue(name, *entry))
    return true;
    
  for (unsigned int i = 0; i < directories.length(); i++)
    if (entries.findValue(directories(i) + name, *entry))
      return true;
      
  ICString::size_type i;
  AVLTreeTIterator <ICString, ZipEntry, 4> iter(entries);
  
  while (iter.next())
  {
    i = iter.key().find(name);
    
    if (i != ICString::npos)
      if (iter.key().substr(i) == name)
      {
        *entry = iter.value();
        return true;
      }
  }
  
  return false;
}

bool Archive::containsEntry(const char* name)
{
  if (!name)
    return false;
    
  if (entries.contains(name))
    return true;
    
  for (unsigned int i = 0; i < directories.length(); i++)
    if (entries.contains(directories(i) + name))
      return true;
      
  ICString::size_type i;
  AVLTreeTIterator <ICString, ZipEntry, 4> iter(entries);
  
  while (iter.next())
  {
    i = iter.key().find(name);
    if (i != ICString::npos)
      if (iter.key().substr(i) == name)
        return true;
  }
  
  return false;
}

unsigned int Archive::verifyZip(unsigned int sig)
{
  return (sig == 0x06054b50) ? NOT_ENCRYPTED : ((sig ^ 0xabababab) == 0x06054b50) ? ENCRYPTED : NOT_RECOGNIZED;
}

bool Archive::unpackEntry(ZipEntry* entry, unsigned char* out)
{
  if (!entry || !out)
    return false;
    
  ArchivedFile ent;
  unsigned short xtra;
  unsigned char* data;
  bool success = false;
  
  //begin hack
  archiveMap.createView(entry->zipname.c_str(), entry->zipdata, entry->zipsize + 30 + entry->zipfnln + entry->zipxtraln, &ent);
  
  ent.seek(28, SEEKD);
  ent.read(&xtra, 2);
  
  if (ENCRYPTED == state)
    decryptushort(xtra);
    
  ent.seek(entry->zipfnln + xtra, SEEKD);
  //end hack
  
  if (entry->zipcomp == 0x0000)
  {
    ent.read(out, entry->zipsize);
    
    if (ENCRYPTED == state)
      for (unsigned int j = 0; j < entry->zipsize; j++)// <-- ?
        decryptuchar(out[j]);
        
    success = true;
  }
  else if (entry->zipcomp == 0x0008)
  {
    data = new unsigned char[entry->zipsize];
    memset(data, 0, entry->zipsize);
    
    ent.read(data, entry->zipsize);
    
    if (ENCRYPTED == state)
      for (unsigned int j = 0; j < entry->zipsize; j++)
        decryptuchar(data[j]);
        
    z_stream inst;
    
    inst.next_in    = (Bytef*)data;
    inst.next_out   = (Bytef*)out;
    inst.avail_out  = (uInt)entry->zipuncmp;
    inst.avail_in   = (uInt)entry->zipsize;
    inst.zalloc     = (alloc_func)0;
    inst.zfree      = (free_func)0;
    
    //if (Z_OK == inflateInit2(&inst, -MAX_WBITS))
    //  if (Z_STREAM_END == inflate(&inst, Z_FINISH))
    //    success = true;
    
    inflateInit2(&inst, -MAX_WBITS);
    int err = inflate(&inst, Z_FINISH);
    inflateEnd(&inst);
    success = true;
    
    deleteArray(data);
  }
  
  archiveMap.unmapView(&ent);
  
  return success;
}

void Archive::decryptulong(unsigned int& v)
{
  v ^= 0xabababab;
}

void Archive::decryptuchar(unsigned char& v)
{
  v ^= 0xab;
}

void Archive::decryptushort(unsigned short& v)
{
  v ^= 0xabab;
}

void Archive::printAllPaths()
{
  if (!archivename.getLength())
    return;
    
  for (unsigned int i = 0; i < directories.length(); i++)
    Logger::writeInfoLog(directories(i).c_str());
}

void Archive::printAllEntries()
{
  if (!archivename.getLength())
    return;
    
  AVLTreeTIterator <ICString, ZipEntry, 4> iter(entries);
  while (iter.next())
    Logger::writeInfoLog(iter.key().c_str());
}

void Archive::close()
{
  archiveMap.close();
}

Archive::~Archive()
{
  close();
}
