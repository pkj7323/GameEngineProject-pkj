using System;
using System.Collections.Generic;
using System.IO;
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


namespace PrimalEditor
{
    /// <summary>
    /// EnginePathDialog.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class EnginePathDialog : Window
    {
        public string PrimalPath { get; private set; }
        public EnginePathDialog()
        {
            InitializeComponent();
            Owner = Application.Current.MainWindow;
        }

        private void OnOk_Button_Click(object sender, RoutedEventArgs e)
        {
            var path = pathTextBox.Text.Trim();
            messageTextBlock.Text = string.Empty;
            if (string.IsNullOrWhiteSpace(path))
            {
                messageTextBlock.Text = "경로를 입력해주세요";
            }
            else if (path.IndexOfAny(Path.GetInvalidPathChars()) != -1)
            {
                messageTextBlock.Text = "올바르지 않은 문자가 경로에 사용됨";
            }
            else if (!Directory.Exists(Path.Combine(path,@"Engine\EngineAPI\")))
            {

                messageTextBlock.Text = "Engine이 없습니다.";
            }
            if (string.IsNullOrWhiteSpace(messageTextBlock.Text))
            {
                if (!Path.EndsInDirectorySeparator(path))
                {
                    path += @"\";
                }
                PrimalPath = path;
                DialogResult = true;
                Close();
            }

        }
    }
}
