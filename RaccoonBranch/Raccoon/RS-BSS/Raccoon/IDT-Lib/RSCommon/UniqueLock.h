#ifndef UniqueLockH
#define UniqueLockH

#include "RSCommonDefinitions.h"
#include <pthread.h>

namespace RSCommon
{
    class RSCOMMON_LIB UniqueLock
    {
        public:
            UniqueLock(pthread_mutex_t mtx);
            ~UniqueLock();
        protected:
        private:
            pthread_mutex_t m_mtx;
    };
}

#endif
