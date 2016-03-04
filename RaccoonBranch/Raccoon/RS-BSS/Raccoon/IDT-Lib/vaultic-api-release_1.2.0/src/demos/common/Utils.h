#include <map>
#include "vaultic_common.h"
#include "vaultic_lib.h"

#define FREE(a)		if (a) \
					{ \
						free(a); \
						a = NULL; \
					}

typedef std::map<unsigned char,std::string> PCSC_DEVICE_LIST;

LONG getPCSCReaderList(PCSC_DEVICE_LIST &readerList);
