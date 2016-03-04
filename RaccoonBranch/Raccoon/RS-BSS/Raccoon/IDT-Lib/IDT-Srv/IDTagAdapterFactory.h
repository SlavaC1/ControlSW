#ifndef IDTagAdapterFactoryH
#define IDTagAdapterFactoryH

#include "VaultIC100Adapter.h"
#include "IMaterialMonitor.h"

using namespace RSCommon;

namespace StratasysIDTSrv
{
	class IDTagAdapterFactory
	{
	public:
		IDTagAdapterFactory() {}
		~IDTagAdapterFactory() {}

		static IDTagAdapter *CreateIDTagAdapter(enAdapterType adapterType = VaultIC100);
	};
}

#endif