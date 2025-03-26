﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace PrimalEditor.Utilities 
{
    /// <summary>
    /// RenderSurfaceView.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class RenderSurfaceView : UserControl ,IDisposable
    {
        private enum Win32Msg {
            WM_SIZING = 0x0214,
            WM_ENTERSIZEMOVE = 0x0231,
            WM_EXITSIZEMOVE = 0x0232,
            WM_SIZE =0x0005,
        }
        private RenderSurfaceHost _renderSurfaceHost = null;

        public RenderSurfaceView()
        {
            InitializeComponent();
            Loaded += RenderSurfaceView_Loaded;
        }
        private void RenderSurfaceView_Loaded(object sender, RoutedEventArgs e) {
            Loaded -= RenderSurfaceView_Loaded;

            _renderSurfaceHost = new RenderSurfaceHost(ActualWidth, ActualHeight);
            _renderSurfaceHost.MessageHook += new HwndSourceHook(HostMsgFilter);
            Content = _renderSurfaceHost;

            
        }

        

        private IntPtr HostMsgFilter(nint hwnd, int msg, nint wParam, nint lParam, ref bool handled) {
            switch ((Win32Msg)msg) {
                case Win32Msg.WM_SIZING:
                    throw new Exception();
                    break;
                case Win32Msg.WM_ENTERSIZEMOVE:
                    throw new Exception();
                    break;
                case Win32Msg.WM_EXITSIZEMOVE:
                    throw new Exception();
                    break;
                case Win32Msg.WM_SIZE:
                    
                    
                    break;
                default:
                    break;
            }
            return IntPtr.Zero;
        }

        #region IDisposable support
        private bool _disposedValue;
        protected virtual void Dispose(bool disposing) {
            if (!_disposedValue) {
                if (disposing) {
                    _renderSurfaceHost.Dispose();
                }
                _disposedValue = true;
            }
        }
        public void Dispose() {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
        #endregion
        
    }
}
