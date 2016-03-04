#include "stdafx.h"
#include "UniqueLock.h"

namespace RSCommon
{
    UniqueLock::UniqueLock(pthread_mutex_t mtx)
    {
        m_mtx = mtx;
        pthread_mutex_lock(&m_mtx);
    }

    UniqueLock::~UniqueLock()
    {
        pthread_mutex_unlock(&m_mtx);
    }
}
