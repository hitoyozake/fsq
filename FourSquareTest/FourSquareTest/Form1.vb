Imports System.Net

Public Class Form1

    Private Sub Button1_Click(sender As System.Object, e As System.EventArgs) Handles Button1.Click
        Dim fsa As FourSquareAPI = New FourSquareAPI()


    End Sub
End Class

Public Class FourSquareAPI
    Public Sub Create()



    End Sub



    Public Function GetVenueList(ByVal parameters As String, ByVal username As String, ByVal password As String) As Tuple(Of String, Integer)
        Dim uri As String = ""
        Dim webRequest As HttpWebRequest = CType(Net.WebRequest.Create(uri), HttpWebRequest)
        webRequest.PreAuthenticate = True
        webRequest.UseDefaultCredentials = "Mozilla 7.0"

        Dim authString As String = username + ":" + password
        Dim authbytes(authString.Length) As Byte
        authbytes = System.Text.Encoding.UTF8.GetBytes(authString)
        webRequest.Headers.Add("Authorization", "Basic" + Convert.ToBase64String(authbytes))
        webRequest.ContentType = "application/x-www-form-urlencoded"
        webRequest.Method = "Post"

        Dim bytes() As Byte = System.Text.Encoding.ASCII.GetBytes(parameters)

        Dim requestStream As IO.Stream = Nothing

        Dim resultStr As String = ""

        Try
            webRequest.ContentLength = bytes.Length
            requestStream = webRequest.GetRequestStream
            requestStream.Write(bytes, 0, bytes.Length)
        Catch ex As Exception
            resultStr = "HttpPost: REquest Error: " + ex.Message
        End Try
        If Not requestStream Is Nothing Then
            requestStream.Close()
        End If

        Try
            Dim webResponse As WebResponse = webRequest.GetResponse

            If webResponse Is Nothing Then
                Return Nothing
            End If
            Dim streamReader As IO.StreamReader = New IO.StreamReader(webResponse.GetResponseStream())
        Catch ex As Exception
            resultStr = "HttpPost: Response Error: " + ex.Message
        End Try


        Dim result As Tuple(Of String, Integer) = New Tuple(Of String, Integer)(resultStr, 0)



        Return result
    End Function


End Class

