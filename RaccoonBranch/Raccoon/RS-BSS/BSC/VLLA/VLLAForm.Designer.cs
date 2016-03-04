namespace CBS.VLLA
{
    partial class VLLAForm
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(VLLAForm));
            this.lblSelect = new System.Windows.Forms.Label();
            this.btnShowOpenFileDialog = new System.Windows.Forms.Button();
            this.txtFileName = new System.Windows.Forms.TextBox();
            this.btnLoadToDB = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.lblMessage = new System.Windows.Forms.TextBox();
            this.pnlMain = new System.Windows.Forms.TableLayoutPanel();
            this.lblUser = new System.Windows.Forms.Label();
            this.txtUser = new System.Windows.Forms.TextBox();
            this.lblPassword = new System.Windows.Forms.Label();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.lblDomain = new System.Windows.Forms.Label();
            this.txtDomain = new System.Windows.Forms.TextBox();
            this.btnOk = new System.Windows.Forms.Button();
            this.pnlMain.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblSelect
            // 
            this.lblSelect.AutoSize = true;
            this.lblSelect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lblSelect.Location = new System.Drawing.Point(3, 108);
            this.lblSelect.Name = "lblSelect";
            this.lblSelect.Size = new System.Drawing.Size(108, 30);
            this.lblSelect.TabIndex = 0;
            this.lblSelect.Text = "Select VaultIC list file:";
            this.lblSelect.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // btnShowOpenFileDialog
            // 
            this.btnShowOpenFileDialog.Location = new System.Drawing.Point(117, 111);
            this.btnShowOpenFileDialog.Name = "btnShowOpenFileDialog";
            this.btnShowOpenFileDialog.Size = new System.Drawing.Size(24, 24);
            this.btnShowOpenFileDialog.TabIndex = 1;
            this.btnShowOpenFileDialog.Text = "...";
            this.btnShowOpenFileDialog.UseVisualStyleBackColor = true;
            this.btnShowOpenFileDialog.Click += new System.EventHandler(this.btnShowOpenFileDialog_Click);
            // 
            // txtFileName
            // 
            this.pnlMain.SetColumnSpan(this.txtFileName, 2);
            this.txtFileName.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFileName.Location = new System.Drawing.Point(3, 141);
            this.txtFileName.Name = "txtFileName";
            this.txtFileName.Size = new System.Drawing.Size(378, 20);
            this.txtFileName.TabIndex = 2;
            // 
            // btnLoadToDB
            // 
            this.pnlMain.SetColumnSpan(this.btnLoadToDB, 2);
            this.btnLoadToDB.Dock = System.Windows.Forms.DockStyle.Fill;
            this.btnLoadToDB.Location = new System.Drawing.Point(3, 167);
            this.btnLoadToDB.Name = "btnLoadToDB";
            this.btnLoadToDB.Size = new System.Drawing.Size(378, 23);
            this.btnLoadToDB.TabIndex = 3;
            this.btnLoadToDB.Text = "Load to DB";
            this.btnLoadToDB.UseVisualStyleBackColor = true;
            this.btnLoadToDB.Click += new System.EventHandler(this.btnLoadToDB_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "VLL.txt";
            this.openFileDialog1.Filter = "Text files|*.txt|All files|*.*";
            // 
            // lblMessage
            // 
            this.pnlMain.SetColumnSpan(this.lblMessage, 2);
            this.lblMessage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lblMessage.Location = new System.Drawing.Point(3, 196);
            this.lblMessage.Multiline = true;
            this.lblMessage.Name = "lblMessage";
            this.lblMessage.ReadOnly = true;
            this.lblMessage.Size = new System.Drawing.Size(378, 13);
            this.lblMessage.TabIndex = 4;
            // 
            // pnlMain
            // 
            this.pnlMain.ColumnCount = 2;
            this.pnlMain.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.pnlMain.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.pnlMain.Controls.Add(this.lblUser, 0, 0);
            this.pnlMain.Controls.Add(this.txtUser, 1, 0);
            this.pnlMain.Controls.Add(this.lblPassword, 0, 1);
            this.pnlMain.Controls.Add(this.txtPassword, 1, 1);
            this.pnlMain.Controls.Add(this.lblDomain, 0, 2);
            this.pnlMain.Controls.Add(this.txtDomain, 1, 2);
            this.pnlMain.Controls.Add(this.lblSelect, 0, 4);
            this.pnlMain.Controls.Add(this.lblMessage, 0, 7);
            this.pnlMain.Controls.Add(this.btnShowOpenFileDialog, 1, 4);
            this.pnlMain.Controls.Add(this.btnLoadToDB, 0, 6);
            this.pnlMain.Controls.Add(this.txtFileName, 0, 5);
            this.pnlMain.Controls.Add(this.btnOk, 0, 3);
            this.pnlMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlMain.Location = new System.Drawing.Point(0, 0);
            this.pnlMain.Name = "pnlMain";
            this.pnlMain.RowCount = 8;
            this.pnlMain.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.pnlMain.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.pnlMain.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.pnlMain.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.pnlMain.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.pnlMain.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.pnlMain.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.pnlMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.pnlMain.Size = new System.Drawing.Size(384, 212);
            this.pnlMain.TabIndex = 5;
            // 
            // lblUser
            // 
            this.lblUser.AutoSize = true;
            this.lblUser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lblUser.Location = new System.Drawing.Point(3, 0);
            this.lblUser.Name = "lblUser";
            this.lblUser.Size = new System.Drawing.Size(108, 26);
            this.lblUser.TabIndex = 6;
            this.lblUser.Text = "User name:";
            this.lblUser.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lblUser.Visible = false;
            // 
            // txtUser
            // 
            this.txtUser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtUser.Location = new System.Drawing.Point(117, 3);
            this.txtUser.Name = "txtUser";
            this.txtUser.Size = new System.Drawing.Size(264, 20);
            this.txtUser.TabIndex = 10;
            this.txtUser.Visible = false;
            // 
            // lblPassword
            // 
            this.lblPassword.AutoSize = true;
            this.lblPassword.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lblPassword.Location = new System.Drawing.Point(3, 26);
            this.lblPassword.Name = "lblPassword";
            this.lblPassword.Size = new System.Drawing.Size(108, 26);
            this.lblPassword.TabIndex = 8;
            this.lblPassword.Text = "Password:";
            this.lblPassword.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lblPassword.Visible = false;
            // 
            // txtPassword
            // 
            this.txtPassword.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtPassword.Location = new System.Drawing.Point(117, 29);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.Size = new System.Drawing.Size(264, 20);
            this.txtPassword.TabIndex = 11;
            this.txtPassword.UseSystemPasswordChar = true;
            this.txtPassword.Visible = false;
            // 
            // lblDomain
            // 
            this.lblDomain.AutoSize = true;
            this.lblDomain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lblDomain.Location = new System.Drawing.Point(3, 52);
            this.lblDomain.Name = "lblDomain";
            this.lblDomain.Size = new System.Drawing.Size(108, 26);
            this.lblDomain.TabIndex = 9;
            this.lblDomain.Text = "Domain:";
            this.lblDomain.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lblDomain.Visible = false;
            // 
            // txtDomain
            // 
            this.txtDomain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtDomain.Location = new System.Drawing.Point(117, 55);
            this.txtDomain.Name = "txtDomain";
            this.txtDomain.Size = new System.Drawing.Size(264, 20);
            this.txtDomain.TabIndex = 12;
            this.txtDomain.Visible = false;
            // 
            // btnOk
            // 
            this.pnlMain.SetColumnSpan(this.btnOk, 2);
            this.btnOk.Dock = System.Windows.Forms.DockStyle.Fill;
            this.btnOk.Location = new System.Drawing.Point(3, 81);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(378, 24);
            this.btnOk.TabIndex = 13;
            this.btnOk.Text = "Ok";
            this.btnOk.UseVisualStyleBackColor = true;
            this.btnOk.Visible = false;
            this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
            // 
            // VLLAForm
            // 
            this.AcceptButton = this.btnOk;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(384, 212);
            this.Controls.Add(this.pnlMain);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "VLLAForm";
            this.Text = "VLL Application";
            this.Load += new System.EventHandler(this.VLLAForm_Load);
            this.pnlMain.ResumeLayout(false);
            this.pnlMain.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label lblSelect;
        private System.Windows.Forms.Button btnShowOpenFileDialog;
        private System.Windows.Forms.TextBox txtFileName;
        private System.Windows.Forms.Button btnLoadToDB;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.TextBox lblMessage;
        private System.Windows.Forms.TableLayoutPanel pnlMain;
        private System.Windows.Forms.TextBox txtDomain;
        private System.Windows.Forms.TextBox txtPassword;
        private System.Windows.Forms.Label lblDomain;
        private System.Windows.Forms.Label lblPassword;
        private System.Windows.Forms.Label lblUser;
        private System.Windows.Forms.TextBox txtUser;
        private System.Windows.Forms.Button btnOk;
    }
}

