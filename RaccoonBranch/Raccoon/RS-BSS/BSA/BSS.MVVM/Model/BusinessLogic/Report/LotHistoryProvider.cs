using BSS.Contracts;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Properties;
using Microsoft.Office.Interop.Excel;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using WcfInfras.Client;
using ExcelApplication = Microsoft.Office.Interop.Excel.Application;

namespace BSS.MVVM.Model.BusinessLogic.Report
{
    public class LotHistoryProvider
    {
        #region Private Fields

        private const char CommaSeparator = ',';

        /// <summary>
        /// The reports directory name.
        /// </summary>
        private const string ReportsDirectory = "Reports";

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="LotHistoryProvider"/> class.
        /// </summary>
        public LotHistoryProvider()
        {
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Exports report to excel.
        /// </summary>
        /// <param name="report">The report.</param>
        /// <returns></returns>
        public async Task<bool> ExportToCsv(BurningActionsStatistics report)
        {
            return await Task.Run(() =>
            {
                try
                {
                    string fileName = "Report_" + DateTime.Now.ToString("dd-MMM-yyyy_HH-mm-ss") + ".csv";
                    MessengerUtils.SendInfoMessage(String.Format(Resources.ExportingToExcel, fileName));

                    StringBuilder reportBuilder = new StringBuilder();
                    string reportHeader = BuildReportHeader();
                    reportBuilder.AppendLine(reportHeader);

                    for (int i = 0; i < report.Tags.Count; i++)
                    {
                        string reportRow = BuildReportRow(i, report.Tags[i]);
                        reportBuilder.AppendLine(reportRow);
                    }

                    if (!Directory.Exists(ReportsDirectory))
                    {
                        Directory.CreateDirectory(ReportsDirectory);
                    }

                    using (StreamWriter sw = new StreamWriter(Path.Combine(ReportsDirectory, fileName)))
                    {
                        sw.Write(reportBuilder.ToString());
                    }

                    MessengerUtils.SendInfoMessage(Resources.ExportToExcelSucceeded);
                    return true;
                }
                catch (Exception ex)
                {
                    MessengerUtils.SendException(ex);
                    return false;
                }
            }).ConfigureAwait(continueOnCapturedContext: false);
        }

        /// <summary>
        /// Exports report to excel.
        /// </summary>
        /// <param name="report">The report.</param>
        /// <returns></returns>
        public async Task<bool> ExportToExcel(BurningActionsStatistics report)
        {
            return await Task.Run(() =>
            {
                ExcelApplication app = null;
                Workbook wb = null;
                Worksheet ws = null;

                try
                {
                    string fileName = "Report_" + DateTime.Now.ToString("dd-MMM-yyyy_HH-mm-ss") + ".xlsx";
                    MessengerUtils.SendInfoMessage(String.Format(Resources.ExportingToExcel, fileName));
                    app = new ExcelApplication();
                    wb = app.Workbooks.Add(1);
                    ws = (Worksheet)wb.Worksheets[1];

                    string reportHeader = BuildReportHeader();
                    string[] headerCells = reportHeader.Split(CommaSeparator);
                    for (int j = 0; j < headerCells.Length; j++)
                    {
                        ws.Cells[1, j + 1] = headerCells[j];
                    }

                    for (int i = 0; i < report.Tags.Count; i++)
                    {
                        string reportRow = BuildReportRow(i, report.Tags[i]);
                        string[] reportRowCells = reportRow.Split(CommaSeparator);
                        for (int j = 0; j < reportRowCells.Length; j++)
                        {
                            ws.Cells[i + 2, j + 1] = reportRowCells[j];
                        }
                    }

                    if (!Directory.Exists(ReportsDirectory))
                    {
                        Directory.CreateDirectory(ReportsDirectory);
                    }

                    wb.SaveAs(Path.Combine(Directory.GetCurrentDirectory(), ReportsDirectory, fileName));
                    wb.Close();
                    app.Quit();

                    MessengerUtils.SendInfoMessage(Resources.ExportToExcelSucceeded);
                    return true;
                }
                catch (Exception ex)
                {
                    MessengerUtils.SendException(ex);
                    return false;
                }
                finally
                {
                    ReleaseComObject(ws);
                    ReleaseComObject(wb);
                    ReleaseComObject(app);
                    GC.Collect();
                }
            }).ConfigureAwait(continueOnCapturedContext: false);
        }

        /// <summary>
        /// Reads all batch numbers.
        /// </summary>
        /// <returns></returns>
        public async Task<string[]> ReadAllBatchNumbers()
        {
            MessengerUtils.SendInfoMessage(Resources.ReadingBatchNumbers);

            using (ProxyInvoker<IReport> reportProxy = ProxyInvokerUtils.CreateProxyInvoker<IReport>("ReportEndPoint"))
            {
                reportProxy.CreateProxy();
                Task<string[]> ReadAllBatchNumbersTask = null;
                await reportProxy.InvokeAsync(report =>
                    {
                        ReadAllBatchNumbersTask = report.ReadAllBatchNumbersAsync();
                        return ReadAllBatchNumbersTask;
                    });

                if (ReadAllBatchNumbersTask.Status == TaskStatus.Faulted)
                {
                    return new string[0];
                }

                await ReadAllBatchNumbersTask.ConfigureAwait(continueOnCapturedContext: false);
                if (ReadAllBatchNumbersTask.Result == null)
                {
                    return new string[0];
                }

                MessengerUtils.SendInfoMessage(Resources.BatchNumbersReadSuccessfully);
                return ReadAllBatchNumbersTask.Result;
            }
        }

        /// <summary>
        /// Reads the batch history.
        /// </summary>
        /// <param name="batchNumber">The batch number.</param>
        /// <returns></returns>
        public async Task<BurningActionsStatistics> ReadBatchHistory(string batchNumber)
        {
            MessengerUtils.SendInfoMessage(String.Format(Resources.GeneratingReport, batchNumber));

            using (ProxyInvoker<IReport> reportProxy = ProxyInvokerUtils.CreateProxyInvoker<IReport>("ReportEndPoint"))
            {
                reportProxy.CreateProxy();
                Task<BurningActionsStatistics> ReadBatchHistoryTask = null;
                await reportProxy.InvokeAsync(report =>
                {
                    ReadBatchHistoryTask = report.ReadBatchHistoryAsync(batchNumber);
                    return ReadBatchHistoryTask;
                });

                if (ReadBatchHistoryTask.Status == TaskStatus.Faulted)
                {
                    return null;
                }

                await ReadBatchHistoryTask.ConfigureAwait(continueOnCapturedContext: false);
                MessengerUtils.SendInfoMessage(String.Format(Resources.ReportGeneratedSuccessfully, batchNumber));
                return ReadBatchHistoryTask.Result;
            }
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Builds the report header.
        /// </summary>
        /// <returns></returns>
        private string BuildReportHeader()
        {
            StringBuilder headerBuilder = new StringBuilder();
            headerBuilder.Append(Resources.RowNumber.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.StationName.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.BurningDate.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.BurningTime.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.SerialNumber.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.BurningAttemptCount.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.TagStructID.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.MaterialType.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.MaterialID.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.ManufacturingTime.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.InitialWeight.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.ExpirationDate.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.Error.Replace(":", String.Empty)).Append(CommaSeparator);
            headerBuilder.Append(Resources.Reference);

            return headerBuilder.ToString();
        }

        /// <summary>
        /// Builds a row for the report from tag statistics.
        /// </summary>
        /// <param name="rowNumber">The row number.</param>
        /// <param name="tagStatistics">The tag statistics.</param>
        /// <returns></returns>
        private string BuildReportRow(int rowNumber, TagStatistics tagStatistics)
        {
            StringBuilder contentRowBuilder = new StringBuilder();
            contentRowBuilder.Append(rowNumber).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.StationName).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.BurningTime.ToString("d")).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.BurningTime.ToString("T")).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.TagInfo.SerialNumber.ToHexString()).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.BurningAttemptCount).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.TagInfo.MaterialInfo.TagStructID).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.TagInfo.MaterialInfo.MaterialName).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.TagInfo.MaterialInfo.MaterialID).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.TagInfo.MaterialInfo.ManufacturingTime.ToString("d")).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.TagInfo.MaterialInfo.InitialWeight).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.TagInfo.MaterialInfo.ExpirationDate.ToString("d")).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.LastError ?? String.Empty).Append(CommaSeparator);
            contentRowBuilder.Append(tagStatistics.IsReference ? "V" : String.Empty);

            return contentRowBuilder.ToString();
        }

        /// <summary>
        /// Releases COM object.
        /// </summary>
        /// <param name="obj">The COM object.</param>
        private void ReleaseComObject(object obj)
        {
            try
            {
                if (obj != null)
                {
                    Marshal.ReleaseComObject(obj);
                    obj = null;
                }
            }
            catch (Exception ex)
            {
                MessengerUtils.SendErrorMessage(ex.Message);
            }
        }

        #endregion Private Methods
    }
}