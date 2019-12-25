using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CardboardLauncher
{
    public partial class GametokenDialog : Form
    {
        public string gameToken = "";

        public GametokenDialog()
        {
            InitializeComponent();
        }

        private void GametokenDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            gameToken = gameTokenBox.Text;
        }

        private void GametokenDialog_Load(object sender, EventArgs e)
        {
            gameTokenBox.Text = gameToken;
        }
    }
}
