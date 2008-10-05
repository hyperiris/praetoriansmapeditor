#ifndef OC_HASHTABLE_H_

// Specialized versions of the HashTableTs that we use all the time.
// Many times, Keys are strings and 8 buckets is more than enough.

// ///////////////////////////////////////////// Include Files

#include "ocport.h"
#include "ocstring.h"
#include "ochashtablet.h"


// ///////////////////////////////////////////// Forwards

// Needed so the iterators can properly reference the tables.
template <class K, class V, int_u4 bucketLists>
class HashTableT;

template <class V>
class HashTable;

#define OC_BUCKETS 8

// ///////////////////////////////////////////// The HashTableIterator

template <class V>
class HashTableIterator : public HashTableTIterator<string, V, OC_BUCKETS>
{

  public:
  
    // ///// Methods
    
    // Constructor
    HashTableIterator(HashTable<V>& table) :
        HashTableTIterator<string, V, OC_BUCKETS>(table)
    { }
    
}; // HashTableIterator


// ///////////////////////////////////////////// The HashTable

template <class V>
class HashTable : public HashTableT<string, V, OC_BUCKETS>
{
    // Everything from Hashtable!
};


#define OC_HASHTABLE_H_
#endif



