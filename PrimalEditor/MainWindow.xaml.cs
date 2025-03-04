using PrimalEditor.GameProject;
using System.ComponentModel;
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

namespace PrimalEditor;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
	public MainWindow()
	{
		InitializeComponent();
		Loaded += OnMainWindwLoaded;
		Closing += OnMainWindowClosing;
	}

	

	private void OnMainWindwLoaded(object sender, RoutedEventArgs e)
	{
		Loaded -= OnMainWindwLoaded;
		OpenProjectBrowserDialog();
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