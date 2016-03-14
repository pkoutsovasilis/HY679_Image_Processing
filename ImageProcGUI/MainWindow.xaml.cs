using ImageProcGUI.Controls;
using ImageProcGUI.ModalWindows;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Shell;

namespace ImageProcGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public unsafe partial class MainWindow : Window
    {
        [DllImport("Kernel32.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern bool SetDllDirectory(String lpPathName);
        CImageTools.ErrorCallback errorCB;

        public static RoutedCommand ImageClose = new RoutedCommand();
        private void CloseExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            List<object> selectedItems = new List<object>();
            foreach (object item in OpenedImages.SelectedItems)
            {
                selectedItems.Add(item);
            }
            foreach (object item in selectedItems)
            {
                OpenedImages.Items.Remove(item);
            }
            if (OpenedImages.Items.Count > 0)
            {
                OpenedImages.SelectedIndex = OpenedImages.Items.Count - 1;
            }
            selectedItems.Clear();
            GC.Collect();
        }

        private void CloseCanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            if (OpenedImages.SelectedItems.Count > 0)
            {
                e.CanExecute = true;
            }
            else
            {
                e.CanExecute = false;
            }
        }

        public static RoutedCommand ImageSinogram = new RoutedCommand();
        private void SinogramExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            FiltersFrequency mWFilt = new FiltersFrequency(((cImage)OpenedImages.SelectedItem));
            mWFilt.Owner = this;
            mWFilt.ShowInTaskbar = false;
            mWFilt.ShowDialog();
        }

        public static RoutedCommand ImageOpen = new RoutedCommand();
        private void OpenExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog ofd = new System.Windows.Forms.OpenFileDialog();
            ofd.Filter = "TIFF images (*.tiff,*.tif)|*.tiff;*.tif|RAW images(*.raw)|*.raw";
            ofd.RestoreDirectory = true;
            ofd.Multiselect = true;
            if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                if (ofd.FilterIndex == 1)
                {
                    foreach (string path in ofd.FileNames)
                    {
                        cImage image = new cImage();
                        image.Height = 100;
                        image.Width = 100;
                        image.Margin = new Thickness(5, 5, 5, 5);
                        image.VerticalAlignment = System.Windows.VerticalAlignment.Stretch;
                        if (image.Load(path))
                        {
                            OpenedImages.Items.Add(image);
                            OpenedImages.SelectedIndex = OpenedImages.Items.Count - 1;
                        }
                        else
                        {
                            image = null;
                        }
                    }
                }
                else if (ofd.FilterIndex == 2)
                {
                    cImage image = new cImage();
                    image.Height = 100;
                    image.Width = 100;
                    image.Margin = new Thickness(5, 5, 5, 5);
                    image.VerticalAlignment = System.Windows.VerticalAlignment.Stretch;
                    RAW mWStats = new RAW(ofd.FileNames, image);
                    mWStats.Owner = this;
                    mWStats.ShowInTaskbar = false;
                    mWStats.ShowDialog();
                    mWStats.Icon = this.Icon;
                    OpenedImages.Items.Add(image);
                    OpenedImages.SelectedIndex = OpenedImages.Items.Count - 1;
                }
               
            }
        }

        private void OpenCanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        public static RoutedCommand ImageSave = new RoutedCommand();
        private void SaveExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            cImage img = (cImage)OpenedImages.SelectedItem;
            CImageTools.SaveImage(img.ImagePointer, img.ImagePath, false);
        }

        public static RoutedCommand ImageSaveAs = new RoutedCommand();
        private void SaveAsExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            cImage img = (cImage)OpenedImages.SelectedItem;
            System.Windows.Forms.SaveFileDialog sfd = new System.Windows.Forms.SaveFileDialog();
            sfd.Filter = "TIFF images (*.tiff,*.tif)|*.tiff;*.tif";
            sfd.InitialDirectory = System.IO.Path.GetDirectoryName(img.ImagePath);
            sfd.FileName = System.IO.Path.GetFileName(img.ImagePath);
            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                CImageTools.SaveImage(img.ImagePointer, sfd.FileName, false);
            }
        }

        public static RoutedCommand ImageStats = new RoutedCommand();
        private void StatsExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Statistics mWStats = new Statistics(((cImage)OpenedImages.SelectedItem));
            mWStats.Owner = this;
            mWStats.ShowInTaskbar = false;
            mWStats.ShowDialog();
        }

        public static RoutedCommand ImageUndo = new RoutedCommand();
        private void UndoExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            CImageTools.IMAGEUndo(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        public static RoutedCommand ImageRestore = new RoutedCommand();
        private void RestoreExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            CImageTools.IMAGERestore(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        public static RoutedCommand ImageRedo = new RoutedCommand();
        private void RedoExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            CImageTools.IMAGERedo(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        public static RoutedCommand ImageFilt = new RoutedCommand();
        private void FiltExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Filters mWFilt = new Filters(((cImage)OpenedImages.SelectedItem));
            mWFilt.Owner = this;
            mWFilt.ShowInTaskbar = false;
            mWFilt.ShowDialog();
        }

        public static RoutedCommand ImageHist = new RoutedCommand();
        private void HistExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Levels mWHist = new Levels(((cImage)OpenedImages.SelectedItem));
            mWHist.Owner = this;
            mWHist.ShowInTaskbar = false;
            mWHist.ShowDialog();
        }

        public static RoutedCommand ImageFourier = new RoutedCommand();
        private void FourierExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Fourier mWFourier = new Fourier(((cImage)OpenedImages.SelectedItem));
            mWFourier.Owner = this;
            mWFourier.ShowInTaskbar = false;
            mWFourier.ShowDialog();
        }

        public static RoutedCommand ImageMorph = new RoutedCommand();
        private void MorphExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Morphology mWMorph = new Morphology(((cImage)OpenedImages.SelectedItem));
            mWMorph.Owner = this;
            mWMorph.ShowInTaskbar = false;
            mWMorph.ShowDialog();
        }

        private void HistCanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            if (OpenedImages.SelectedItems.Count > 0)
            {
                e.CanExecute = true;
            }
            else
            {
                e.CanExecute = false;
            }
        }

        public MainWindow()
        {
            SetDllDirectory(null);
            SetDllDirectory(AppDomain.CurrentDomain.BaseDirectory + "Libs\\");

            InitializeComponent();
            if (!DesignerProperties.GetIsInDesignMode(this))
            {
                errorCB = ErrorCB;
                CImageTools.SetErrorCallback(errorCB);
            }
           
        }

        private void ErrorCB(string message)
        {
            System.Windows.Forms.MessageBox.Show(message, "Error", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
        }

        /// <summary>
        /// Disposes all opened Images
        /// </summary>
        /// <param name="sender">Main Window</param>
        /// <param name="e"></param>
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            for (int i = 0; i < OpenedImages.Items.Count; i++)
            {
                var temp = (cImage)OpenedImages.Items[i];
                temp.Dispose();
                temp = null;
            }
        }

        /// <summary>
        /// Updates the correspondig bottom text element about Mouse Position and 
        /// Image's Pixel Value at specified position
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Image_MouseMove(object sender, MouseEventArgs e)
        {
            var pos = e.GetPosition((Image)sender);
            var cImage = (cImage)OpenedImages.SelectedItem;
            if (cImage != null)
            {
                UInt32 x = (UInt32)Math.Round(pos.X), y = (UInt32)Math.Round(pos.Y);
                var result = CImageTools.IMAGEGetPixelValue(cImage.ImagePointer, x, y);
                MousePos.Content = x.ToString() + "," + y.ToString();
                PixelValue.Content = "(" + ((result >> 16) & 0xff).ToString() + "," + ((result >> 8) & 0xff).ToString() + "," + ((result >> 0) & 0xff).ToString() + ")";
            }
        }

        private void Image_MouseLeave(object sender, MouseEventArgs e)
        {
            MousePos.Content = "-";
            PixelValue.Content = "-";
        }
        private void Exit_MenuItem_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Convert_YCBCR_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.IMAGEToYCBCR(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        private void Convert_RGB_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.IMAGEToRGB(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        private void FlipHor_MenuItem_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.GEOMETRYFlipHorizontal(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        private void FlipVer_MenuItem_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.GEOMETRYFlipVertical(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        private void RotC180_MenuItem_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.GEOMETRYFlipVertical(((cImage)OpenedImages.SelectedItem).ImagePointer);
            CImageTools.GEOMETRYFlipHorizontal(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        private void RotC90_MenuItem_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.GEOMETRYRotate(((cImage)OpenedImages.SelectedItem).ImagePointer);
            CImageTools.GEOMETRYFlipHorizontal(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        private void RotCC90_MenuItem_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.GEOMETRYRotate(((cImage)OpenedImages.SelectedItem).ImagePointer);
            CImageTools.GEOMETRYFlipVertical(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        private void Negative_MenuItem_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.negative(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }

        private void Grayscale_MenuItem_Click(object sender, RoutedEventArgs e)
        {
            CImageTools.grayscale(((cImage)OpenedImages.SelectedItem).ImagePointer);
        }
    }
}
