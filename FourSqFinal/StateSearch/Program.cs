using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace StateSearch
{
    class Program
    {


        static void Main(string[] args)
        {
            var sr = new StreamReader("input.txt");

            var wr = new StreamWriter("searched.txt");

            bool find_flag = false;
            var data = new List< String >();

            wr.WriteLine("{\"data\" :[");

            sr.ReadLine();


            while (!sr.EndOfStream)
            {
                String input = sr.ReadLine();

                if (input.Length > 0)
                {
                    if (input[0] == '{' && input.Length <= 1)
                    {
                        if (find_flag == true)
                        {
                            for (int i = 0; i < data.Count; ++i)
                            {
                                wr.WriteLine( data[ i ] );
                            }
                        }
                        data.Clear();
                        find_flag = false;
                    }

                    data.Add(input);

                    if (input.IndexOf("\"京都") > 0)
                    {
                        find_flag = true;
                    }

                }
            }
            for (int i = 0; i < data.Count; ++i)
            {
                wr.WriteLine(data[i]);
            }
        }
    }
}
