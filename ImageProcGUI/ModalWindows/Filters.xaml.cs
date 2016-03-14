using ImageProcGUI.Controls;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Reflection;
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
    /// Interaction logic for Filters.xaml
    /// </summary>
    public partial class Filters : Window
    {
        public Filters(cImage selectedimg)
        {
            img = selectedimg;
            InitializeComponent();
            this.Closing += (a, b) =>
            {
                foreach ( Filter filt in list.Items)
                {
                    CImageTools.FILTERSFree(filt.FilterPointer);
                }
            };
            filtAdd = filterAdd;
            filtClear = filterClear;
            FiltersCol = new ObservableCollection<Filter>();
            CImageTools.FILTERSInitialise(filtAdd, filterClear);
            DirectoryInfo di = new DirectoryInfo(System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + "\\Filters");
            var directories = di.GetFiles("*", SearchOption.AllDirectories);
            List<string> files = new List<string>();
            foreach (FileInfo i in directories)
            {
                files.Add(i.FullName.ToString());
            }
            CImageTools.FILTERSLoad(files.ToArray(), files.Count);
        }

        CImageTools.FiltersAdd filtAdd;
        CImageTools.FiltersClear filtClear;
        private cImage img;

        public readonly static DependencyProperty _filters = DependencyProperty.Register("FiltersCol", typeof(ObservableCollection<Filter>), typeof(Filters), new PropertyMetadata(null));

        public ObservableCollection<Filter> FiltersCol
        {
            get { return (ObservableCollection<Filter>)GetValue(_filters); }
            protected set { SetValue(_filters, value); }
        }

        private void filterAdd(IntPtr pointer, string name)
        {
            FiltersCol.Add(new Filter()
            {
                FilterPointer = pointer,
                Name = name
            });
        }

        private void filterClear()
        {
            FiltersCol.Clear();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.FILTERSApply(img.ImagePointer, ((Filter)list.SelectedItem).FilterPointer);
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }

    public class Filter
    {
        private IntPtr _mem = IntPtr.Zero;

        public IntPtr FilterPointer
        {
            get { return _mem; }
            set
            {
                _mem = value;
            }
        }

        private string _name = null;

        public string Name
        {
            get { return _name; }
            set
            {
                _name = value;
            }
        }
    }
}
