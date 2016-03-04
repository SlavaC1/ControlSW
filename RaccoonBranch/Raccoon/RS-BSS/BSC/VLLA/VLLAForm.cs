using CBS.VLL.Contracts;
using CBS.VLLA.Properties;
using System;
using System.Configuration;
using System.IO;
using System.Net;
using System.ServiceModel;
using System.Threading.Tasks;
using System.Windows.Forms;
using WcfInfras.Client;

namespace CBS.VLLA
{
    public partial class VLLAForm : Form, IVllCallback
    {
        #region Private Fields

        private bool ignoreMessages;

        private ProxyInvoker<IVll> proxyInvoker;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="VLLAForm"/> class.
        /// </summary>
        public VLLAForm()
        {
            InitializeComponent();
            System.Configuration.Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
            KeyValueConfigurationElement configElement;

            configElement = config.AppSettings.Settings["Domain"];
            if (configElement != null)
            {
                txtDomain.Text = configElement.Value;
            }
        }

        #endregion Public Constructors

        #region Public Methods

        public void OnLoadingCompleted(int listCount, int numberOfTagsToLoad, int newTagsCount, int usedTagsCount)
        {
            if (InvokeRequired)
            {
                this.Invoke(
                    new Action<int, int, int, int>(OnLoadingCompleted),
                    listCount, numberOfTagsToLoad, newTagsCount, usedTagsCount);
            }
            else
            {
                lblMessage.Text = String.Format(Resources.VllLoadingCompleted, listCount, numberOfTagsToLoad, newTagsCount, usedTagsCount);
            }
        }

        public void ReportProgress(string tagSerialNumber, int index, int tagsCount)
        {
            if (InvokeRequired)
            {
                this.Invoke(
                    new Action<string, int, int>(ReportProgress),
                    tagSerialNumber, index, tagsCount);
            }
            else
            {
                if (!ignoreMessages)
                {
                    if (tagSerialNumber != null)
                    {
                        lblMessage.Text = String.Format(Resources.VllLoadingProgress, tagSerialNumber, index, tagsCount);
                    }
                    else if (tagsCount == 0)
                    {
                        lblMessage.Text = Resources.ParsingList;
                    }
                    else
                    {
                        lblMessage.Text = Resources.EliminatingConflicts;
                    }
                }
            }
        }

        #endregion Public Methods

        #region Private Methods

        private async Task Authenticate()
        {
            ignoreMessages = true;

            bool rejected = true;
            try
            {
                lblMessage.Clear();

                ProxyInvoker<IVll> tmpProxyInvoker = new ProxyInvoker<IVll>(this, "VLLEndPoint");
                await tmpProxyInvoker.InvokeAsync((vll) =>
                {
                    return vll.LoadVaultICListFileAsync(name: String.Empty, content: String.Empty);
                });

                rejected = tmpProxyInvoker.IsRejected;
            }
            catch
            {
            }

            lblMessage.Clear();
            lblUser.Visible = txtUser.Visible =
                lblPassword.Visible = txtPassword.Visible =
                lblDomain.Visible = txtDomain.Visible =
                btnOk.Visible = rejected;

            btnLoadToDB.Enabled = txtFileName.Enabled = btnShowOpenFileDialog.Enabled = !rejected;

            if (!rejected)
            {
                proxyInvoker = new ProxyInvoker<IVll>(this, "VLLEndPoint");
                proxyInvoker.Error += (s, args) => ShowMessage(args.Error);
                proxyInvoker.Exception += (s, args) => ShowMessage(args.Exception.Message);

                this.AcceptButton = btnShowOpenFileDialog;
            }
        }

        private async void btnLoadToDB_Click(object sender, EventArgs e)
        {
            try
            {
                ignoreMessages = false;
                lblMessage.Clear();

                string content = String.Empty;
                using (StreamReader sr = new StreamReader(txtFileName.Text))
                {
                    content = sr.ReadToEnd();
                }

                //DuplexChannelFactory<IVll> channelFactory = new DuplexChannelFactory<IVll>(this, "VLLEndPoint");
                //IVll vllClient = channelFactory.CreateChannel();
                //await vllClient.LoadVaultICListFileAsync(Path.GetFullPath(txtFileName.Text), content);

                proxyInvoker.CreateProxy();
                lblMessage.Text = Resources.LoadingStarted;
                await proxyInvoker.InvokeAsync((vll) =>
                    {
                        return vll.LoadVaultICListFileAsync(Path.GetFullPath(txtFileName.Text), content);
                    });
            }
            catch (Exception ex)
            {
                lblMessage.Text = ex.Message;
            }
            finally
            {
                proxyInvoker.RemoveProxy();
            }
        }

        private async void btnOk_Click(object sender, EventArgs e)
        {
            ProxyInvokerCredentials.ClientCredential = new NetworkCredential(txtUser.Text, txtPassword.Text, txtDomain.Text);
            await Authenticate();
        }

        private void btnShowOpenFileDialog_Click(object sender, EventArgs e)
        {
            DialogResult result = openFileDialog1.ShowDialog();
            if (result == DialogResult.OK)
            {
                txtFileName.Text = openFileDialog1.FileName;
                this.AcceptButton = btnLoadToDB;
            }
        }

        private void ShowMessage(string message)
        {
            if (InvokeRequired)
            {
                this.Invoke(new Action<string>(ShowMessage), message);
            }
            else
            {
                lblMessage.Text = message;
            }
        }

        private async void VLLAForm_Load(object sender, EventArgs e)
        {
            await Authenticate();
        }

        #endregion Private Methods
    }
}