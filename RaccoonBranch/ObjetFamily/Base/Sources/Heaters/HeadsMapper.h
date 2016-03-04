#ifndef _HEADS_MAPPER_H_
#define _HEADS_MAPPER_H_

#include "HeatersDefs.h"

// Exception class for all the QLib RTTI elements
class EHeadMapper : public EQException {
  public:
    EHeadMapper(const QString& ErrMsg,const TQErrCode ErrCode=0) : EQException(ErrMsg,ErrCode) {}
};


// Template class for redirecting Eden450 8-heads into Eden250 4-heads.
template<class T, int TSize, char* TmyID>
class CQHeadsMapper {
  public:

    // Constructor
    CQHeadsMapper(void);

    // Constructor
    CQHeadsMapper(T* arr);

    void Init(void);

    // Destructor
    ~CQHeadsMapper()
    {
      //if (m_Map)
      //  delete []m_Map;
    }

    // A Setter function, the head element is mapped into its physical location
    void Map(T& Element, unsigned int location);
    int AssignMapper(int *newmapper, int size);

    // A Getter function, the head element is retreived by the lookup table.
    T& Lookup(unsigned int location);

    // A Getter function, the head element is retreived by physical location.
    T& Get(unsigned int location);

    // A Setter function, the head element is mapped by physical location.
    void Put(T& Element, unsigned int location);

    // An operator[] this enables to maintain old code that uses a simple array.
    T& operator[](int location);

    // validity checking methods:
    bool IsValid(int location);
    bool IsValidIndirect(int location);

  private:

    // to make this class thread-safe
    // CQMutex m_MutexHeadsMapper;

    char m_myID[256]; // todo -oNobody -cNone: change the 256
    int m_TSize; // to be able to see the template's TSize.

    // a flag to indicate structure validity.
    bool m_IsValid;

    // array for the original objects:
    typedef std::vector<T> TVector;

    // this is only for debug purpose.
    // this array holds pointers to the data in the STL vector.
    T* mp_Tarr[TSize];

    TVector m_Items;

    // array of ints for the remap
    int m_Map[TSize];
    
};

// Constructors:
template<class T, int TSize, char* TmyID>
CQHeadsMapper<T,TSize,TmyID>::CQHeadsMapper(void)
{
//  m_MutexHeadsMapper.WaitFor();

  Init();
//  m_MutexHeadsMapper.Release();
};

template<class T, int TSize, char* TmyID>
CQHeadsMapper<T,TSize,TmyID>::CQHeadsMapper(T* arr)
{
//  m_MutexHeadsMapper.WaitFor();

  Init();
//  int init_array_size = sizeof(*T)/sizeof(T[0]);

// todo -oNobody -cNone: make sure that the size of the initialization 'arr'
// matches that of the template derivative we're constructing.
//  if (init_array_size <= TSize)
    // todo -oNobody -cNone: throw exception here.
//    init_array_size = TSize;

  for (int i=0; i < TSize; i++)
   (*this)[i] = arr[i];

//  m_MutexHeadsMapper.Release();
}


template<class T, int TSize, char* TmyID>
void CQHeadsMapper<T,TSize,TmyID>::Init(void)
{
  T temp;

  strcpy (m_myID, TmyID);
  m_TSize = TSize;


#ifdef EDEN_250
  // Mapping from native-logical head reference to physical.
  // Logical 7 is S3. NA in Eden250. Physical M0..M3 gets this dummy value.
  // i didn't initialize to mapper[TSize] so that the sizeof(mapper) will give actual size. see next lines.
  // int mapper[8] = {4, 4, 0, 1, 4, 4, 2, 3};

  // int mapper[8] = {2, 3, 6, 7, 4, 4, 4, 4}; // before switching M & S blocks.
  int mapper[TOTAL_NUMBER_OF_HEATERS] = {6, 7, 2, 3, 4, 4, 4, 4, 11 ,10 ,9 ,8, 12}; // after switching M & S blocks. and, Front/End
  //int mapper[12] = {6, 7, 2, 3, 4, 4, 4, 4, 10 ,11 ,8 ,9}; // after switching M & S blocks.

  // (not used) Mapping from native-physical head reference to logical.
  // int mapper[8] = {7, 7, 0, 1, 7, 7, 2, 3};
#else
  // All others mapping "1:1" Mapping from native-physical head reference to logical.
#ifdef OBJET_MACHINE
  int mapper[TOTAL_NUMBER_OF_HEATERS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
#else
  int mapper[TOTAL_NUMBER_OF_HEATERS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
#endif  
#endif

  // allocate the mapper array:
  //m_Map = new int[TSize];

  // initialize the arrays with defaults:
  m_IsValid = true;
  for (int i=0; i<TSize; i++)
  {
    // allocate the vector with dummy objects, so that we can use it later with operator[]
    m_Items.push_back(temp);

    // mp_Tarr is for debug only.
    // mp_Tarr[i] = NULL;

	if (static_cast<unsigned>(i) < (sizeof(mapper)/sizeof(mapper[0]))) // actual items count given in mapper init.
    {  // set the m_Map array according to mapper values
      m_Map[i] = mapper[i];
    }
	else
    {
      // set the m_Map array to a "one to one" map, in case no values were given.
      m_Map[i] = i;
    }
  }

 // int xxx = m_Map[100];
//  T* yyy = mp_Tarr[100];
}

template<class T, int TSize, char* TmyID>
T& CQHeadsMapper<T,TSize,TmyID>::operator[](int location)
{

  // mp_Tarr is for debug
  //for (int i=0; i<TSize; i++)
  //  mp_Tarr[i] = &m_Items[i];

  if (IsValid(location))
    return m_Items[location];
  else
    throw EHeadMapper("EHeadMapper::operator[] access violation.");

//    return m_Items[0]; // todo -oNobody -cNone: throw exception
}

template<class T, int TSize, char* TmyID>
T& CQHeadsMapper<T,TSize,TmyID>::Lookup(unsigned int location)
{
  // mp_Tarr is for debug
  //for (int i=0; i<TSize; i++)
  //  mp_Tarr[i] = &m_Items[i];

  if (IsValidIndirect(location))
    return m_Items[m_Map[location]];
  else
    throw EHeadMapper("EHeadMapper::Lookup() access violation.");

 //   return m_Items[0]; // todo -oNobody -cNone: throw exception
};
/////////////////////////////////////


template<class T, int TSize, char* TmyID>
void CQHeadsMapper<T,TSize,TmyID>::Map(T& Element, unsigned int location)
{
  if (m_IsValid)
    m_Items[m_Map[location]] = Element;
};

template<class T, int TSize, char* TmyID>
int CQHeadsMapper<T,TSize,TmyID>::AssignMapper(int *newmapper, int size)
{
  if (size > TSize)
    return -1;

  for (int i=0; i<size; i++)
   m_Map[i] = newmapper[i];

  return 0;
};



template<class T, int TSize, char* TmyID>
T& CQHeadsMapper<T,TSize,TmyID>::Get(unsigned int location)
{
  if (m_IsValid)
    return m_Items[location];
};

template<class T, int TSize, char* TmyID>
void CQHeadsMapper<T,TSize,TmyID>::Put(T& Element, unsigned int location)
{
  if (m_IsValid)
    m_Items[location] = Element;
}

template<class T, int TSize, char* TmyID>
bool CQHeadsMapper<T,TSize,TmyID>::IsValid(int location)
{
  if (location <0 || location >= TSize)
    return false;

  int vect_size = m_Items.size();

  if (vect_size != TSize)
    return false;

  return true;
};

template<class T, int TSize, char* TmyID>
bool CQHeadsMapper<T,TSize,TmyID>::IsValidIndirect(int location)
{
  if (!IsValid(location) || !IsValid(m_Map[location]))
    return false;

  return true;
};

const char *NameOf_THeadsMapArray = "TOTAL_NUMBER_OF_HEATERS int";
typedef CQHeadsMapper <int, (int)TOTAL_NUMBER_OF_HEATERS, NameOf_THeadsMapArray> THeadsMapArray;

#endif
