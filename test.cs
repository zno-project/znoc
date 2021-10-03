using System;
using System.Drawing;

namespace _210930
{
    class Program
    {

        static void Main(string[] args)
        {
            Bitmap img = new Bitmap("*imagePath*");
            for (int i = 0; i < img.Width; i++)
            {
                for (int j = 0; j < img.Height; j++)
                {
                    Color pixel = img.GetPixel(i, j);
                }
            }
        }
    }
}