using ImageProcGUI.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace ImageProcGUI.ModalWindows
{
    /// <summary>
    /// Interaction logic for Fourier.xaml
    /// </summary>
    public partial class Fourier : Window
    {
        public Fourier(cImage selectedimg)
        {
            InitializeComponent();
            img = selectedimg;
        }

        cImage img;

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.FOURIERCalculate(img.ImagePointer,(Byte)1);
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            CImageTools.FOURIERCalculate(img.ImagePointer,(Byte)0);
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
