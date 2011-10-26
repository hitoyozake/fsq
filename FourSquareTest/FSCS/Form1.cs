using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FSCS
{
    public partial class Form1 : Form
    {
        public Form1 ()
        {
            InitializeComponent();
        }

        private void Form1_Load ( object sender, EventArgs e )
        {
            var fsa = new FourSquareAPI();
            var url = fsa.MakeRequest();

            richTextBox1.Text = url;
        }
    }

    public class FourSquareAPI
    {
        static private String clientID = "EEX2G05PTTJBYTA4SLHNYXVZYZNKLCCKDKJM05MXPT4V5RG0";
        static private String secretID = "LOSPORV2HFBWUXHROKN4MOQ1YWFHC41AUXO4DFTMFOIOJXGY";
        static private String uri = "";
        static private String secretURI = "http://d.hatena.ne.jp/mikoto1212/";
        static private String authURL = "https://foursquare.com/oauth2/authenticate?client_id="
                + clientID + "&response_type=code&redirect_uri="
                + secretURI;
        static private String code = "M4DKOG4JGYSG1E1RLPOZKXV2QCYOOE3R13DVKEL0HQCHHSH0";
        static private string accessToken = "TTI1DEVFLWZYZZLXGHVRGMVLVD3JUREKQ5UTK5HKJZ2RYGT1";

        public String GetAuthURL()
        {
            return authURL;
        }

        public String MakeRequest()
        {
            String request = "https://foursquare.com/oauth2/access_token"
            + "?client_id=" + clientID
            + "&client_secret=" + secretID
            + "&grant_type=authorization_code"
            + "&redirect_uri=" + secretURI
            + "&code=" + code;

            return request;

        }


    }
}
