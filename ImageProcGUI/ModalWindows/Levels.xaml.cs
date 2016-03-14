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
using ImageProcGUI.Controls;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace ImageProcGUI.ModalWindows
{
    /// <summary>
    /// Interaction logic for Levels.xaml
    /// </summary>
    public partial class Levels : Window
    {
        public Levels(cImage selectedImage)
        {
            InitializeComponent();
            if (!DesignerProperties.GetIsInDesignMode(this))
            {
                mainImage = selectedImage;
                HistChannels = new ObservableCollection<ChannelHistogram>();
                histAdd = histogramAdd;
                histClear = histogramClear;
                this.histPTR = CImageTools.HISTInitialise(selectedImage.ImagePointer, histImage.ImagePointer, histAdd, histClear);
                CImageTools.HISTCalculate(this.histPTR);
            }
        }
        cImage mainImage = null;
        IntPtr histPTR = IntPtr.Zero;
        CImageTools.HistogramAdd histAdd;
        CImageTools.HistogramClear histClear;

        public readonly static DependencyProperty _histChannels = DependencyProperty.Register("HistChannels", typeof(ObservableCollection<ChannelHistogram>), typeof(Levels), new PropertyMetadata(null));

        public ObservableCollection<ChannelHistogram> HistChannels
        {
            get { return (ObservableCollection<ChannelHistogram>)GetValue(_histChannels); }
            protected set { SetValue(_histChannels, value); }
        }

        private void histogramAdd(float average, float standard_dev, UInt64 max_val, string channel)
        {
            HistChannels.Add(new ChannelHistogram()
            {
                Average = average,
                Standard_Dev = standard_dev,
                Max_Val = max_val,
                Channel = channel
            });
        }

        private void histogramClear()
        {
            HistChannels.Clear();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.HISTEqualize(this.histPTR);
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.SaveFileDialog sfd = new System.Windows.Forms.SaveFileDialog();
            sfd.Filter = "TIFF images (*.tiff,*.tif)|*.tiff;*.tif";
            sfd.InitialDirectory = System.IO.Path.GetDirectoryName(mainImage.ImagePath);
            sfd.FileName = "histogram_" + System.IO.Path.GetFileName(mainImage.ImagePath);
            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                CImageTools.SaveImage(histImage.ImagePointer, sfd.FileName,false);
            }
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            CImageTools.HISTReset(this.histPTR, true);
        }

        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Hist_Closing(object sender, CancelEventArgs e)
        {
            histImage.Dispose();
            CImageTools.HISTFree(this.histPTR);
        }

        private void Button_Click_4(object sender, RoutedEventArgs e)
        {
            CImageTools.HISTReset(this.histPTR, false);
            this.Close();
        }
    }

    public class ChannelHistogram 
    {
        private float _average = 0.0f;

        public float Average
        {
            get { return _average; }
            set
            {
                _average = value;
            }
        }

        private float _std_dev = 0.0f;

        public float Standard_Dev
        {
            get { return _std_dev; }
            set
            {
                _std_dev = value;
            }
        }

        private UInt64 _max_val;

        public UInt64 Max_Val
        {
            get { return _max_val; }
            set
            {
                _max_val = value;
            }
        }

        private string _channel;
        public string Channel
        {
            get { return _channel; }
            set
            {
                _channel = value;
            }
        }
    }
}
