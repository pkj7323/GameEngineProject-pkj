using PrimalEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Data;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace PrimalEditor.GameProject
{
    [DataContract]
    public class ProjectData
    {
        [DataMember]
        public string ProjectName { get; set; }
        [DataMember]
        public string ProjectPath { get; set; }
        [DataMember]
        public DateTime Date { get; set; }


        public string FullPath { get => $"{ProjectPath}{ProjectName}{Project.Extension}"; }
        public byte[] Icon { get; set; }
        public byte[] ScreenShot { get; set; }

    }
    [DataContract]
    public class ProjectDataList
    {
        [DataMember]
        public List<ProjectData> Projects { get; set; }
    }
    class OpenProject
    {
        private static readonly string _applicationDataPath = 
            $@"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}\PrimalEditor\";
        private static readonly string _projectDataPath;
        private static readonly ObservableCollection<ProjectData> _projects = new ObservableCollection<ProjectData>();
        public static ReadOnlyObservableCollection<ProjectData> Projects { get; }

        private static void ReadProjectData()
        {
            if (File.Exists(_projectDataPath))
            {
                var projects = Serializer.FromFile<ProjectDataList>(_projectDataPath).Projects.OrderByDescending(x => x.Date);
                _projects.Clear();
                foreach (var project in projects)
                {
                    if (File.Exists(project.FullPath))
                    {
                        project.Icon = File.ReadAllBytes($@"{project.ProjectPath}\.Primal\Icon.png");
                        project.ScreenShot = File.ReadAllBytes($@"{project.ProjectPath}\.Primal\ScreenShot.png");
                        _projects.Add(project);
                    }
                }
            }
        }
        private static void WriteProjectData()
        {
            var projects = _projects.OrderBy(x => x.FullPath).ToList();
            Serializer.ToFile(new ProjectDataList { Projects = projects }, _projectDataPath);
        }
        public static Project Open(ProjectData projectData)
        {
            ReadProjectData();
            var project = _projects.FirstOrDefault(x => x.FullPath == projectData.FullPath);
            if (project != null)
            {

            }
            else
            {
                project = projectData;
                project.Date = DateTime.Now;
                _projects.Add(project);
            }
            WriteProjectData();

            return Project.Load(project.FullPath);
        }


        static OpenProject()
        {
            try
            {
                if (!Directory.Exists(_applicationDataPath))
                {
                    Directory.CreateDirectory(_applicationDataPath);
                }
                _projectDataPath = $@"{_applicationDataPath}ProjectData.xml";
                Projects = new ReadOnlyObservableCollection<ProjectData>(_projects);
                ReadProjectData();

            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                Logger.Log(MessageType.Error, $"프로젝트 데이터 읽기 실패");
                throw;
            }
        }
    }
}
