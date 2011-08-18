using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace recursiveDir
{
    class Program
    {
        static void Main(string[] args)
        {
            
            System.IO.DirectoryInfo di = new System.IO.DirectoryInfo(@"C:\Documents and Settings\a\My Documents\Downloads\\TorrentSeedFiles");

            foreach (System.IO.DirectoryInfo dif in di.GetDirectories())
            {
                System.Console.WriteLine( dif.Name );

                foreach (System.IO.FileInfo f in dif.GetFiles())
                {
                    //listBox1.Items.Add(f.Name + "  " + f.Extension)
                    System.Console.WriteLine(f.Name + "  " + f.Extension);
                }
            }
        }
    }
}
