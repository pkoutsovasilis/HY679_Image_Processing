using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ImageProcGUI.Controls
{

    /// <summary>
    /// Interaction logic for UserControl1.xaml
    /// </summary>
    public unsafe partial class cImage : UserControl, IDisposable
    {
        CImageTools.CreateImage cImg;
        CImageTools.LockImage loImg;
        CImageTools.UnlockImage unImg;
        CImageTools.ChangeImageProperty chgProp;

        WriteableBitmap mBitmap;

        public cImage()
        {
            InitializeComponent();
            if (!DesignerProperties.GetIsInDesignMode(this))
            {
                cImg = createImage;
                loImg = lockImage;
                unImg = unlockImage;
                chgProp = changeImageProperty;
                this.ImagePointer = CImageTools.InitialiseImage(this.ImageType, cImg, loImg, unImg, chgProp);
            }
        }

        public Boolean Load(string path)
        {

            int i = CImageTools.LoadImage(this.ImagePointer, path);
            if (i > 0)
            {
                return true;
            }

            return false;
        }

        private Byte* createImage(UInt32 width, UInt32 height, UInt32* Stride)
        {
            mBitmap = new WriteableBitmap((int)width, (int)height, 96, 96, PixelFormats.Rgb24, null);
            displayImage.Source = mBitmap;
            ImageSource = mBitmap;
            *Stride = (UInt32)mBitmap.BackBufferStride;
            return (Byte*)mBitmap.BackBuffer;
        }

        private void lockImage()
        {
            mBitmap.Lock();
        }

        private void unlockImage(UInt32 width, UInt32 height)
        {
            mBitmap.AddDirtyRect(new Int32Rect(0, 0, (int)width, (int)height));
            mBitmap.Unlock();
        }

        private void changeImageProperty(string propertyName, void* propertyValue)
        {
            PropertyInfo propertyInfo = this.GetType().GetProperty(propertyName);
            if (propertyInfo.PropertyType == typeof(UInt32))
            {
                UInt32* temp = (UInt32*)propertyValue;
                propertyInfo.SetValue(this, *temp, null);
            }
            else if (propertyInfo.PropertyType == typeof(string))
            {
                string temp = Marshal.PtrToStringAnsi((IntPtr)propertyValue);
                propertyInfo.SetValue(this, temp, null);
            }
            else if (propertyInfo.PropertyType == typeof(bool))
            {
                bool* temp = (bool*)propertyValue;
                propertyInfo.SetValue(this, *temp, null);
            }
        }

        public readonly static DependencyProperty strPointer = DependencyProperty.Register("ImagePointer", typeof(IntPtr), typeof(cImage), new PropertyMetadata(IntPtr.Zero));

        public IntPtr ImagePointer
        {
            get { return (IntPtr)GetValue(strPointer); }
            protected set { SetValue(strPointer, value); }
        }

        public readonly static DependencyProperty imgUndo = DependencyProperty.Register("ImageUndo", typeof(bool), typeof(cImage), new PropertyMetadata(false));

        public bool ImageUndo
        {
            get { return (bool)GetValue(imgUndo); }
            protected set
            {
                SetValue(imgUndo, value);
                SetValue(imgRedo, !value);
            }
        }

        public readonly static DependencyProperty imgRedo = DependencyProperty.Register("ImageRedo", typeof(bool), typeof(cImage), new PropertyMetadata(false));

        public bool ImageRedo
        {
            get { return !((bool)GetValue(imgUndo)); }
            protected set { SetValue(imgRedo, value); }
        }

        public readonly static DependencyProperty imgWidth = DependencyProperty.Register("ImageWidth", typeof(UInt32), typeof(cImage), new PropertyMetadata((UInt32)0));

        public UInt32 ImageWidth
        {
            get { return (UInt32)GetValue(imgWidth); }
            protected set { SetValue(imgWidth, value); }
        }

        public readonly static DependencyProperty imgHeight = DependencyProperty.Register("ImageHeight", typeof(UInt32), typeof(cImage), new PropertyMetadata((UInt32)0));

        public UInt32 ImageHeight
        {
            get { return (UInt32)GetValue(imgHeight); }
            protected set { SetValue(imgHeight, value); }
        }

        public readonly static DependencyProperty imgEncoding = DependencyProperty.Register("ImageEncoding", typeof(string), typeof(cImage), new PropertyMetadata("-"));

        public string ImageEncoding
        {
            get { return (string)GetValue(imgEncoding); }
            protected set { SetValue(imgEncoding, value); }
        }

        public static DependencyProperty imgType = DependencyProperty.Register("ImageType", typeof(CImageTools.IMAGE_TYPE), typeof(cImage), new PropertyMetadata(CImageTools.IMAGE_TYPE.TYPE_VISUAL));

        public CImageTools.IMAGE_TYPE ImageType
        {
            get { return (CImageTools.IMAGE_TYPE)GetValue(imgType); }
            set { SetValue(imgType, value); }
        }

        public readonly static DependencyProperty imgSource = DependencyProperty.Register("ImageSource", typeof(ImageSource), typeof(cImage), new PropertyMetadata(null));

        public ImageSource ImageSource
        {
            get { return (ImageSource)GetValue(imgSource); }
            set { SetValue(imgSource, value); }
        }

        public readonly static DependencyProperty imgPath = DependencyProperty.Register("ImagePath", typeof(string), typeof(cImage), new PropertyMetadata(""));

        public string ImagePath
        {
            get { return (string)GetValue(imgPath); }
            set { SetValue(imgPath, value); }
        }

        public void Dispose()
        {
            CImageTools.IMAGEFree(this.ImagePointer);
            this.displayImage.Source = null;
        }

        
    }
}
