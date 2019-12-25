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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(GametokenDialog));
            this.gameTokenBtn = new System.Windows.Forms.Button();
            this.gameTokenBox = new System.Windows.Forms.TextBox();
            this.gameTokenLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // gameTokenBtn
            // 
            this.gameTokenBtn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.gameTokenBtn.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.gameTokenBtn.FlatAppearance.BorderSize = 0;
            this.gameTokenBtn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.gameTokenBtn.Location = new System.Drawing.Point(200, 55);
            this.gameTokenBtn.Name = "gameTokenBtn";
            this.gameTokenBtn.Size = new System.Drawing.Size(80, 24);
            this.gameTokenBtn.TabIndex = 0;
            this.gameTokenBtn.Text = "Save Token";
            this.gameTokenBtn.UseVisualStyleBackColor = false;
            // 
            // gameTokenBox
            // 
            this.gameTokenBox.Location = new System.Drawing.Point(12, 29);
            this.gameTokenBox.Name = "gameTokenBox";
            this.gameTokenBox.Size = new System.Drawing.Size(460, 20);
            this.gameTokenBox.TabIndex = 1;
            this.gameTokenBox.UseSystemPasswordChar = true;
            // 
            // gameTokenLabel
            // 
            this.gameTokenLabel.AutoSize = true;
            this.gameTokenLabel.Location = new System.Drawing.Point(13, 13);
            this.gameTokenLabel.Name = "gameTokenLabel";
            this.gameTokenLabel.Size = new System.Drawing.Size(151, 13);
            this.gameTokenLabel.TabIndex = 2;
            this.gameTokenLabel.Text = "Please enter your game token:";
            // 
            // GametokenDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(48)))), ((int)(((byte)(48)))));
            this.ClientSize = new System.Drawing.Size(484, 91);
            this.Controls.Add(this.gameTokenLabel);
            this.Controls.Add(this.gameTokenBox);
            this.Controls.Add(this.gameTokenBtn);
            this.ForeColor = System.Drawing.SystemColors.HighlightText;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "GametokenDialog";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "Cardboard Launcher";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.GametokenDialog_FormClosing);
            this.Load += new System.EventHandler(this.GametokenDialog_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button gameTokenBtn;
        private System.Windows.Forms.TextBox gameTokenBox;
        private System.Windows.Forms.Label gameTokenLabel;
    }
}