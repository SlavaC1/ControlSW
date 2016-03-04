#ifndef MaterialMonitorFactoryH
#define MaterialMonitorFactoryH

#include "IMaterialMonitor.h"

namespace RSCommon
{
	typedef IMaterialMonitor*(*pfnCreateMaterialMonitor)();

	class RSCOMMON_LIB MaterialMonitorFactory
	{
	public:
		static MaterialMonitorFactory& GetInstance();
		IMaterialMonitor* CreateMaterialMonitor(pfnCreateMaterialMonitor);

	private:
		MaterialMonitorFactory() { m_materialMonitor = NULL; }
		MaterialMonitorFactory(const MaterialMonitorFactory &) {}
		~MaterialMonitorFactory() { }

	private:
		IMaterialMonitor* m_materialMonitor;
	};
}

#endif
