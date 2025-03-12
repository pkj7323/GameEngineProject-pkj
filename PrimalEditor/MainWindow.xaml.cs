using PrimalEditor.GameProject;
using System.ComponentModel;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;


namespace PrimalEditor;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
	//일단 엔진 개발중이니까 어디에 설치 할지 모르는 상태임 일단 자기 디렉토리로 설정할것
	public static string PrimalPath { get; private set; } = @"C:\Users\MSI\source\repos\pkj7323\GameEngineProject-pkj";

    public MainWindow()
	{
		InitializeComponent();
		Loaded += OnMainWindwLoaded;
		Closing += OnMainWindowClosing;
	}

	

	private void OnMainWindwLoaded(object sender, RoutedEventArgs e)
	{
		Loaded -= OnMainWindwLoaded;
        GetEnginePath();
		OpenProjectBrowserDialog();
	}

    private void GetEnginePath()
    {
		var primalPath = Environment.GetEnvironmentVariable("PRIMAL_ENGINE", EnvironmentVariableTarget.User);
		if (primalPath == null || !Directory.Exists(Path.Combine(primalPath, @"Engine\EngineAPI")))
		{
			var dlg = new EnginePathDialog();
            if (dlg.ShowDialog() == true)
            {
                primalPath = dlg.PrimalPath;
                Environment.SetEnvironmentVariable("PRIMAL_ENGINE", primalPath.ToUpper(), EnvironmentVariableTarget.User);
            }
			else
			{
				Application.Current.Shutdown();
            }
        }
		else
		{
			PrimalPath = primalPath;
        }
    }

    private void OnMainWindowClosing(object? sender, CancelEventArgs e)
	{
		Closing -= OnMainWindowClosing;
		Project.Current?.Unload();
	}
	private void OpenProjectBrowserDialog()
	{
		var projectBrowser = new ProjectBrowserDialog
		{
			Owner = this,
		};
		if (projectBrowser.ShowDialog() == false || projectBrowser.DataContext == null)
		{
			Application.Current.Shutdown();
		}
		else
		{
			Project.Current?.Unload();
			DataContext = projectBrowser.DataContext;
		}
	}
}