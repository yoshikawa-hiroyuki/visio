//
// vsnDataCache
//
#include "vsnDataCache.h"
#include "vsnError.h"
#include "vfrDefs.h"
#include <algorithm>

using namespace std;
using namespace VFR;


//----------------------------------------------------------------
// class vsnDataCache
//----------------------------------------------------------------

/* static members */

const size_t vsnDataCache::DefaultCacheSize = 5;


/* constructors / destructor */

vsnDataCache::vsnDataCache(const size_t csz)
  : m_cacheReady(false), m_totalSteps(0), m_cacheSteps(csz),
    m_cacheUnitSize(0), m_dataPool(NULL)
{
}

vsnDataCache::~vsnDataCache() {
  if ( m_dataPool ) DeAllocate(m_dataPool);
  m_cacheList.clear();
}


vsnDataCache::CacheType vsnDataCache::isCached(const size_t stp) const {
  if ( ! isCacheReady() ) return OutOfRange;
  if ( stp >= m_totalSteps ) return OutOfRange;

  deque<CacheUnit>::iterator it;
  for ( it = m_cacheList.begin(); it != m_cacheList.end(); it++ ) {
    if ( it->m_used && it->m_step == stp )
      return Cached;
  } // end of for(it)

  return OutOfCache;
}


const unsigned char* const vsnDataCache::getData(const size_t stp) {
  if ( ! isCacheReady() ) return NULL;

  deque<CacheUnit>::iterator it;
  switch ( isCached(stp) ) {
  case Cached: {
    for ( it = m_cacheList.begin(); it != m_cacheList.end(); it++ )
      if ( it->m_used && it->m_step == stp )
	return it->p_data;
    return NULL; // not found...
  }
  case OutOfCache: {
    // check empty pool
    for ( it = m_cacheList.begin(); it != m_cacheList.end(); it++ )
      if ( ! it->m_used ) break;
    if ( it == m_cacheList.end() ) { // no empty pool
      // rotate cache list
      rotate(m_cacheList.begin(), m_cacheList.begin()+1, m_cacheList.end());
      // override the last unit
      it = m_cacheList.end()-1;
    } else
      it->m_used = true;

    // read data into it->p_data
    if ( ! readin(stp, it->p_data) ) {
      it->m_used = false;
      return NULL;
    }
    it->m_step = stp;
    return it->p_data;
  }
  default:
    break;
  } // end of switch(ct)

  return NULL;
}


//----------------------------------------------------------------
// class vsnDataCacheUF
//----------------------------------------------------------------

/* constructors / destructor */

vsnDataCacheUF::vsnDataCacheUF(const size_t csz)
  : vsnDataCache(csz), m_cacheFILE(NULL)
{
}

vsnDataCacheUF::~vsnDataCacheUF() {
  if ( m_cacheFILE )
    fclose(m_cacheFILE);
}


/* methods */

bool vsnDataCacheUF::setupCachePool(const std::string& path) {
  string msgHdr = string("setupCachePool: ");

  // invalidate
  m_cacheReady = false;
  m_cachePath = string("");
  if ( m_cacheFILE ) {
    fclose(m_cacheFILE);
    m_cacheFILE = NULL;
  }

  // setup in derived class
  if ( ! setupUF(path) ) return false;
  m_cachePath = path;

  size_t sz = m_cacheSteps * m_cacheUnitSize;
  if ( sz < 1 ) return false;

  // allocate dataPool
  m_dataPool = (unsigned char*)ReAllocate(m_dataPool, sz);
  while ( ! m_dataPool ) {
    sz = --m_cacheSteps * m_cacheUnitSize;
	if ( sz < 1 ) {
      ErrMsg(MsgERR, msgHdr + string("out of memory"));
      return false;
    }
    m_dataPool = (unsigned char*)Allocate(sz);
  } // end of while
  if ( ! m_dataPool ) {
    ErrMsg(MsgERR, msgHdr + string("out of memory"));
    return false;
  }

  // resize cacheList
  m_cacheList.resize(m_cacheSteps);
  for ( int i = 0; i < m_cacheSteps; i++ ) {
    m_cacheList[i].m_used = false;
    m_cacheList[i].m_step = 0;
    m_cacheList[i].p_data = &m_dataPool[i * m_cacheUnitSize];
  } // end of for(i)

  m_cacheReady = true;
  return true;
}


//----------------------------------------------------------------
// class vsnDataCacheMF
//----------------------------------------------------------------

/* constructors / destructor */

vsnDataCacheMF::vsnDataCacheMF(const size_t csz)
  : vsnDataCache(csz)
{
}

vsnDataCacheMF::~vsnDataCacheMF() {
}


/* methods */

bool vsnDataCacheMF::setupCachePool(const std::deque<std::string>& path_lst) {
  string msgHdr = string("setupCachePool: ");

  // invalidate
  m_cacheReady = false;
  m_cachePathes.clear();

  // setup in derived class
  deque<string> new_path_lst;
  if ( ! setupMF(path_lst, new_path_lst) ) return false;
  m_cachePathes = new_path_lst;

  size_t sz = m_cacheSteps * m_cacheUnitSize;
  if ( sz < 1 ) return false;

  // allocate dataPool
  m_dataPool = (unsigned char*)ReAllocate(m_dataPool, sz);
  while ( ! m_dataPool ) {
    sz = --m_cacheSteps * m_cacheUnitSize;
	if ( sz < 1 ) {
      ErrMsg(MsgERR, msgHdr + string("out of memory"));
      return false;
    }
    m_dataPool = (unsigned char*)Allocate(sz);
  } // end of while
  if ( ! m_dataPool ) {
    ErrMsg(MsgERR, msgHdr + string("out of memory"));
    return false;
  }

  // resize cacheList
  m_cacheList.resize(m_cacheSteps);
  for ( int i = 0; i < m_cacheSteps; i++ ) {
    m_cacheList[i].m_used = false;
    m_cacheList[i].m_step = 0;
    m_cacheList[i].p_data = &m_dataPool[i * m_cacheUnitSize];
  } // end of for(i)

  m_cacheReady = true;
  return true;  
}


std::string vsnDataCacheMF::getCachePath(const size_t stp) const {
  if ( ! isCacheReady() ) return string("");
  if ( stp >= m_totalSteps ) return string("");

  return m_cachePathes[stp];
}
