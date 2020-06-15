namespace CardboardLauncher
{
    partial class GametokenDialog
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
            if(disposing && (components != null))
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(GametokenDialog));
            this.gameTokenBtn = new System.Windows.Forms.Button();
            this.gameTokenBox = new System.Windows.Forms.TextBox();
            this.gameTokenLabel = new System.Windows.Forms.Label();
            this.launcherTitle = new System.Windows.Forms.Label();
            this.closeBtn = new System.Windows.Forms.Panel();
            this.title = new System.Windows.Forms.Panel();
            this.title.SuspendLayout();
            this.SuspendLayout();
            // 
            // gameTokenBtn
            // 
            this.gameTokenBtn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.gameTokenBtn.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.gameTokenBtn.Enabled = false;
            this.gameTokenBtn.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.gameTokenBtn.FlatAppearance.BorderSize = 0;
            this.gameTokenBtn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.gameTokenBtn.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gameTokenBtn.Location = new System.Drawing.Point(200, 81);
            this.gameTokenBtn.Name = "gameTokenBtn";
            this.gameTokenBtn.Size = new System.Drawing.Size(80, 24);
            this.gameTokenBtn.TabIndex = 0;
            this.gameTokenBtn.Text = "Save Token";
            this.gameTokenBtn.UseVisualStyleBackColor = false;
            // 
            // gameTokenBox
            // 
            this.gameTokenBox.Location = new System.Drawing.Point(12, 55);
            this.gameTokenBox.Name = "gameTokenBox";
            this.gameTokenBox.Size = new System.Drawing.Size(460, 20);
            this.gameTokenBox.TabIndex = 1;
            this.gameTokenBox.UseSystemPasswordChar = true;
            this.gameTokenBox.TextChanged += new System.EventHandler(this.gameTokenBox_TextChanged);
            // 
            // gameTokenLabel
            // 
            this.gameTokenLabel.AutoSize = true;
            this.gameTokenLabel.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gameTokenLabel.Location = new System.Drawing.Point(13, 39);
            this.gameTokenLabel.Name = "gameTokenLabel";
            this.gameTokenLabel.Size = new System.Drawing.Size(162, 13);
            this.gameTokenLabel.TabIndex = 2;
            this.gameTokenLabel.Text = "Please enter your game token:";
            // 
            // launcherTitle
            // 
            this.launcherTitle.BackColor = System.Drawing.Color.Transparent;
            this.launcherTitle.Font = new System.Drawing.Font("Segoe UI", 10.25F);
            this.launcherTitle.ForeColor = System.Drawing.Color.White;
            this.launcherTitle.Location = new System.Drawing.Point(0, 0);
            this.launcherTitle.Name = "launcherTitle";
            this.launcherTitle.Padding = new System.Windows.Forms.Padding(6, 0, 0, 0);
            this.launcherTitle.Size = new System.Drawing.Size(164, 26);
            this.launcherTitle.TabIndex = 1;
            this.launcherTitle.Text = "Cardboard Launcher";
            this.launcherTitle.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.launcherTitle.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Title_MouseDown);
            this.launcherTitle.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Title_MouseMove);
            this.launcherTitle.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Title_MouseUp);
            // 
            // closeBtn
            // 
            this.closeBtn.BackgroundImage = global::CardboardLauncher.Properties.Resources.close;
            this.closeBtn.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.closeBtn.Location = new System.Drawing.Point(432, 0);
            this.closeBtn.Name = "closeBtn";
            this.closeBtn.Size = new System.Drawing.Size(52, 26);
            this.closeBtn.TabIndex = 0;
            this.closeBtn.MouseClick += new System.Windows.Forms.MouseEventHandler(this.closeBtn_MouseClick);
            this.closeBtn.MouseDown += new System.Windows.Forms.MouseEventHandler(this.closeBtn_MouseDown);
            this.closeBtn.MouseUp += new System.Windows.Forms.MouseEventHandler(this.closeBtn_MouseUp);
            // 
            // title
            // 
            this.title.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.title.Controls.Add(this.launcherTitle);
            this.title.Controls.Add(this.closeBtn);
            this.title.Location = new System.Drawing.Point(0, 0);
            this.title.Name = "title";
            this.title.Size = new System.Drawing.Size(484, 26);
            this.title.TabIndex = 9;
            this.title.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Title_MouseDown);
            this.title.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Title_MouseMove);
            this.title.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Title_MouseUp);
            // 
            // GametokenDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(48)))));
            this.ClientSize = new System.Drawing.Size(484, 117);
            this.Controls.Add(this.title);
            this.Controls.Add(this.gameTokenLabel);
            this.Controls.Add(this.gameTokenBox);
            this.Controls.Add(this.gameTokenBtn);
            this.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "GametokenDialog";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "Cardboard Launcher";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.GametokenDialog_FormClosing);
            this.Load += new System.EventHandler(this.GametokenDialog_Load);
            this.title.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button gameTokenBtn;
        private System.Windows.Forms.TextBox gameTokenBox;
        private System.Windows.Forms.Label gameTokenLabel;
        private System.Windows.Forms.Label launcherTitle;
        private System.Windows.Forms.Panel closeBtn;
        private System.Windows.Forms.Panel title;
    }
}