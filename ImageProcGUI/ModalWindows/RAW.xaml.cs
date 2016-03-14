using ImageProcGUI.Controls;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
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
    /// Interaction logic for RAW.xaml
    /// </summary>
    public partial class RAW : Window
    {
        public string[] channels = { "Red", "Green", "Blue" };
        private cImage img;
        public RAW(string[] files, cImage image)
        {
            InitializeComponent();
            img = image;
            Files = new ObservableCollection<RAWFile>();
            int i = 3;
            
            if (files.Count() < i)
            {
                i = files.Count();
            }
            for (int j = 0; j < 1; j++)
            {
                Files.Add(new RAWFile() { Path = files[j], Channels = channels, SelIndex=j });
            }
        }

        public readonly static DependencyProperty _files = DependencyProperty.Register("Files", typeof(ObservableCollection<RAWFile>), typeof(RAW), new PropertyMetadata(null));

        public ObservableCollection<RAWFile> Files
        {
            get { return (ObservableCollection<RAWFile>)GetValue(_files); }
            protected set { SetValue(_files, value); }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            string[] files = new string[3];

            for(int i=0; i<Files.Count;i++)
            {
                files[i] = Files[i].Path;
            }
            Debug.WriteLine(combBits.Items[0].ToString());
            CImageTools.LoadRAWImage(img.ImagePointer,files,(short)Files.Count(),short.Parse(((ComboBoxItem) combBits.SelectedItem).Content.ToString()), UInt32.Parse(widthText.Text),UInt32.Parse(heightText.Text));
            this.Close();
        }

        private string previousText = "0";

        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {

            int num = 0;
            if (((TextBox)sender).Text == "")
            {
                ((TextBox)sender).Text = "0";
                ((TextBox)sender).Select(0,((TextBox)sender).Text.Length);
            }
            bool success = int.TryParse(((TextBox)sender).Text, out num);
            if (success & num >= 0)
                previousText = ((TextBox)sender).Text;
            else
            {
                ((TextBox)sender).Text = previousText;
                ((TextBox)sender).Select(((TextBox)sender).Text.Length,0);
            }

        }

        private string previousTextHeight = "0";

        private void TextBox_TextChanged_1(object sender, TextChangedEventArgs e)
        {

            int num = 0;
            if (((TextBox)sender).Text == "")
            {
                ((TextBox)sender).Text = "0";
                ((TextBox)sender).Select(0, ((TextBox)sender).Text.Length);
            }
            bool success = int.TryParse(((TextBox)sender).Text, out num);
            if (success & num >= 0)
                previousTextHeight = ((TextBox)sender).Text;
            else
            {
                ((TextBox)sender).Text = previousTextHeight;
                ((TextBox)sender).Select(((TextBox)sender).Text.Length, 0);
            }

        }

    }

    public class RAWFile : INotifyPropertyChanged

    {
        private string _path = null;
        public string Path
        {
            get { return _path; }
            set
            {
                if (_path != value)
                {
                    _path = value;
                    RaisePropertyChanged("Path");
                }
            }
        }

        private int _selIndex = 0;
        public int SelIndex
        {
            get { return _selIndex; }
            set
            {
                if (_selIndex != value)
                {
                    _selIndex = value;
                    RaisePropertyChanged("SelIndex");
                }
            }
        }

        private string[] _channel = null;
        public string[] Channels
        {
            get { return _channel; }
            set
            {
                if (_channel != value)
                {
                    _channel = value;
                    RaisePropertyChanged("Channels");
                }

            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected void RaisePropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }

    public class ButtonEnabledConverter : IMultiValueConverter
    {

        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            try
            {
                if ((int.Parse((string)values[0]) > 0) & (int.Parse((string)values[1]) > 0))
                {
                    return true;
                }
                else return false;
            }
            catch 
            {
                return false;
            }
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
