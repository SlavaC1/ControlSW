#include "stdafx.h"
#include "IDTagAdapterFactory.h"

namespace StratasysIDTSrv
{
	IDTagAdapter* IDTagAdapterFactory::CreateIDTagAdapter(enAdapterType adapterType)
	{
		switch (adapterType)
		{
		case VaultIC100:
			return new VaultIC100Adapter();
		default:
			return new VaultIC100Adapter();
		}
	}
}