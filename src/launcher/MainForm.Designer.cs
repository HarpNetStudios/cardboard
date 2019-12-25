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
            this.saveConfigBtn = new System.Windows.Forms.Button();
            this.webWarn = new System.Windows.Forms.Panel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.userAuthLabel = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.archGroup.SuspendLayout();
            this.homeDirGroup.SuspendLayout();
            this.webWarn.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // archGroup
            // 
            this.archGroup.Controls.Add(this.archRadio64);
            this.archGroup.Controls.Add(this.archRadio32);
            this.archGroup.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.archGroup.Location = new System.Drawing.Point(12, 12);
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
            this.archRadio64.Size = new System.Drawing.Size(78, 17);
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
            this.archRadio32.Size = new System.Drawing.Size(78, 17);
            this.archRadio32.TabIndex = 0;
            this.archRadio32.TabStop = true;
            this.archRadio32.Text = "32-Bit (x86)";
            this.archRadio32.UseVisualStyleBackColor = true;
            // 
            // playButton
            // 
            this.playButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.playButton.FlatAppearance.BorderSize = 0;
            this.playButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.playButton.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.playButton.Location = new System.Drawing.Point(12, 320);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(184, 100);
            this.playButton.TabIndex = 1;
            this.playButton.Text = "Play {GAMETITLE}";
            this.playButton.UseVisualStyleBackColor = false;
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            this.playButton.Enabled = false;
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
            this.homeDirGroup.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.homeDirGroup.Location = new System.Drawing.Point(12, 60);
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
            this.homeDirBox.Location = new System.Drawing.Point(6, 19);
            this.homeDirBox.Name = "homeDirBox";
            this.homeDirBox.Size = new System.Drawing.Size(146, 20);
            this.homeDirBox.TabIndex = 0;
            this.homeDirBox.TextChanged += new System.EventHandler(this.homeDirBox_TextChanged);
            // 
            // versionLabel
            // 
            this.versionLabel.AutoSize = true;
            this.versionLabel.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(36)))), ((int)(((byte)(0)))));
            this.versionLabel.Location = new System.Drawing.Point(12, 300);
            this.versionLabel.Name = "versionLabel";
            this.versionLabel.Size = new System.Drawing.Size(108, 13);
            this.versionLabel.TabIndex = 3;
            this.versionLabel.Text = "Launcher Version 1.0";
            this.versionLabel.DoubleClick += new System.EventHandler(this.versionLabel_DoubleClick);
            // 
            // saveConfigBtn
            // 
            this.saveConfigBtn.AutoEllipsis = true;
            this.saveConfigBtn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.saveConfigBtn.FlatAppearance.BorderSize = 0;
            this.saveConfigBtn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.saveConfigBtn.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.saveConfigBtn.Location = new System.Drawing.Point(12, 179);
            this.saveConfigBtn.Name = "saveConfigBtn";
            this.saveConfigBtn.Size = new System.Drawing.Size(184, 24);
            this.saveConfigBtn.TabIndex = 2;
            this.saveConfigBtn.Text = "Save Config";
            this.saveConfigBtn.UseVisualStyleBackColor = false;
            this.saveConfigBtn.Click += new System.EventHandler(this.saveConfigBtn_Click);
            // 
            // webWarn
            // 
            this.webWarn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(48)))));
            this.webWarn.Controls.Add(this.webLauncher);
            this.webWarn.Location = new System.Drawing.Point(200, 10);
            this.webWarn.Name = "webWarn";
            this.webWarn.Size = new System.Drawing.Size(596, 416);
            this.webWarn.TabIndex = 4;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.userAuthLabel);
            this.groupBox1.Controls.Add(this.button1);
            this.groupBox1.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.groupBox1.Location = new System.Drawing.Point(12, 111);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(184, 62);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Authentication";
            // 
            // userAuthLabel
            // 
            this.userAuthLabel.AutoSize = true;
            this.userAuthLabel.Location = new System.Drawing.Point(6, 16);
            this.userAuthLabel.Name = "userAuthLabel";
            this.userAuthLabel.Size = new System.Drawing.Size(55, 13);
            this.userAuthLabel.TabIndex = 2;
            this.userAuthLabel.Text = "User: N/A";
            // 
            // button1
            // 
            this.button1.AutoEllipsis = true;
            this.button1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.button1.FlatAppearance.BorderSize = 0;
            this.button1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button1.Location = new System.Drawing.Point(6, 32);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(172, 24);
            this.button1.TabIndex = 1;
            this.button1.Text = "Change Game Token";
            this.button1.UseVisualStyleBackColor = false;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(48)))));
            this.ClientSize = new System.Drawing.Size(809, 431);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.webWarn);
            this.Controls.Add(this.saveConfigBtn);
            this.Controls.Add(this.versionLabel);
            this.Controls.Add(this.homeDirGroup);
            this.Controls.Add(this.playButton);
            this.Controls.Add(this.archGroup);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "mainForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "{GAMETITLE} Launcher";
            this.Load += new System.EventHandler(this.mainForm_Load);
            this.archGroup.ResumeLayout(false);
            this.archGroup.PerformLayout();
            this.homeDirGroup.ResumeLayout(false);
            this.homeDirGroup.PerformLayout();
            this.webWarn.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
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
        private System.Windows.Forms.Button saveConfigBtn;
        private System.Windows.Forms.Panel webWarn;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label userAuthLabel;
        private System.Windows.Forms.Button button1;
    }
}

