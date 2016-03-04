namespace CBS.BspHostService
{
    partial class ProjectInstaller
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.bspServiceProcessInstaller = new System.ServiceProcess.ServiceProcessInstaller();
            this.bspServiceInstaller = new System.ServiceProcess.ServiceInstaller();
            // 
            // bspServiceProcessInstaller
            // 
            this.bspServiceProcessInstaller.Account = System.ServiceProcess.ServiceAccount.NetworkService;
            this.bspServiceProcessInstaller.Password = null;
            this.bspServiceProcessInstaller.Username = null;
            // 
            // bspServiceInstaller
            // 
            this.bspServiceInstaller.Description = "Provides burning information for Raccoon burning station";
            this.bspServiceInstaller.ServiceName = "Burning Software Controller";
            this.bspServiceInstaller.StartType = System.ServiceProcess.ServiceStartMode.Automatic;
            // 
            // ProjectInstaller
            // 
            this.Installers.AddRange(new System.Configuration.Install.Installer[] {
            this.bspServiceProcessInstaller,
            this.bspServiceInstaller});

        }

        #endregion

        private System.ServiceProcess.ServiceProcessInstaller bspServiceProcessInstaller;
        private System.ServiceProcess.ServiceInstaller bspServiceInstaller;
    }
}