using ImageProcGUI.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
    /// Interaction logic for Sinogram.xaml
    /// </summary>
    public partial class Sinogram : Window
    {
        private cImage image;
        public Sinogram(cImage img)
        {
            InitializeComponent();
            image = img;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.grayscale(image.ImagePointer);
            if (nfilt.IsChecked.Value)
            {
                CImageTools.SINOGRAMBackprojection(image.ImagePointer);
            }
            else if(hamfilt.IsChecked.Value)
            {
                CImageTools.SINOGRAMFilteredBackprojection(image.ImagePointer, CImageTools.Window_type.TYPE_HAMMING);
            }
            else if(hanfilt.IsChecked.Value)
            {
                CImageTools.SINOGRAMFilteredBackprojection(image.ImagePointer, CImageTools.Window_type.TYPE_HANNING);
            }
            this.Close();
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

    }
}
