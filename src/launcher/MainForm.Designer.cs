namespace CardboardLauncher
{
    partial class mainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(mainForm));
            this.archGroup = new System.Windows.Forms.GroupBox();
            this.archRadio64 = new System.Windows.Forms.RadioButton();
            this.archRadio32 = new System.Windows.Forms.RadioButton();
            this.playButton = new System.Windows.Forms.Button();
            this.webLauncher = new System.Windows.Forms.WebBrowser();
            this.homeDirBrowser = new System.Windows.Forms.FolderBrowserDialog();
            this.homeDirGroup = new System.Windows.Forms.GroupBox();
            this.homeDirBtn = new System.Windows.Forms.Button();
            this.homeDirBox = new System.Windows.Forms.TextBox();
            this.versionLabel = new System.Windows.Forms.Label();
            this.webWarn = new System.Windows.Forms.Panel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.userAuthLabel = new System.Windows.Forms.Label();
            this.gameTokenBtn = new System.Windows.Forms.Button();
            this.saveConfigChkBox = new System.Windows.Forms.CheckBox();
            this.qConnectChkBox = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.qConnectServBox = new System.Windows.Forms.TextBox();
            this.pageSelectCombo = new System.Windows.Forms.ComboBox();
            this.title = new System.Windows.Forms.Panel();
            this.launcherTitle = new System.Windows.Forms.Label();
            this.closeBtn = new System.Windows.Forms.Panel();
            this.advSettings = new System.Windows.Forms.Panel();
            this.playOfflineChkBox = new System.Windows.Forms.CheckBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.archGroup.SuspendLayout();
            this.homeDirGroup.SuspendLayout();
            this.webWarn.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.title.SuspendLayout();
            this.advSettings.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // archGroup
            // 
            this.archGroup.Controls.Add(this.archRadio64);
            this.archGroup.Controls.Add(this.archRadio32);
            this.archGroup.Font = new System.Drawing.Font("Segoe UI", 8.25F);
            this.archGroup.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.archGroup.Location = new System.Drawing.Point(9, 3);
            this.archGroup.Name = "archGroup";
            this.archGroup.Size = new System.Drawing.Size(184, 42);
            this.archGroup.TabIndex = 0;
            this.archGroup.TabStop = false;
            this.archGroup.Text = "Architecture";
            this.archGroup.Layout += new System.Windows.Forms.LayoutEventHandler(this.archGroup_Layout);
            // 
            // archRadio64
            // 
            this.archRadio64.AutoSize = true;
            this.archRadio64.Location = new System.Drawing.Point(90, 19);
            this.archRadio64.Name = "archRadio64";
            this.archRadio64.Size = new System.Drawing.Size(80, 17);
            this.archRadio64.TabIndex = 1;
            this.archRadio64.TabStop = true;
            this.archRadio64.Text = "64-Bit (x64)";
            this.archRadio64.UseVisualStyleBackColor = true;
            this.archRadio64.CheckedChanged += new System.EventHandler(this.archRadio64_CheckedChanged);
            // 
            // archRadio32
            // 
            this.archRadio32.AutoSize = true;
            this.archRadio32.Location = new System.Drawing.Point(6, 19);
            this.archRadio32.Name = "archRadio32";
            this.archRadio32.Size = new System.Drawing.Size(80, 17);
            this.archRadio32.TabIndex = 0;
            this.archRadio32.TabStop = true;
            this.archRadio32.Text = "32-Bit (x86)";
            this.archRadio32.UseVisualStyleBackColor = true;
            // 
            // playButton
            // 
            this.playButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.playButton.Enabled = false;
            this.playButton.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.playButton.FlatAppearance.BorderSize = 0;
            this.playButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.playButton.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.playButton.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.playButton.Location = new System.Drawing.Point(-1, 26);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(184, 50);
            this.playButton.TabIndex = 1;
            this.playButton.Text = "Play {GAMETITLE}";
            this.playButton.UseVisualStyleBackColor = false;
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            // 
            // webLauncher
            // 
            this.webLauncher.AllowWebBrowserDrop = false;
            this.webLauncher.Location = new System.Drawing.Point(3, 3);
            this.webLauncher.MinimumSize = new System.Drawing.Size(20, 20);
            this.webLauncher.Name = "webLauncher";
            this.webLauncher.ScrollBarsEnabled = false;
            this.webLauncher.Size = new System.Drawing.Size(590, 410);
            this.webLauncher.TabIndex = 2;
            this.webLauncher.Url = new System.Uri("", System.UriKind.Relative);
            this.webLauncher.WebBrowserShortcutsEnabled = false;
            this.webLauncher.Navigating += new System.Windows.Forms.WebBrowserNavigatingEventHandler(this.webLauncher_Navigating);
            // 
            // homeDirBrowser
            // 
            this.homeDirBrowser.Description = "Select a directory for the game to save user files.";
            // 
            // homeDirGroup
            // 
            this.homeDirGroup.Controls.Add(this.homeDirBtn);
            this.homeDirGroup.Controls.Add(this.homeDirBox);
            this.homeDirGroup.Font = new System.Drawing.Font("Segoe UI", 8.25F);
            this.homeDirGroup.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.homeDirGroup.Location = new System.Drawing.Point(9, 51);
            this.homeDirGroup.Name = "homeDirGroup";
            this.homeDirGroup.Size = new System.Drawing.Size(184, 45);
            this.homeDirGroup.TabIndex = 2;
            this.homeDirGroup.TabStop = false;
            this.homeDirGroup.Text = "Home Directory";
            // 
            // homeDirBtn
            // 
            this.homeDirBtn.AutoEllipsis = true;
            this.homeDirBtn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.homeDirBtn.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.homeDirBtn.FlatAppearance.BorderSize = 0;
            this.homeDirBtn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.homeDirBtn.Location = new System.Drawing.Point(158, 19);
            this.homeDirBtn.Name = "homeDirBtn";
            this.homeDirBtn.Size = new System.Drawing.Size(20, 20);
            this.homeDirBtn.TabIndex = 1;
            this.homeDirBtn.Text = "...";
            this.homeDirBtn.UseVisualStyleBackColor = false;
            this.homeDirBtn.Click += new System.EventHandler(this.homeDirBtn_Click);
            // 
            // homeDirBox
            // 
            this.homeDirBox.AllowDrop = true;
            this.homeDirBox.Location = new System.Drawing.Point(6, 18);
            this.homeDirBox.Name = "homeDirBox";
            this.homeDirBox.Size = new System.Drawing.Size(146, 22);
            this.homeDirBox.TabIndex = 0;
            this.homeDirBox.TextChanged += new System.EventHandler(this.homeDirBox_TextChanged);
            // 
            // versionLabel
            // 
            this.versionLabel.AutoSize = true;
            this.versionLabel.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.versionLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(36)))), ((int)(((byte)(0)))));
            this.versionLabel.Location = new System.Drawing.Point(9, 345);
            this.versionLabel.Name = "versionLabel";
            this.versionLabel.Size = new System.Drawing.Size(122, 13);
            this.versionLabel.TabIndex = 3;
            this.versionLabel.Text = "Launcher Version 1.2.1";
            this.versionLabel.DoubleClick += new System.EventHandler(this.versionLabel_DoubleClick);
            // 
            // webWarn
            // 
            this.webWarn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(48)))));
            this.webWarn.Controls.Add(this.webLauncher);
            this.webWarn.Location = new System.Drawing.Point(793, 124);
            this.webWarn.Name = "webWarn";
            this.webWarn.Size = new System.Drawing.Size(596, 416);
            this.webWarn.TabIndex = 4;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.userAuthLabel);
            this.groupBox1.Controls.Add(this.gameTokenBtn);
            this.groupBox1.Font = new System.Drawing.Font("Segoe UI", 8.25F);
            this.groupBox1.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.groupBox1.Location = new System.Drawing.Point(12, 64);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(184, 62);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Authentication";
            // 
            // userAuthLabel
            // 
            this.userAuthLabel.AutoSize = true;
            this.userAuthLabel.Font = new System.Drawing.Font("Segoe UI", 8.25F);
            this.userAuthLabel.Location = new System.Drawing.Point(6, 16);
            this.userAuthLabel.Name = "userAuthLabel";
            this.userAuthLabel.Size = new System.Drawing.Size(55, 13);
            this.userAuthLabel.TabIndex = 2;
            this.userAuthLabel.Text = "User: N/A";
            // 
            // gameTokenBtn
            // 
            this.gameTokenBtn.AutoEllipsis = true;
            this.gameTokenBtn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.gameTokenBtn.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.gameTokenBtn.FlatAppearance.BorderSize = 0;
            this.gameTokenBtn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.gameTokenBtn.Font = new System.Drawing.Font("Segoe UI", 8.25F);
            this.gameTokenBtn.Location = new System.Drawing.Point(6, 32);
            this.gameTokenBtn.Name = "gameTokenBtn";
            this.gameTokenBtn.Size = new System.Drawing.Size(172, 24);
            this.gameTokenBtn.TabIndex = 1;
            this.gameTokenBtn.Text = "Change Game Token";
            this.gameTokenBtn.UseVisualStyleBackColor = false;
            this.gameTokenBtn.Click += new System.EventHandler(this.gameTokenBtn_Click);
            // 
            // saveConfigChkBox
            // 
            this.saveConfigChkBox.AutoSize = true;
            this.saveConfigChkBox.Checked = true;
            this.saveConfigChkBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.saveConfigChkBox.Font = new System.Drawing.Font("Segoe UI", 8.25F);
            this.saveConfigChkBox.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(36)))), ((int)(((byte)(0)))));
            this.saveConfigChkBox.Location = new System.Drawing.Point(12, 183);
            this.saveConfigChkBox.Name = "saveConfigChkBox";
            this.saveConfigChkBox.Size = new System.Drawing.Size(87, 17);
            this.saveConfigChkBox.TabIndex = 5;
            this.saveConfigChkBox.Text = "Save Config";
            this.saveConfigChkBox.UseVisualStyleBackColor = true;
            this.saveConfigChkBox.CheckedChanged += new System.EventHandler(this.saveConfigChkBox_CheckedChanged);
            // 
            // qConnectChkBox
            // 
            this.qConnectChkBox.AutoSize = true;
            this.qConnectChkBox.Font = new System.Drawing.Font("Segoe UI", 8.25F);
            this.qConnectChkBox.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.qConnectChkBox.Location = new System.Drawing.Point(97, 183);
            this.qConnectChkBox.Name = "qConnectChkBox";
            this.qConnectChkBox.Size = new System.Drawing.Size(101, 17);
            this.qConnectChkBox.TabIndex = 6;
            this.qConnectChkBox.Text = "Quick Connect";
            this.qConnectChkBox.UseVisualStyleBackColor = true;
            this.qConnectChkBox.CheckedChanged += new System.EventHandler(this.qConnectChkBox_CheckedChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.qConnectServBox);
            this.groupBox2.Font = new System.Drawing.Font("Segoe UI", 8.25F);
            this.groupBox2.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.groupBox2.Location = new System.Drawing.Point(12, 132);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(184, 45);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Quick Connect Server";
            // 
            // qConnectServBox
            // 
            this.qConnectServBox.AllowDrop = true;
            this.qConnectServBox.Enabled = false;
            this.qConnectServBox.Location = new System.Drawing.Point(6, 18);
            this.qConnectServBox.Name = "qConnectServBox";
            this.qConnectServBox.Size = new System.Drawing.Size(172, 22);
            this.qConnectServBox.TabIndex = 0;
            this.qConnectServBox.TextChanged += new System.EventHandler(this.qConnectServBox_TextChanged);
            // 
            // pageSelectCombo
            // 
            this.pageSelectCombo.BackColor = System.Drawing.SystemColors.Window;
            this.pageSelectCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.pageSelectCombo.Font = new System.Drawing.Font("Segoe UI", 8.25F);
            this.pageSelectCombo.FormattingEnabled = true;
            this.pageSelectCombo.Items.AddRange(new object[] {
            "HNID",
            "Advanced Settings"});
            this.pageSelectCombo.Location = new System.Drawing.Point(12, 37);
            this.pageSelectCombo.Name = "pageSelectCombo";
            this.pageSelectCombo.Size = new System.Drawing.Size(184, 21);
            this.pageSelectCombo.TabIndex = 7;
            this.pageSelectCombo.SelectedIndexChanged += new System.EventHandler(this.pageSelectCombo_SelectedIndexChanged);
            // 
            // title
            // 
            this.title.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.title.Controls.Add(this.launcherTitle);
            this.title.Controls.Add(this.closeBtn);
            this.title.Location = new System.Drawing.Point(0, 0);
            this.title.Name = "title";
            this.title.Size = new System.Drawing.Size(809, 26);
            this.title.TabIndex = 8;
            this.title.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Title_MouseDown);
            this.title.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Title_MouseMove);
            this.title.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Title_MouseUp);
            // 
            // launcherTitle
            // 
            this.launcherTitle.BackColor = System.Drawing.Color.Transparent;
            this.launcherTitle.Font = new System.Drawing.Font("Segoe UI", 10.25F);
            this.launcherTitle.ForeColor = System.Drawing.Color.White;
            this.launcherTitle.Location = new System.Drawing.Point(0, -2);
            this.launcherTitle.Name = "launcherTitle";
            this.launcherTitle.Padding = new System.Windows.Forms.Padding(6, 0, 0, 0);
            this.launcherTitle.Size = new System.Drawing.Size(748, 26);
            this.launcherTitle.TabIndex = 1;
            this.launcherTitle.Text = "{GAMETITLE} Launcher";
            this.launcherTitle.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.launcherTitle.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Title_MouseDown);
            this.launcherTitle.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Title_MouseMove);
            this.launcherTitle.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Title_MouseUp);
            // 
            // closeBtn
            // 
            this.closeBtn.BackgroundImage = global::CardboardLauncher.Properties.Resources.close;
            this.closeBtn.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.closeBtn.Location = new System.Drawing.Point(757, 0);
            this.closeBtn.Name = "closeBtn";
            this.closeBtn.Size = new System.Drawing.Size(52, 26);
            this.closeBtn.TabIndex = 0;
            this.closeBtn.MouseClick += new System.Windows.Forms.MouseEventHandler(this.closeBtn_MouseClick);
            this.closeBtn.MouseDown += new System.Windows.Forms.MouseEventHandler(this.closeBtn_MouseDown);
            this.closeBtn.MouseUp += new System.Windows.Forms.MouseEventHandler(this.closeBtn_MouseUp);
            // 
            // advSettings
            // 
            this.advSettings.Controls.Add(this.archGroup);
            this.advSettings.Controls.Add(this.homeDirGroup);
            this.advSettings.Location = new System.Drawing.Point(206, 34);
            this.advSettings.Name = "advSettings";
            this.advSettings.Size = new System.Drawing.Size(596, 416);
            this.advSettings.TabIndex = 9;
            // 
            // playOfflineChkBox
            // 
            this.playOfflineChkBox.AutoSize = true;
            this.playOfflineChkBox.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Bold);
            this.playOfflineChkBox.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(36)))), ((int)(((byte)(0)))));
            this.playOfflineChkBox.Location = new System.Drawing.Point(5, 5);
            this.playOfflineChkBox.Name = "playOfflineChkBox";
            this.playOfflineChkBox.Size = new System.Drawing.Size(95, 17);
            this.playOfflineChkBox.TabIndex = 10;
            this.playOfflineChkBox.Text = "Play OFFLINE";
            this.playOfflineChkBox.UseVisualStyleBackColor = true;
            this.playOfflineChkBox.CheckedChanged += new System.EventHandler(this.playOfflineChkBox_CheckedChanged);
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.playButton);
            this.panel1.Controls.Add(this.playOfflineChkBox);
            this.panel1.Location = new System.Drawing.Point(12, 372);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(184, 78);
            this.panel1.TabIndex = 11;
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(48)))));
            this.ClientSize = new System.Drawing.Size(809, 457);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.advSettings);
            this.Controls.Add(this.title);
            this.Controls.Add(this.pageSelectCombo);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.qConnectChkBox);
            this.Controls.Add(this.saveConfigChkBox);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.webWarn);
            this.Controls.Add(this.versionLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "mainForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Tag = "";
            this.Text = "{GAMETITLE} Launcher";
            this.Load += new System.EventHandler(this.mainForm_Load);
            this.archGroup.ResumeLayout(false);
            this.archGroup.PerformLayout();
            this.homeDirGroup.ResumeLayout(false);
            this.homeDirGroup.PerformLayout();
            this.webWarn.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.title.ResumeLayout(false);
            this.advSettings.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox archGroup;
        private System.Windows.Forms.RadioButton archRadio32;
        private System.Windows.Forms.RadioButton archRadio64;
        private System.Windows.Forms.Button playButton;
        private System.Windows.Forms.WebBrowser webLauncher;
        private System.Windows.Forms.FolderBrowserDialog homeDirBrowser;
        private System.Windows.Forms.GroupBox homeDirGroup;
        private System.Windows.Forms.TextBox homeDirBox;
        private System.Windows.Forms.Button homeDirBtn;
        private System.Windows.Forms.Label versionLabel;
        private System.Windows.Forms.Panel webWarn;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label userAuthLabel;
        private System.Windows.Forms.Button gameTokenBtn;
        private System.Windows.Forms.CheckBox saveConfigChkBox;
        private System.Windows.Forms.CheckBox qConnectChkBox;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox qConnectServBox;
        private System.Windows.Forms.ComboBox pageSelectCombo;
        private System.Windows.Forms.Panel title;
        private System.Windows.Forms.Panel closeBtn;
        private System.Windows.Forms.Label launcherTitle;
        private System.Windows.Forms.Panel advSettings;
        private System.Windows.Forms.CheckBox playOfflineChkBox;
        private System.Windows.Forms.Panel panel1;
    }
}

