#include "stdafx.h"
#include "MaterialMonitorFactory.h"

namespace RSCommon
{
	MaterialMonitorFactory& MaterialMonitorFactory::GetInstance()
	{
		static MaterialMonitorFactory instance;
		return instance;
	}

	IMaterialMonitor* MaterialMonitorFactory::CreateMaterialMonitor(pfnCreateMaterialMonitor createMaterialMonitor)
	{
		if (m_materialMonitor == NULL)
		{
			if (createMaterialMonitor != NULL)
			{
				m_materialMonitor = createMaterialMonitor();
			}
		}

		return m_materialMonitor;
	}
}