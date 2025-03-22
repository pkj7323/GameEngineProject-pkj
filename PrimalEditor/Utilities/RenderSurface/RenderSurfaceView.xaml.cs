using System;
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
        private bool _canResize = true;
        private bool _moved = false;

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

            var window = this.FindVisualParent<Window>();
            Debug.Assert(window != null);

            var helper = new WindowInteropHelper(window);
            if (helper.Handle != null) {
                HwndSource.FromHwnd(helper.Handle)?.AddHook(HwndMessageHook);
            }
        }

        private nint HwndMessageHook(nint hwnd, int msg, nint wParam, nint lParam, ref bool handled) {
            switch ((Win32Msg)msg) {
                case Win32Msg.WM_SIZING:
                    _canResize = false;
                    _moved = false;
                    break;
                case Win32Msg.WM_ENTERSIZEMOVE:
                    _moved = true;
                    break;
                case Win32Msg.WM_EXITSIZEMOVE:
                    _canResize = true;
                    if (!_moved) _renderSurfaceHost.Resize();
                    break;
                
                default:
                    break;
            }
            return IntPtr.Zero;
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
                    if (_canResize) _renderSurfaceHost.Resize();
                    
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
