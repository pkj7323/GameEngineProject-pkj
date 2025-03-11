using System;
using System.Collections.Generic;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace PrimalEditor.Utilities
{
    /// <summary>
    /// LoggerView.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class LoggerView : UserControl
    {
        public LoggerView()
        {
            InitializeComponent();
            Loaded += (s, e) =>
            {
                Logger.Log(MessageType.Info, "정보 메세지");
                Logger.Log(MessageType.Warning, "경고 메세지");
                Logger.Log(MessageType.Error, "오류 메세지");
            };
        }

        private void OnClear_Button_Click(object sender, RoutedEventArgs e)
        {
            Logger.Clear();
        }

        private void OnMessageFilter_Button_Click(object sender, RoutedEventArgs e)
        {
            var filter = 0;
            if (toggleInfo.IsChecked == true)
                filter |= (int)MessageType.Info;
            if (toggleWarnings.IsChecked == true)
                filter |= (int)MessageType.Warning;
            if (toggleErrors.IsChecked == true)
                filter |= (int)MessageType.Error;

            Logger.SetMessageFilter(filter);
        }
    }
}
