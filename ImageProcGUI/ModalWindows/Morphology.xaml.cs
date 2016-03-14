using ImageProcGUI.Controls;
using System;
using System.Collections.Generic;
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
    /// Interaction logic for Morphology.xaml
    /// </summary>
    public partial class Morphology : Window
    {

        private cImage img;
        public Morphology(cImage selectedimg)
        {
            InitializeComponent();
            img = selectedimg;
            DirectoryInfo di = new DirectoryInfo(System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + "\\Patterns");
            var directories = di.GetFiles("*", SearchOption.AllDirectories);
            List<string> files = new List<string>();
            foreach (FileInfo i in directories)
            {
                files.Add(i.FullName.ToString());
            }
            CImageTools.MORPHLoad(files.ToArray(), files.Count);
        }

       
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (ShrinkRB.IsChecked.Value)
            {
                CImageTools.MORPHProcess(img.ImagePointer,(int)Value.Value, CImageTools.Process_Type.PR_TYPE_SHRINK);
            }
            else if (ThinkRB.IsChecked.Value)
            {
                CImageTools.MORPHProcess(img.ImagePointer, (int)Value.Value, CImageTools.Process_Type.PR_TYPE_THIN);
            }
            else if (SkeletRB.IsChecked.Value)
            {
                CImageTools.MORPHProcess(img.ImagePointer, (int)Value.Value, CImageTools.Process_Type.PR_TYPE_SKELET);
            }
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
}
