using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Net;
using System.Security.Permissions;
using System.Windows.Forms;
using Newtonsoft.Json;

namespace CardboardLauncher
{
    [PermissionSet(SecurityAction.Demand, Name = "FullTrust")]
    [System.Runtime.InteropServices.ComVisibleAttribute(true)]
    public partial class mainForm : Form
    {

        public const int launcherVersion = 3;

        private int gameId = 1; // Project Crimson

        private bool drag = false; // determine if we should be moving the form
        private Point startPoint = new Point(0, 0); // also for the moving

        private bool success;
        private int pageSelected;

        private static string gameName = "Project Crimson";

        private bool use64bit;

        private Config config;

        private void DisplayMessage(string text, string origin = null, MessageBoxButtons buttons = MessageBoxButtons.OK, MessageBoxIcon icon = MessageBoxIcon.None)
        {
            MessageBox.Show(text, "Cardboard Launcher" + (origin != null ? " - "+origin : ""), buttons, icon);
        }

        private void LoadConfig()
        {
            try
            {
                using (StreamReader r = new StreamReader("launcher.json"))
                {
                    string json = r.ReadToEnd();
                    config = JsonConvert.DeserializeObject<Config>(json);
                }
            }
            catch (FileNotFoundException)
            {
                using (StreamWriter file = File.CreateText("launcher.json"))
                {
                    JsonSerializer serializer = new JsonSerializer();
                    //serialize object directly into file stream

                    Config tmpConf = new Config();
                    tmpConf.webUrl = "https://harpnetstudios.com/hnid/launcher/";
                    tmpConf.qConnectServ = "hnss.gq";

                    serializer.Serialize(file, tmpConf);
                    config = tmpConf;
                }
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
            try
            {
                HttpWebRequest request = (HttpWebRequest)WebRequest.Create(@"https://harpnetstudios.com/hnid/api/v1/game/get/userinfo?id=" + gameId + "&token=" + token);
                HttpWebResponse response = (HttpWebResponse)request.GetResponse();
                string content = new StreamReader(response.GetResponseStream()).ReadToEnd();
                UserInfo info = JsonConvert.DeserializeObject<UserInfo>(content);
                Console.WriteLine(content);

                success = info.status == 0;

                if (success)
                {
                    config.gameToken = token;
                    userAuthLabel.Text = "User: " + info.username;
                }
                else
                {
                    config.gameToken = "";
                    userAuthLabel.Text = "User: N/A";
                }
                playOfflineChkBox.Checked = playOfflineChkBox.Enabled = !success;
                playButton.Enabled = success || playOfflineChkBox.Checked;
            }
            catch (Exception e)
            {
                DisplayMessage("Exception! "+e.Message,"Account Server - Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            

            return success;
        }

        public class Config
        {
            public string webUrl = "";
            public string homeDir = "";
            public string gameToken = "";
            public string qConnectServ = "";
        }

        public class UserInfo
        {
            public string message { get; set; }
            public int status { get; set; }
            public string username { get; set; }
        }

        public mainForm()
        {
            InitializeComponent();

            webWarn.Location = new Point(206, 34);

            advSettings.Visible = false;

            webLauncher.Document.BackColor = this.BackColor;
            #if DEBUG
                webLauncher.ScriptErrorsSuppressed = false;
            #else
                webLauncher.ScriptErrorsSuppressed = true;
            #endif

            LoadConfig();
            GrabInfo(config.gameToken);
            if (config.homeDir == "") config.homeDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "My Games", "Project Crimson Alpha");
            homeDirBox.Text = config.homeDir;
            qConnectServBox.Text = config.qConnectServ;

            pageSelectCombo.SelectedIndex = 0; // HNID

            webLauncher.ObjectForScripting = this;
            webLauncher.Url = new Uri(config.webUrl);
            this.Text = gameName + " Launcher";
            launcherTitle.Text = gameName + " Launcher";
            playButton.Text = "Play " + gameName;
        }

        public void displayMessage(String message)
        {
            DisplayMessage(message, "Webpage");
        }

        public void setScroll(bool scroll)
        {
            this.webLauncher.ScrollBarsEnabled = scroll;
        }
        
        public void setGameToken(string token, bool quiet=true)
        {
            if (GrabInfo(token))
            {
                if (token.Length.Equals(64))
                {
                    if(!quiet) DisplayMessage("Successfully set game token!");
                    return;
                }
            } else
            {
                playOfflineChkBox.Enabled = true;
            }
            DisplayMessage("Error setting game token", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        public bool isTokenSet()
        {
            return config.gameToken != "";
        }

        public bool checkToken(string token)
        {
            return config.gameToken == token;
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

        private void archRadio64_CheckedChanged(object sender, EventArgs e)
        {
            use64bit = archRadio64.Checked;
        }

        private void playButton_Click(object sender, EventArgs e)
        {
            // Save config if box is checked.
            if(saveConfigChkBox.Checked) SaveConfig();

            string launchToken = config.gameToken;

            if(playOfflineChkBox.Checked)
            {
                launchToken = "OFFLINE";
                DialogResult offlineMessage = MessageBox.Show("Hey, you are about to start the game in OFFLINE mode!\n\nTo experience all that the game has to offer, including multiplayer, please log into your HNID account.\n\nAre you sure you want to continue?", "Offline Mode Warning", MessageBoxButtons.YesNo);
                if (offlineMessage == DialogResult.No) return;
            }
            
            // Create new process definition
            ProcessStartInfo gameProcess = new ProcessStartInfo();
            gameProcess.FileName = Path.Combine("bin" + (use64bit ? "64" : ""), "cardboard_msvc.exe");
            gameProcess.Arguments = "-q\"" + config.homeDir + "\" -glog.txt -c"+launchToken + (qConnectChkBox.Checked ? " -x\"connect "+config.qConnectServ+"\"" : "");
            
            
            // Attempt to start process with correct arguments
            try
            {
                Process.Start(gameProcess);
                Environment.Exit(0); // Close launcher if successful game launch
            } 
            catch(Exception ex)
            {
                DisplayMessage("Error while starting game process: "+ex.Message,"Error",MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
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
            if(e.Url.ToString() == "about:blank") { return; }

            string extForce = "#_force";

            if(e.Url.ToString().EndsWith(extForce))
            {
                //cancel the current event
                e.Cancel = true;

                //this opens the URL in the user's default browser
                Process.Start(e.Url.ToString().Remove(e.Url.ToString().Length-extForce.Length));
            }

            string safeSite = "https://harpnetstudios.com/";
            Console.WriteLine(e.Url.ToString());
            if (!e.Url.ToString().StartsWith(safeSite))
            {
                webWarn.BackColor = Color.Red;
            }
            else webWarn.BackColor = this.BackColor;
        }

        private void gameTokenBtn_Click(object sender, EventArgs e)
        {
            GametokenDialog gtDialog = new GametokenDialog();
            gtDialog.gameToken = config.gameToken;
            if (gtDialog.ShowDialog() == DialogResult.OK)
            {
                setGameToken(gtDialog.gameToken, false);
            }
        }

        private void qConnectChkBox_CheckedChanged(object sender, EventArgs e)
        {
            qConnectServBox.Enabled = qConnectChkBox.Checked;
        }

        private void qConnectServBox_TextChanged(object sender, EventArgs e)
        {
            config.qConnectServ = qConnectServBox.Text;
        }

        private void saveConfigChkBox_CheckedChanged(object sender, EventArgs e)
        {
            saveConfigChkBox.ForeColor = saveConfigChkBox.Checked ? Color.FromArgb(255, 36, 0) : SystemColors.HighlightText;
        }

        private void pageSelectCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            int ps = pageSelectCombo.SelectedIndex;
            if (ps == pageSelected) return; // Don't flash the screen trying to change the page, it looks bad. -Y

            webWarn.Visible = false;
            advSettings.Visible = false;

            switch (ps)
            {
                case 0: // HNID
                    webWarn.Visible = true;
                    break;

                case 1: // Advanced Settings
                    advSettings.Visible = true;
                    break;

                case 2: // About
                    break;

                default: 
                    break;
            }

            pageSelected = ps;
        }

        private void closeBtn_MouseClick(object sender, MouseEventArgs e)
        {
            Application.Exit();
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
            if (this.drag)
            { // if we should be dragging it, we need to figure out some movement
                Point p1 = new Point(e.X, e.Y);
                Point p2 = this.PointToScreen(p1);
                Point p3 = new Point(p2.X - this.startPoint.X,
                                     p2.Y - this.startPoint.Y);
                this.Location = p3;
            }
        }

        private void playOfflineChkBox_CheckedChanged(object sender, EventArgs e)
        {
            if(playOfflineChkBox.Checked && !success)
            {
                playButton.Enabled = true;
            }
            else if(!success)
            {
                playButton.Enabled = false;
            }
        }
    }
}
