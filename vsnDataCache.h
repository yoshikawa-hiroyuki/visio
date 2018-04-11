//
// vsnDataCache
//
#ifndef _VSN_DATA_CACHE_H_
#define _VSN_DATA_CACHE_H_

#include <cstdio>
#include <string>
#include <deque>


//----------------------------------------------------------------
// class vsnDataCache
//----------------------------------------------------------------
class vsnDataCache {
public:
  static const size_t DefaultCacheSize;

  enum CacheType {OutOfRange =0, OutOfCache, Cached};

  struct CacheUnit {
    bool m_used;
    size_t m_step;
    unsigned char* p_data;
    CacheUnit() : m_used(false), m_step(0), p_data(NULL) {}
    CacheUnit(const CacheUnit& org) {*this = org;}
    ~CacheUnit() {};
    void operator=(const CacheUnit& org) {
      m_used = org.m_used; m_step = org.m_step; p_data = org.p_data;
    }
  };

public:
  virtual ~vsnDataCache();

  bool isCacheReady() const {return m_cacheReady;}
  CacheType isCached(const size_t stp) const;
  const unsigned char* const getData(const size_t stp);

  size_t getTotalSteps() const {return m_totalSteps;}
  size_t getCacheSteps() const {return m_cacheSteps;}
  size_t getUnitSize() const {return m_cacheUnitSize;}

  /* readin() need to do:
     - read m_cacheUnitSize bytes data into 'pdata'
   */ 
  virtual bool readin(const size_t stp, unsigned char* pdata) =0;
		    
protected:
  vsnDataCache(const size_t csz =DefaultCacheSize);

  volatile bool   m_cacheReady;
  size_t          m_totalSteps;
  size_t          m_cacheSteps;
  size_t          m_cacheUnitSize;
  mutable std::deque<CacheUnit>
                  m_cacheList; // [m_cacheSteps]
  unsigned char*  m_dataPool;  // [m_cacheSteps x m_cacheUnitSize]
};


//----------------------------------------------------------------
// class vsnDataCacheUF
//----------------------------------------------------------------
class vsnDataCacheUF : public vsnDataCache {
public:
  vsnDataCacheUF(const size_t csz =DefaultCacheSize);
  virtual ~vsnDataCacheUF();

  bool setupCachePool(const std::string& path);
  std::string getCachePath() const {return m_cachePath;}

  /* setupUF() need to do:
     - set m_totalSteps and m_cacheUnitSize
     - allocate m_fposList[m_totalSteps] and setup each element
     - change m_cacheSteps if need
   */
  virtual bool setupUF(const std::string& path) =0;

protected:
  std::string         m_cachePath;
  FILE*               m_cacheFILE;
  std::deque<fpos_t>  m_fposList;  // [m_totalSteps]
  
};


//----------------------------------------------------------------
// class vsnDataCacheMF
//----------------------------------------------------------------
class vsnDataCacheMF : public vsnDataCache {
public:
  vsnDataCacheMF(const size_t csz =DefaultCacheSize);
  virtual ~vsnDataCacheMF();

  bool setupCachePool(const std::deque<std::string>& path_lst);
  std::string getCachePath(const size_t stp) const;

  /* setupMF() need to do:
     - set m_totalSteps and m_cacheUnitSize
     - change m_cacheSteps if need
     - returns checked new_path_lst
   */
  virtual bool setupMF(const std::deque<std::string>& path_lst,
		       std::deque<std::string>& new_path_lst) =0;

protected:
  std::deque<std::string>
    m_cachePathes; // [m_totalSteps]
};

#endif // _VSN_DATA_CACHE_H_
