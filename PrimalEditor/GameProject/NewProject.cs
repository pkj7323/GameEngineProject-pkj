using PrimalEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace PrimalEditor.GameProject
{
	[DataContract]
	public class ProjectTemplate
	{
		[DataMember]
		public string ProjectType { get; set; }
		[DataMember]
		public string ProjectFile { get; set; }
		[DataMember]
		public List<string> Folders { get; set; }
	}
	class NewProject : ViewModelBase
	{
		//TODO: 설치된 위치 경로를 가져와야함
		private readonly string _templatePath = @"C:\Users\PARK\source\repos\Primal\PrimalEditor\ProjectTempletes";
		private string _name = "NewProject";
		public string Name
		{
			get => _name;
			set
			{
				if (_name != value)
				{
					_name = value;
					OnPropertyChanged(nameof(Name));
				}
			}
		}

		private string _path = $@"{Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)}\PrimalProject\";
		public string Path
		{
			get => _path;
			set
			{
				if (_path != value)
				{
					_path = value;
					OnPropertyChanged(nameof(Path));
				}
			}
		}
		private ObservableCollection<ProjectTemplate> _projectTemplates = new ObservableCollection<ProjectTemplate>();
		public ReadOnlyObservableCollection<ProjectTemplate> ProjectTemplates { get; }



		public NewProject()
		{
			ProjectTemplates = new ReadOnlyObservableCollection<ProjectTemplate>(_projectTemplates);
            try
			{
				var templatesFiles = Directory.GetFiles(_templatePath, "template.xml",SearchOption.AllDirectories);
				Debug.Assert(templatesFiles.Any());
				foreach (var file in templatesFiles)
				{
					var template = Serializer.FromFile<ProjectTemplate>(file);
                    _projectTemplates.Add(template);
                }
			}
			catch (Exception e)
			{
				Debug.WriteLine(e.Message);
				// TODO: log error
			}
		}
	}
}
