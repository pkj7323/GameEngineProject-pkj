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

		public Byte[] Icon { get; set; }
		public Byte[] ScreenShot { get; set; }

		public string IconFilePath { get; set; }
		public string ScreenShotFilePath { get; set; }

		public string ProjectFilePath { get; set; }
	}
	class NewProject : ViewModelBase
	{
		//TODO: 설치된 위치 경로를 가져와야함
		// 현재 작업 디렉토리에서 네 번 상위 디렉토리로 이동한 후 PrimalEditor\ProjectTemplates 경로를 설정
		private readonly string _templatePath = Path.Combine(
			Directory.GetParent(Directory.GetParent(Directory.GetParent(Directory.GetParent(Directory.GetCurrentDirectory()).FullName).FullName).FullName).FullName,
			"PrimalEditor", "ProjectTemplates");
		private string _projectName = "NewProject";
		public string ProjectName
		{
			get => _projectName;
			set
			{
				if (_projectName != value)
				{
					_projectName = value;
					ValidateProjectPath();
                    OnPropertyChanged(nameof(ProjectName));
				}
			}
		}

		private string _projectPath = $@"{Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)}\PrimalProjects\";
		public string ProjectPath
		{
			get => _projectPath;
			set
			{
				if (_projectPath != value)
				{
					_projectPath = value;
                    ValidateProjectPath();
                    OnPropertyChanged(nameof(Path));
				}
			}
		}
		private bool _isValid;
		public bool IsValid
		{
			get => _isValid;
			set
			{
				if (_isValid != value)
				{
					_isValid = value;
					OnPropertyChanged(nameof(IsValid));
				}
			}
		}
		private string _errorMessage;
		public string ErrorMessage
		{
			get => _errorMessage;
			set
			{
				if (_errorMessage != value)
				{
					_errorMessage = value;
					OnPropertyChanged(nameof(ErrorMessage));
				}
			}
		}

		private ObservableCollection<ProjectTemplate> _projectTemplates = new ObservableCollection<ProjectTemplate>();
		public ReadOnlyObservableCollection<ProjectTemplate> ProjectTemplates { get; }

		private bool ValidateProjectPath()
		{
			var path = ProjectPath;
			if (!Path.EndsInDirectorySeparator(path))
			{
				path += @"\";
			}
			path += $@"{ProjectName}\";

			IsValid = false;
			if (string.IsNullOrWhiteSpace(ProjectName.Trim()))
			{
				ErrorMessage = "프로젝트 이름을 입력하세요";
			}
			else if (ProjectName.IndexOfAny(Path.GetInvalidFileNameChars()) != -1)
			{
				ErrorMessage = "프로젝트 이름에 허용되지 않는 문자가 포함되어 있습니다.";
			}
			else if (string.IsNullOrWhiteSpace(ProjectPath.Trim()))
			{
				ErrorMessage = "프로젝트 경로를 입력하세요";
			}
			else if (ProjectPath.IndexOfAny(Path.GetInvalidPathChars()) != -1)
			{
				ErrorMessage = "프로젝트 경로에 허용되지 않는 문자가 포함되어 있습니다.";
			}
			else if (Directory.Exists(path) && Directory.EnumerateFileSystemEntries(path).Any())
			{
				ErrorMessage = "선택함 프로젝트 폴더가 이미 존재하거나 비어있지 않습니다";
			}
			else
			{
				ErrorMessage = string.Empty;
				IsValid = true;
			}
			return IsValid;
		}

		public string CreateProject(ProjectTemplate template)
		{
			ValidateProjectPath();
            if (!IsValid)
			{
				return string.Empty;
            }
            if (!Path.EndsInDirectorySeparator(ProjectPath))
            {
                ProjectPath += @"\";
            }
            var path = $@"{ProjectPath}{ProjectName}\";

			try
			{
				if(!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }
                foreach (var folder in template.Folders)
				{
					Directory.CreateDirectory(Path.GetFullPath(Path.Combine(Path.GetDirectoryName(path), folder)));
                }
				var dirInfo = new DirectoryInfo(path+@".Primal\");
				dirInfo.Attributes |= FileAttributes.Hidden;
				File.Copy(template.IconFilePath, Path.GetFullPath(Path.Combine(dirInfo.FullName,"Icon.png")));
				File.Copy(template.ScreenShotFilePath, Path.GetFullPath(Path.Combine(dirInfo.FullName,"ScreenShot.png")));

                //.primal으로 저장된 파일을 사용할거임
                //project는 씬을 리스트로 저장, 씬은 게임 Entity를 리스트로 저장, Entity는 컴포넌트를 리스트로 저장

				var projectXml = File.ReadAllText(template.ProjectFilePath);
				projectXml = string.Format(projectXml, ProjectName, ProjectPath);
				var projectPath = Path.GetFullPath(Path.Combine(path, $"{ProjectName}{Project.Extension}"));
                File.WriteAllText(projectPath, projectXml);
                return path;
            }
            catch (Exception e)
			{
				Debug.WriteLine(e.Message);
				Logger.Log(MessageType.Error, $"{ProjectName} 만들기 실패");
				throw;
            }
        }

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
					template.IconFilePath = System.IO.Path.GetFullPath(Path.Combine(Path.GetDirectoryName(file), "Icon.png"));
					template.Icon = File.ReadAllBytes(template.IconFilePath);
					template.ScreenShotFilePath = System.IO.Path.GetFullPath(Path.Combine(Path.GetDirectoryName(file), "ScreenShot.png"));
					template.ScreenShot = File.ReadAllBytes(template.ScreenShotFilePath);
					template.ProjectFilePath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(file), template.ProjectFile));



					_projectTemplates.Add(template);
				}
				ValidateProjectPath();
			}
			catch (Exception e)
			{
				Debug.WriteLine(e.Message);
                Logger.Log(MessageType.Error, $"프로젝트 템플릿 읽기 오류");
                throw;
            }
		}
	}
}
