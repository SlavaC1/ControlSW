using CBS.BSP;
using CBS.BSP.HSM;
using CBS.BSP.Report;
using CBS.BSP.TagInfoProvider;
using CBS.BSP.VLL;
using CBS.BspHost.Properties;
using log4net;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.ServiceModel;
using System.Text;
using System.Threading.Tasks;

//Here is the once-per-application setup information
[assembly: log4net.Config.XmlConfigurator(Watch = true)]

namespace CBS.BspHost
{
    class Program
    {
        static void Main(string[] args)
        {
            using (ServiceHost HsmServiceHost = new ServiceHost(typeof(HsmService)),
                TagInfoProviderServiceHost = new ServiceHost(typeof(TagInfoProviderService)),
                ReportServiceHost = new ServiceHost(typeof(ReportService)),
                VllServiceHost = new ServiceHost(typeof(VllService)))
            {
                var log = LogManager.GetLogger("root");
                log.Info(Resources.BspServicesHostStarted);

                HsmServiceHost.Open();
                TagInfoProviderServiceHost.Open();
                ReportServiceHost.Open();
                VllServiceHost.Open();

                Console.ReadLine();

                LogManager.Shutdown();
            }
        }
    }
}
