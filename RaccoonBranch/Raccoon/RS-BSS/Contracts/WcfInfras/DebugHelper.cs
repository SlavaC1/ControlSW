using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WcfInfras
{
    public static class DebugHelper
    {
        public const bool IncludeExceptionDetailInFaults =
#if DEBUG
            true;
#else
            false;
#endif
    }
}
