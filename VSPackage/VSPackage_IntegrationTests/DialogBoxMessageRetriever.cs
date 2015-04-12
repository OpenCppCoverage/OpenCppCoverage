using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.Runtime.InteropServices;
using System.Text;

namespace VSPackage_IntegrationTests
{
    class DialogBoxMessageRetriever: IDisposable
    {
        //---------------------------------------------------------------------
        public DialogBoxMessageRetriever(IVsUIShell uiShell, TimeSpan timeout)
        {
            this.uiShell = uiShell;
            this.thread = new System.Threading.Thread(o =>
            {
                TestHelpers.Wait(timeout, "Cannot get expected messagebox", () =>
                {
                    this.messageFound = GetMessageBoxText();
                    return this.messageFound != null;
                });
            });

            this.thread.Start();            
        }

        //---------------------------------------------------------------------
        public void Dispose()
        {
            this.thread.Join();
        }

        //---------------------------------------------------------------------
        public string GetMessage()
        {
            this.thread.Join();
            return this.messageFound;
        }

        //---------------------------------------------------------------------
        string GetMessageBoxText()
        {
            IntPtr messageBoxHwnd = GetMessageBoxHwnd();

            if (messageBoxHwnd != IntPtr.Zero)
            {
                var message = GetMessageBoxMessage(messageBoxHwnd);
                const int IDOK = 1;
                Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(EndDialog(messageBoxHwnd, IDOK));

                if (message != null)
                    return message.Trim();
            }

            return null;
        }

        //---------------------------------------------------------------------
        IntPtr GetMessageBoxHwnd()
        {
            IntPtr hwnd;

            Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(uiShell.GetDialogOwnerHwnd(out hwnd));
            if (hwnd != IntPtr.Zero)
            {
                var windowClassName = GetClassName(hwnd);

                // The #32770 is the class name of a messagebox dialog.
                if (windowClassName.ToString().Contains("#32770"))
                    return hwnd;
            }

            return IntPtr.Zero;
        }

        //---------------------------------------------------------------------
        static string GetMessageBoxMessage(IntPtr hwnd)
        {
            IntPtr unmanagedMemoryLocation = IntPtr.Zero;

            try
            {
                unmanagedMemoryLocation = Marshal.AllocHGlobal(10 * 1024);
                Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(
                    EnumChildWindows(hwnd,
                        new EnumChildWindowsCallBack(FindMessageBoxString), unmanagedMemoryLocation));

                return Marshal.PtrToStringUni(unmanagedMemoryLocation);
            }
            finally
            {
                if (unmanagedMemoryLocation != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(unmanagedMemoryLocation);
                }
            }
        }

        //---------------------------------------------------------------------
        static string GetClassName(IntPtr hwnd)
        {
            var windowClassName = new StringBuilder(1024);
            Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(
                GetClassName(hwnd, windowClassName, windowClassName.Capacity));

            return windowClassName.ToString();
        }

        //---------------------------------------------------------------------
        public static bool FindMessageBoxString(IntPtr hwnd, IntPtr unmanagedMemoryLocation)
        {
            var windowClassName = GetClassName(hwnd);

            if (windowClassName.ToLower().Contains("static"))
            {
                StringBuilder windowText = new StringBuilder(2048);
                GetWindowText(hwnd, windowText, windowText.Capacity);

                if (windowText.Length > 0)
                {
                    IntPtr stringAsPtr = IntPtr.Zero;
                    try
                    {
                        stringAsPtr = Marshal.StringToHGlobalAnsi(windowText.ToString());
                        char[] stringAsArray = windowText.ToString().ToCharArray();

                        // Since unicode characters are copied check if we are out of the allocated length.
                        // If not add the end terminating zero.
                        if ((2 * stringAsArray.Length) + 1 < 2048)
                        {
                            Marshal.Copy(stringAsArray, 0, unmanagedMemoryLocation, stringAsArray.Length);
                            Marshal.WriteInt32(unmanagedMemoryLocation, 2 * stringAsArray.Length, 0);
                        }
                    }
                    finally
                    {
                        if (stringAsPtr != IntPtr.Zero)
                        {
                            Marshal.FreeHGlobal(stringAsPtr);
                        }
                    }
                    return false;
                }
            }

            return true;
        }                

        [DllImport("user32")]
        static extern int EndDialog(IntPtr hDlg, int result);

        [DllImport("user32")]
        static extern int GetClassName(IntPtr hWnd,
                                               StringBuilder className,
                                               int stringLength);

        delegate bool EnumChildWindowsCallBack(IntPtr hwnd, IntPtr lParam);

        [DllImport("user32")]
        static extern int EnumChildWindows(IntPtr hwnd, EnumChildWindowsCallBack x, IntPtr y);

        [DllImport("user32")]
        static extern int GetWindowText(IntPtr hWnd, StringBuilder className, int stringLength);

        volatile string messageFound;
        readonly System.Threading.Thread thread;
        readonly IVsUIShell uiShell;
    }
}
