using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Security.Permissions;
using System.Text;
using System.Windows.Forms;
using Newtonsoft.Json;

namespace CardboardLauncher
{
    [PermissionSet(SecurityAction.Demand, Name = "FullTrust")]
    [System.Runtime.InteropServices.ComVisibleAttribute(true)]
    public partial class mainForm : Form
    {
        private static string gameName = "Project Crimson";

        private bool use64bit;

        private Config config;

        private void LoadConfig()
        {
            using (StreamReader r = new StreamReader("launcher.json"))
            {
                string json = r.ReadToEnd();
                config = JsonConvert.DeserializeObject<Config>(json);
            }
        }

        private void SaveConfig()
        {
            using (StreamWriter file = File.CreateText("launcher.json"))
            {
                JsonSerializer serializer = new JsonSerializer();
                //serialize object directly into file stream
                serializer.Serialize(file, config);
            }
        }

        private bool GrabInfo(string token)
        {
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(@"https://harpnetstudios.com/hnid/api/v1/game/get/userinfo?id=1&token="+token);
            HttpWebResponse response = (HttpWebResponse)request.GetResponse();
            string content = new StreamReader(response.GetResponseStream()).ReadToEnd();
            UserInfo info = JsonConvert.DeserializeObject<UserInfo>(content);
            Console.WriteLine(content);

            bool success = info.status == 0;

            if (success)
            {
                userAuthLabel.Text = "User: " + info.username;
            } 
            else
            {
                userAuthLabel.Text = "User: N/A";
            }
            playButton.Enabled = success;

            return success;
        }

        private class Config
        {
            public string webUrl = "";
            public string homeDir = "";
            public string gameToken = "";
        }

        private class UserInfo
        {
            public string message;
            public int status;
            public string username;
        }

        public mainForm()
        {
            InitializeComponent();
            webLauncher.Document.BackColor = this.BackColor;
            LoadConfig();
            GrabInfo(config.gameToken);
            if (config.homeDir == "") config.homeDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "My Games", "Project Crimson Alpha");
            homeDirBox.Text = config.homeDir;
            webLauncher.ObjectForScripting = this;
            webLauncher.Url = new Uri(config.webUrl+"?goAway=1234");
            this.Text = gameName + " Launcher";
            playButton.Text = "Play " + gameName;
        }

        public void displayMessage(String message)
        {
            MessageBox.Show(message, "Cardboard Launcher");
        }
        
        public void setGameToken(string token)
        {
            if (GrabInfo(token))
            {
                config.gameToken = token;
                MessageBox.Show("Successfully set game token!", "Cardboard Launcher");
            } else MessageBox.Show("Error setting game token", "Cardboard Launcher", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void archGroup_Layout(object sender, LayoutEventArgs e)
        {
            if (!Environment.Is64BitOperatingSystem)
            {
                archRadio32.Checked = true;
                archGroup.Enabled = archRadio32.Enabled = archRadio64.Enabled = false;
            }
            else
            {
                archRadio64.Checked = true;
            }
        }

        private void homeDirBtn_Click(object sender, EventArgs e)
        {
            homeDirBrowser.SelectedPath = homeDirBox.Text;
            if (homeDirBrowser.ShowDialog() == DialogResult.OK)
            {
                homeDirBox.Text = homeDirBrowser.SelectedPath;
            }
        }

        private void saveConfigBtn_Click(object sender, EventArgs e)
        {
            SaveConfig();
        }

        private void archRadio64_CheckedChanged(object sender, EventArgs e)
        {
            use64bit = archRadio64.Checked;
        }

        private void playButton_Click(object sender, EventArgs e)
        {
            ProcessStartInfo gameProcess = new ProcessStartInfo();
            gameProcess.FileName = Path.Combine("bin" + (use64bit ? "64" : ""), "cardboard_msvc.exe");
            gameProcess.Arguments = "-q\"" + config.homeDir + "\" -glog.txt -c"+config.gameToken;
            try
            {
                Process.Start(gameProcess);
                Environment.Exit(0);
            } 
            catch(Exception ex)
            {
                MessageBox.Show("Error while starting game process: "+ex.Message,"Launcher Error",MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            

            Console.WriteLine(gameProcess.WorkingDirectory);
        }

        private void homeDirBox_TextChanged(object sender, EventArgs e)
        {
            config.homeDir = homeDirBox.Text;
        }

        private void versionLabel_DoubleClick(object sender, EventArgs e)
        {
            MessageBox.Show("Created by Yellowberry.\nSpecial thanks to the HN crew!", "Cardboard Launcher");
        }

        private void mainForm_Load(object sender, EventArgs e)
        {
            webLauncher.ObjectForScripting = this;
        }

        private void webLauncher_Navigating(object sender, WebBrowserNavigatingEventArgs e)
        {
            if(webLauncher.Url.ToString() == "about:blank") { return; }
            string safeSite = "https://harpnetstudios.com/";
            Console.WriteLine(webLauncher.Url.ToString());
            if (!webLauncher.Url.ToString().StartsWith(safeSite))
            {
                webWarn.BackColor = Color.Red;
            }
            else webWarn.BackColor = this.BackColor;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            GametokenDialog gtDialog = new GametokenDialog();
            gtDialog.gameToken = config.gameToken;
            if (gtDialog.ShowDialog() == DialogResult.OK)
            {
                setGameToken(gtDialog.gameToken);
            }
        }
    }
}
