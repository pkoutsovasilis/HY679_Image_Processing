using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace ImageProcGUI.Controls
{
    public unsafe abstract class CImageTools
    {
        public enum IMAGE_TYPE
        {
            TYPE_VISUAL,
            TYPE_FOURIER,
            TYPE_HISTOGRAM
        }

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr InitialiseImage(IMAGE_TYPE type, CreateImage fn2, LockImage fn3, UnlockImage fn4, ChangeImageProperty fn5);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int LoadImage(IntPtr cImage, [MarshalAs(UnmanagedType.LPStr)]string path);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void LoadRAWImage(IntPtr cImage, [MarshalAs(UnmanagedType.LPArray)]string[] paths, short channels, short bitpsam, UInt32 width, UInt32 height);
        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SaveImage(IntPtr cImage, [MarshalAs(UnmanagedType.LPStr)]string path, bool openSavedIMG);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void IMAGEFree(IntPtr cImage);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void IMAGEUndo(IntPtr cImage);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void IMAGERedo(IntPtr cImage);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void IMAGERestore(IntPtr cImage);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void IMAGEToYCBCR(IntPtr cImage);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void IMAGEToRGB(IntPtr cImage);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetErrorCallback(ErrorCallback errCB);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 IMAGEGetPixelValue(IntPtr imgPTR, UInt32 x, UInt32 y);

        public delegate void ErrorCallback([MarshalAs(UnmanagedType.LPStr)]string message);
        public delegate Byte* CreateImage(UInt32 Width, UInt32 Height, UInt32* stride);
        public delegate void LockImage();
        public delegate void UnlockImage(UInt32 Width, UInt32 Height);
        public delegate void ChangeImageProperty([MarshalAs(UnmanagedType.LPStr)]string propertyName, void* propertyValue);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr HISTInitialise(IntPtr imgPtr, IntPtr histimgPTR, HistogramAdd histAdd, HistogramClear histClear);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void HISTCalculate(IntPtr histPTR);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void HISTFree(IntPtr histPTR);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void HISTEqualize(IntPtr histPTR);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void HISTReset(IntPtr histPTR, bool RefreshHIST);

        public delegate void HistogramAdd(float average, float standard_dev, UInt64 max_val, [MarshalAs(UnmanagedType.LPStr)]string channel);
        public delegate void HistogramClear();

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void STATSCalculate(IntPtr histPTR, StatisticsAdd staAdd, StatisticsClear staClear);

        public delegate void StatisticsAdd(float average, float standard_dev, float correlation_hor_f, float correlation_hor_s, float correlation_ver_f, float correlation_ver_s, [MarshalAs(UnmanagedType.LPStr)]string channel);
        public delegate void StatisticsClear();

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FILTERSInitialise(FiltersAdd Add, FiltersClear Clear);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FILTERSLoad([MarshalAs(UnmanagedType.LPArray)]string[] paths, int pathsN);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FILTERSFree(IntPtr filtPTR);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FILTERSApply(IntPtr imgPTR, IntPtr filterPTR);

        public delegate void FiltersAdd(IntPtr pointer, [MarshalAs(UnmanagedType.LPStr)]string name);
        public delegate void FiltersClear();

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FOURIERCalculate(IntPtr imgPTR, Byte type);

        public enum Process_Type
        {
            PR_TYPE_SHRINK,
            PR_TYPE_THIN,
            PR_TYPE_SKELET
        }

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void MORPHLoad([MarshalAs(UnmanagedType.LPArray)]string[] paths, int pathsN);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void MORPHProcess(IntPtr imgPTR, int iterations, Process_Type type);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GEOMETRYFlipHorizontal(IntPtr imgPTR);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GEOMETRYFlipVertical(IntPtr imgPTR);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GEOMETRYRotate(IntPtr imgPTR);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void negative(IntPtr cImage);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void grayscale(IntPtr cImage);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SINOGRAMBackprojection(IntPtr cImage);

        public enum Window_type
        {
            TYPE_HANNING,
            TYPE_HAMMING
        }

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SINOGRAMFilteredBackprojection(IntPtr cImage, Window_type type);

        [DllImport("ImageProc.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SINOGRAMFilt(IntPtr cImage, double[] type);
    }
}
