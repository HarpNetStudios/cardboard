using System;
using System.Drawing;
using System.Windows.Forms;

namespace CardboardLauncher
{
    public partial class GametokenDialog : Form
    {
        public string gameToken = "";

        private bool drag = false; // determine if we should be moving the form
        private Point startPoint = new Point(0, 0); // also for the moving

        public GametokenDialog()
        {
            InitializeComponent();
        }

        private void GametokenDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(gameTokenBox.Text.Length.Equals(64)) gameToken = gameTokenBox.Text;
        }

        private void GametokenDialog_Load(object sender, EventArgs e)
        {
            gameTokenBox.Text = gameToken;
        }

        private void gameTokenBox_TextChanged(object sender, EventArgs e)
        {
            gameTokenBtn.Enabled = gameTokenBox.Text.Length.Equals(64);
        }

        private void closeBtn_MouseClick(object sender, MouseEventArgs e)
        {
            this.Close();
        }

        private void closeBtn_MouseDown(object sender, MouseEventArgs e)
        {
            closeBtn.BackgroundImage = CardboardLauncher.Properties.Resources.close_click;
        }

        private void closeBtn_MouseUp(object sender, MouseEventArgs e)
        {
            closeBtn.BackgroundImage = CardboardLauncher.Properties.Resources.close;
        }

        private void Title_MouseUp(object sender, MouseEventArgs e)
        {
            this.drag = false;
        }

        private void Title_MouseDown(object sender, MouseEventArgs e)
        {
            this.startPoint = e.Location;
            this.drag = true;
        }

        private void Title_MouseMove(object sender, MouseEventArgs e)
        {
            if(this.drag)
            { // if we should be dragging it, we need to figure out some movement
                Point p1 = new Point(e.X, e.Y);
                Point p2 = this.PointToScreen(p1);
                Point p3 = new Point(p2.X - this.startPoint.X,
                                     p2.Y - this.startPoint.Y);
                this.Location = p3;
            }
        }
    }
}
