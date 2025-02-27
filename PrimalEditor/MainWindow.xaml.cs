using PrimalEditor.GameProject1;
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
		Loaded += OnMainWindwoLoaded;
	}
	private void OnMainWindwoLoaded(object sender, RoutedEventArgs e)
	{
		Loaded -= OnMainWindwoLoaded;
		OpenProjectBrowserDialog();
	}

	private void OpenProjectBrowserDialog()
	{
	   var projectBrowser = new ProjectBrowserDialog();
	   if(projectBrowser.ShowDialog() == false)
		{
			Application.Current.Shutdown();
        }
		else
		{
            // Load the project
        }
    }
}