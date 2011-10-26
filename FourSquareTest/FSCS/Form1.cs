using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.IO;

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
            var fsa = new FourSquareAPI( "setting.txt" );
            var text = fsa.SendRequest( "users/self" );

            richTextBox1.Text = text;
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
        static private string accessToken = "";

        static String apiURI = "https://api.foursquare.com/v2/";


        public String ReadSettings ( String filename )
        {
            var reader = new StreamReader( filename );

            var token = reader.ReadLine();

            reader.Close();

            return token;
        }

        public FourSquareAPI( String filename )
        {
            accessToken = ReadSettings( filename );
        }


        public String GetAuthURL()
        {
            return authURL;
        }

        public String SendRequest( String req )
        {
            String reqStr = apiURI + req + "?oauth_token=" + accessToken;

            var client = new WebClient();
            var stream = client.OpenRead( reqStr );

            var sr = new StreamReader( stream, Encoding.GetEncoding( "UTF-8" ) );

            String result = "";

            while( !sr.EndOfStream )
            {
                result += sr.ReadLine();
            }
            return result;
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
