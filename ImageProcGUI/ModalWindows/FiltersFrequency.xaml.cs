using ImageProcGUI.Controls;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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
    /// Interaction logic for FiltersFrequency.xaml
    /// </summary>
    /// 

    

    public partial class FiltersFrequency : Window
    {
        private ChartStyleGridlines cs;
        private DataCollection dc;
        private DataSeries ds;
        private cImage image;
        private bool applyRunned = false;


        public static readonly DependencyProperty CurrentTimeProperty = DependencyProperty.Register("Filters", typeof(List<FilterFreq>), typeof(Window), new FrameworkPropertyMetadata(null));
        public List<FilterFreq> Filters
        {
            get { return (List<FilterFreq>)GetValue(CurrentTimeProperty); }
            set { SetValue(CurrentTimeProperty, value); }
        }

        public FiltersFrequency(cImage img)
        {
            image = img;
            Filters = new List<FilterFreq>();
            Filters.Add(new Hamming(1024) { Title = "Hamming" });
            Filters.Add(new Hanning(1024) { Title = "Hanning" });
            Filters.Add(new Ramp(1024) { Title = "Ramp" });
            Filters.Add(new Triangular(1024) { Title = "Triangular" });
            Filters.Add(new Welch(1024) { Title = "Welch" });
            Filters.Add(new Gaussian(1024) { Title = "Gaussian σ=0.4" });
            Filters.Add(new Flat_top(1024) { Title = "Flat top" });
            Filters.Add(new Poisson(1024) { Title = "Poisson τ=N/2" });
            InitializeComponent();
        }

        private void chartGrid_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            textCanvas.Width = chartGrid.ActualWidth;
            textCanvas.Height = chartGrid.ActualHeight;
            CheckBox_Checked_1(null, null);
        }

        private void CheckBox_Checked_1(object sender, RoutedEventArgs e)
        {
            chartCanvas.Children.Clear();
            textCanvas.Children.RemoveRange(1, textCanvas.Children.Count - 1);
            textCanvas.Width = chartGrid.ActualWidth;
            textCanvas.Height = chartGrid.ActualHeight;
            chartCanvas.Children.Clear();
            textCanvas.Children.RemoveRange(1, textCanvas.Children.Count - 1);
            cs = new ChartStyleGridlines();
            dc = new DataCollection();
            var convolution = new List<Point>();
            ds = new DataSeries();
            ds.LineColor = Brushes.Blue;
            ds.LineThickness = 2;
            double max = 0.0, min = 0.0;
            foreach (FilterFreq ft in Filters)
            {
                if (ft.Enabled)
                {
                    if (convolution.Count == 0)
                    {
                        convolution.AddRange(ft.points);
                        max = ft.max;
                        min = ft.min;
                        cs.Title = ft.Title;
                    }
                    else
                    {
                        for (int i = 0; i < convolution.Count(); i++)
                        {
                            convolution[i] = new Point(i, convolution[i].Y * ft.points[i].Y);
                        }
                        max *= ft.max;
                        min *= ft.min;
                        cs.Title += "*" + ft.Title;
                    }
                }
            }
            if (convolution.Count == 0)
            {
                for (int i = 0; i < 1024; i++)
                {
                    ds.LineSeries.Points.Add(new Point(i, 1));
                }
                max = 1.02;
                cs.Title = "No Filter";
            }
            else
            {
                foreach (Point pt in convolution)
                {
                    ds.LineSeries.Points.Add(pt);
                    
                }
            }
            cs.ChartCanvas = chartCanvas;
            cs.TextCanvas = textCanvas;
            cs.YLabel = "amplitude";
            cs.XLabel = "samples";
            cs.Xmin = 0;
            cs.Xmax = 1024;
            cs.XTick = 128;
            cs.YTick = 1;
            cs.Ymax = Math.Round(max)+1.0;
            cs.Ymin = -Math.Round(max) - 1.0;
            cs.GridlinePattern = ChartStyleGridlines.GridlinePatternEnum.Dot;
            cs.GridlineColor = Brushes.Black;
            cs.AddChartStyle(tbTitle, tbXLabel, tbYLabel);
            dc.DataList.Add(ds);
            dc.AddLines(cs);
            applyRunned = false ;
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            if (cs.Title == "No Filter")
            {
                CImageTools.SINOGRAMBackprojection(image.ImagePointer);
            }
            else
            {
                double[] filter = new double[1024];
                for (int i = 0; i < 1024; i++)
                {
                    filter[i] = 1;
                }
                foreach (FilterFreq ft in Filters)
                {
                    if (ft.Enabled)
                    {
                        for (int i = 0; i < 1024; i++)
                        {
                            filter[i] *= ft.points[i].Y;
                        }
                    }
                }
                CImageTools.SINOGRAMFilt(image.ImagePointer, filter);
            }
            applyRunned = true;
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            if (!applyRunned)
            {
                Button_Click_1(null, null);
            }
            Button_Click_3(null, null);
        }

        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    
    }

    public class ChartStyle
    {
        private Canvas chartCanvas;
        private double xmin = 0;
        private double xmax = 10;
        private double ymin = 0;
        private double ymax = 10;
        public Canvas ChartCanvas
        {
            get { return chartCanvas; }
            set { chartCanvas = value; }
        }
        public double Xmin
        {
            get { return xmin; }
            set { xmin = value; }
        }
        public double Xmax
        {
            get { return xmax; }
            set { xmax = value; }
        }
        public double Ymin
        {
            get { return ymin; }
            set { ymin = value; }
        }
        public double Ymax
        {
            get { return ymax; }
            set { ymax = value; }
        }
        public Point NormalizePoint(Point pt)
        {
            if (ChartCanvas.Width.ToString() == "NaN")
                ChartCanvas.Width = 270;
            if (ChartCanvas.Height.ToString() == "NaN")
                ChartCanvas.Height = 250;
            Point result = new Point();
            result.X = (pt.X - Xmin) * ChartCanvas.Width / (Xmax - Xmin);
            result.Y = ChartCanvas.Height -
            (pt.Y - Ymin) * ChartCanvas.Height / (Ymax - Ymin);
            return result;
        }
    }

    public class DataSeries
    {
        private Polyline lineSeries = new Polyline();
        private Brush lineColor;
        private double lineThickness = 1;
        private LinePatternEnum linePattern;
        private string seriesName = "Default Name";
        //private Symbols symbols;
        public DataSeries()
        {
            LineColor = Brushes.Black;
            // symbols = new Symbols();
        }
        /*public Symbols Symbols
        {
        get { return symbols; }
        set { symbols = value; }
        }*/
        public Brush LineColor
        {
            get { return lineColor; }
            set { lineColor = value; }
        }
        public Polyline LineSeries
        {
            get { return lineSeries; }
            set { lineSeries = value; }
        }
        public double LineThickness
        {
            get { return lineThickness; }
            set { lineThickness = value; }
        }
        public LinePatternEnum LinePattern
        {
            get { return linePattern; }
            set { linePattern = value; }
        }
        public string SeriesName
        {
            get { return seriesName; }
            set { seriesName = value; }
        }
        public void AddLinePattern()
        {
            LineSeries.Stroke = LineColor;
            LineSeries.StrokeThickness = LineThickness;
            switch (LinePattern)
            {
                case LinePatternEnum.Dash:
                    LineSeries.StrokeDashArray =
                    new DoubleCollection(new double[2] { 4, 3 });
                    break;
                case LinePatternEnum.Dot:
                    LineSeries.StrokeDashArray =
                    new DoubleCollection(new double[2] { 1, 2 });
                    break;
                case LinePatternEnum.DashDot:
                    LineSeries.StrokeDashArray =
                    new DoubleCollection(new double[4] { 4, 2, 1, 2 });
                    break;
                case LinePatternEnum.None:
                    LineSeries.Stroke = Brushes.Transparent;
                    break;
            }
        }
        public enum LinePatternEnum
        {
            Solid = 1,
            Dash = 2,
            Dot = 3,
            DashDot = 4,
            None = 5
        }
    }

    public class DataCollection
    {
        private List<DataSeries> dataList;
        public DataCollection()
        {
            dataList = new List<DataSeries>();
        }
        public List<DataSeries> DataList
        {
            get { return dataList; }
            set { dataList = value; }
        }
        public void AddLines(ChartStyle cs)
        {
            int j = 0;
            foreach (DataSeries ds in DataList)
            {
                if (ds.SeriesName == "Default Name")
                {
                    ds.SeriesName = "DataSeries" + j.ToString();
                }
                ds.AddLinePattern();
                for (int i = 0; i < ds.LineSeries.Points.Count; i++)
                {
                    ds.LineSeries.Points[i] =
                    cs.NormalizePoint(ds.LineSeries.Points[i]);
                }
                cs.ChartCanvas.Children.Add(ds.LineSeries);
                j++;
            }
        }
    }

    public class ChartStyleGridlines : ChartStyle
    {
        private string title;
        private string xLabel;
        private string yLabel;
        private Canvas textCanvas;
        private bool isXGrid = true;
        private bool isYGrid = true;
        private Brush gridlineColor = Brushes.LightGray;
        private double xTick = 1;
        private double yTick = 0.5;
        private GridlinePatternEnum gridlinePattern;
        private double leftOffset = 20;
        private double bottomOffset = 15;
        private double rightOffset = 10;
        private Line gridline = new Line();
        public ChartStyleGridlines()
        {
            title = "Title";
            xLabel = "X Axis";
            yLabel = "Y Axis";
        }
        public string Title
        {
            get { return title; }
            set { title = value; }
        }
        public string XLabel
        {
            get { return xLabel; }
            set { xLabel = value; }
        }
        public string YLabel
        {
            get { return yLabel; }
            set { yLabel = value; }
        }
        public GridlinePatternEnum GridlinePattern
        {
            get { return gridlinePattern; }
            set { gridlinePattern = value; }
        }
        public double XTick
        {
            get { return xTick; }
            set { xTick = value; }
        }
        public double YTick
        {
            get { return yTick; }
            set { yTick = value; }
        }
        public Brush GridlineColor
        {
            get { return gridlineColor; }
            set { gridlineColor = value; }
        }
        public Canvas TextCanvas
        {
            get { return textCanvas; }
            set { textCanvas = value; }
        }
        public bool IsXGrid
        {
            get { return isXGrid; }
            set { isXGrid = value; }
        }
        public bool IsYGrid
        {
            get { return isYGrid; }
            set { isYGrid = value; }
        }
        public void AddChartStyle(TextBlock tbTitle, TextBlock tbXLabel, TextBlock tbYLabel)
        {
            Point pt = new Point();
            Line tick = new Line();
            double offset = 0;
            double dx, dy;
            TextBlock tb = new TextBlock();
            // determine right offset:
            tb.Text = Xmax.ToString();
            tb.Measure(new Size(Double.PositiveInfinity, Double.PositiveInfinity));
            Size size = tb.DesiredSize;
            rightOffset = size.Width / 2 + 2;
            // Determine left offset:
            for (dy = Ymin; dy <= Ymax; dy += YTick)
            {
                pt = NormalizePoint(new Point(Xmin, dy));
                tb = new TextBlock();
                tb.Text = dy.ToString();
                tb.TextAlignment = TextAlignment.Right;
                tb.Measure(new Size(Double.PositiveInfinity,
                Double.PositiveInfinity));
                size = tb.DesiredSize;
                if (offset < size.Width)
                    offset = size.Width;
            }
            leftOffset = offset + 5;
            Canvas.SetLeft(ChartCanvas, leftOffset);
            Canvas.SetBottom(ChartCanvas, bottomOffset);
            ChartCanvas.Width = Math.Abs(TextCanvas.Width -
            leftOffset - rightOffset);
            ChartCanvas.Height = Math.Abs(TextCanvas.Height -
            bottomOffset - size.Height / 2);
            Rectangle chartRect = new Rectangle();
            chartRect.Stroke = Brushes.Black;
            chartRect.Width = ChartCanvas.Width;
            chartRect.Height = ChartCanvas.Height;
            ChartCanvas.Children.Add(chartRect);
            // Create vertical gridlines:
            if (IsYGrid == true)
            {
                for (dx = Xmin + XTick; dx < Xmax; dx += XTick)
                {
                    gridline = new Line();
                    AddLinePattern();
                    gridline.X1 = NormalizePoint(new Point(dx, Ymin)).X;
                    gridline.Y1 = NormalizePoint(new Point(dx, Ymin)).Y;
                    gridline.X2 = NormalizePoint(new Point(dx, Ymax)).X;
                    gridline.Y2 = NormalizePoint(new Point(dx, Ymax)).Y;
                    ChartCanvas.Children.Add(gridline);
                }
            }
            // Create horizontal gridlines:
            if (IsXGrid == true)
            {
                for (dy = Ymin + YTick; dy < Ymax; dy += YTick)
                {
                    gridline = new Line();
                    AddLinePattern();
                    gridline.X1 = NormalizePoint(new Point(Xmin, dy)).X;
                    gridline.Y1 = NormalizePoint(new Point(Xmin, dy)).Y;
                    gridline.X2 = NormalizePoint(new Point(Xmax, dy)).X;
                    gridline.Y2 = NormalizePoint(new Point(Xmax, dy)).Y;
                    ChartCanvas.Children.Add(gridline);
                }
            }
            // Create x-axis tick marks:
            for (dx = Xmin; dx <= Xmax; dx += xTick)
            {
                pt = NormalizePoint(new Point(dx, Ymin));
                tick = new Line();
                tick.Stroke = Brushes.Black;
                tick.X1 = pt.X;
                tick.Y1 = pt.Y;
                tick.X2 = pt.X;
                tick.Y2 = pt.Y - 5;
                ChartCanvas.Children.Add(tick);
                tb = new TextBlock();
                tb.Text = dx.ToString();
                tb.Measure(new Size(Double.PositiveInfinity,
                Double.PositiveInfinity));
                size = tb.DesiredSize;
                TextCanvas.Children.Add(tb);
                Canvas.SetLeft(tb, leftOffset + pt.X - size.Width / 2);
                Canvas.SetTop(tb, pt.Y + 2 + size.Height / 2);
            }
            // Create y-axis tick marks:
            for (dy = Ymin; dy <= Ymax; dy += YTick)
            {
                pt = NormalizePoint(new Point(Xmin, dy));
                tick = new Line();
                tick.Stroke = Brushes.Black;
                tick.X1 = pt.X;
                tick.Y1 = pt.Y;
                tick.X2 = pt.X + 5;
                tick.Y2 = pt.Y;
                ChartCanvas.Children.Add(tick);
                tb = new TextBlock();
                tb.Text = dy.ToString();
                tb.Measure(new Size(Double.PositiveInfinity, Double.PositiveInfinity));
                size = tb.DesiredSize;
                TextCanvas.Children.Add(tb);
                Canvas.SetRight(tb, ChartCanvas.Width + 20);
                Canvas.SetTop(tb, pt.Y);
            }
            // Add title and labels:
            tbTitle.Text = Title;
            tbXLabel.Text = XLabel;
            tbYLabel.Text = YLabel;
            tbXLabel.Margin = new Thickness(leftOffset + 2, 2, 2, 2);
            tbTitle.Margin = new Thickness(leftOffset + 2, 2, 2, 2);
        }
        public void AddLinePattern()
        {
            gridline.Stroke = GridlineColor;
            gridline.StrokeThickness = 1;
            switch (GridlinePattern)
            {
                case GridlinePatternEnum.Dash:
                    gridline.StrokeDashArray =
                    new DoubleCollection(new double[2] { 4, 3 });
                    break;
                case GridlinePatternEnum.Dot:
                    gridline.StrokeDashArray =
                    new DoubleCollection(new double[2] { 1, 2 });
                    break;
                case GridlinePatternEnum.DashDot:
                    gridline.StrokeDashArray =
                    new DoubleCollection(new double[4] { 4, 2, 1, 2 });
                    break;
            }
        }
        public enum GridlinePatternEnum
        {
            Solid = 1,
            Dash = 2,
            Dot = 3,
            DashDot = 4
        }
    }

    public class MVVMBase : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string name)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(name));
            }
        }
    }

    public abstract class FilterFreq : MVVMBase
    {
        public List<Point> points;

        private bool _enabled = false;
        public bool Enabled
        {
            get { return _enabled; }
            set
            {
                if (_enabled != value)
                {
                    _enabled = value;
                    OnPropertyChanged("Enabled");
                }
            }
        }

        public double max = 0.0;
        public double min = 0.0;

        private string _title = "";
        public string Title
        {
            get { return _title; }
            set
            {
                if (_title != value)
                {
                    _title = value;
                    OnPropertyChanged("Title");
                }
            }
        }
    }

    public class Hamming : FilterFreq
    {
        public Hamming(double width)
        {
            this.points = new List<Point>();
            for (int i = 0; i < width; i++)
            {
                double y = 0.54 - 0.46 * Math.Cos((2 * Math.PI * i) / (width - 1.0));
                if (this.max < y)
                {
                    this.max = y;
                }
                else if (this.min > y)
                {
                    this.min = y;
                }
                this.points.Add(new Point(i, y));
            }
        }
    }

    public class Hanning : FilterFreq
    {
        public Hanning(double width)
        {
            this.points = new List<Point>();
            for (int i = 0; i < width; i++)
            {
                double y = 0.5 - 0.5 * Math.Cos((2 * Math.PI * i) / (width - 1.0));
                if (this.max < y)
                {
                    this.max = y;
                }
                else if (this.min > y)
                {
                    this.min = y;
                }
                this.points.Add(new Point(i, y));
            }
        }
    }

    public class Ramp : FilterFreq
    {
        public Ramp(double width)
        {
            this.points = new List<Point>();
            for (int i = 0; i < width; i++)
            {
                double y = Math.Abs((i - (width - 1) / 2.0) / (width / 2.0));
                if (this.max < y)
                {
                    this.max = y;
                }
                else if (this.min > y)
                {
                    this.min = y;
                }
                this.points.Add(new Point(i, y));
            }
        }
    }

    public class Triangular : FilterFreq
    {
        public Triangular(double width)
        {
            this.points = new List<Point>();
            double max = 0;
            for (int i = 0; i < width; i++)
            {
                double y =1 - Math.Abs((i - (width -1)/ 2.0)/(width/2.0));
                if (this.max < y)
                {
                    this.max = y;
                }
                else if (this.min > y)
                {
                    this.min = y;
                }
                this.points.Add(new Point(i, y));
            }
        }
    }

    public class Welch : FilterFreq
    {
        public Welch(double width)
        {
            this.points = new List<Point>();
            double max = 0;
            for (int i = 0; i < width; i++)
            {
                double y = 1 - Math.Pow(((i - (width - 1) / 2.0) / ((width+1.0) / 2.0)),2);
                if (this.max < y)
                {
                    this.max = y;
                }
                else if (this.min > y)
                {
                    this.min = y;
                }
                this.points.Add(new Point(i, y));
            }
        }
    }

    public class Gaussian : FilterFreq
    {
        public Gaussian(double width)
        {
            this.points = new List<Point>();
            double max = 0;
            for (int i = 0; i < width; i++)
            {
                double y = Math.Pow(Math.E, -0.5 * Math.Pow(((i - ((width - 1) / 2)) / (0.4 * (width - 1) / 2)), 2));
                if (this.max < y)
                {
                    this.max = y;
                }
                else if (this.min > y)
                {
                    this.min = y;
                }
                this.points.Add(new Point(i, y));
            }
        }
    }

    public class Flat_top : FilterFreq
    {
        public Flat_top(double width)
        {
            this.points = new List<Point>();
            for (int i = 0; i < width; i++)
            {
                double y = 1 - 1.93 * Math.Cos(2 * Math.PI * i / (width - 1)) + 1.29 * Math.Cos(4 * Math.PI * i / (width - 1)) - 0.388 * Math.Cos(6 * Math.PI * i / (width - 1)) + 0.028 * Math.Cos(8 * Math.PI * i / (width - 1));
                if (this.max < y)
                {
                    this.max = y;
                }
                else if (this.min > y)
                {
                    this.min = y;
                }
                this.points.Add(new Point(i, y));
            }

        }
    }

    public class Poisson : FilterFreq
    {
        public Poisson(double width)
        {
            this.points = new List<Point>();
            for (int i = 0; i < width; i++)
            {
                double y = Math.Pow(Math.E,-Math.Abs(i-((width-1.0)/2.0))*(1/(width/2)));
                if (this.max < y)
                {
                    this.max = y;
                }
                else if (this.min > y)
                {
                    this.min = y;
                }
                this.points.Add(new Point(i, y));
            }

        }
    }
}
