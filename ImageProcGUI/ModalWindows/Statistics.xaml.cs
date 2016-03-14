using ImageProcGUI.Controls;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
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
    /// Interaction logic for Statistics.xaml
    /// </summary>
    public partial class Statistics : Window
    {
        
        public Statistics(cImage img)
        {
            InitializeComponent();
            if (!DesignerProperties.GetIsInDesignMode(this))
            {
                StatsChannels = new ObservableCollection<ChannelStatistic>();
                staAdd = statisticsAdd;
                staClear = statisticsClear;
                CImageTools.STATSCalculate(img.ImagePointer, staAdd, staClear);
            }
        }

        CImageTools.StatisticsAdd staAdd;
        CImageTools.StatisticsClear staClear;

        public readonly static DependencyProperty _statsChannels = DependencyProperty.Register("StatsChannels", typeof(ObservableCollection<ChannelStatistic>), typeof(Statistics), new PropertyMetadata(null));

        public ObservableCollection<ChannelStatistic> StatsChannels
        {
            get { return (ObservableCollection<ChannelStatistic>)GetValue(_statsChannels); }
            protected set { SetValue(_statsChannels, value); }
        }

        private void statisticsAdd(float average, float standard_dev, float correlation_hor_f, float correlation_hor_s, float correlation_ver_f, float correlation_ver_s, string channel)
        {
            StatsChannels.Add(new ChannelStatistic()
            {
                Average = average,
                Standard_Dev = standard_dev,
                Cor_Hor_1st = correlation_hor_f,
                Cor_Ver_1st = correlation_ver_f,
                Cor_Hor_2nd = correlation_hor_s,
                Cor_Ver_2nd = correlation_ver_s,
                Channel = channel
            });
        }

        public void statisticsClear()
        {
            StatsChannels.Clear();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }



    public class ChannelStatistic
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

        private float _cor_hor_1st = 0.0f;

        public float Cor_Hor_1st
        {
            get { return _cor_hor_1st; }
            set
            {
                _cor_hor_1st = value;
            }
        }

        private float _cor_ver_1st = 0.0f;

        public float Cor_Ver_1st
        {
            get { return _cor_ver_1st; }
            set
            {
                _cor_ver_1st = value;
            }
        }

        private float _cor_hor_2nd = 0.0f;

        public float Cor_Hor_2nd
        {
            get { return _cor_hor_2nd; }
            set
            {
                _cor_hor_2nd = value;
            }
        }

        private float _cor_ver_2nd = 0.0f;

        public float Cor_Ver_2nd
        {
            get { return _cor_ver_2nd; }
            set
            {
                _cor_ver_2nd = value;
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
