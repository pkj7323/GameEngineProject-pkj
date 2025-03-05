﻿using PrimalEditor.GameProject;
using PrimalEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace PrimalEditor.Components
{
	[DataContract]
	[KnownType(typeof(Transform))]
    public class GameEntity : ViewModelBase
	{
		private bool _isEnalbed = true;
        [DataMember]
        public bool IsEnabled
        {
            get => _isEnalbed;
            set
            {
                if (_isEnalbed != value)
                {
                    _isEnalbed = value;
                    OnPropertyChanged(nameof(IsEnabled));
                }
            }
        }


        private string _name;
		[DataMember]
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

		[DataMember]
		public Scene ParentScene { get; private set; }

		[DataMember(Name = nameof(Components))]
		private readonly ObservableCollection<Component> _components = new ObservableCollection<Component>();
		public ReadOnlyObservableCollection<Component> Components { get; private set; }

		public ICommand RenameCommand { get; set; }
		public ICommand IsEnabledCommand { get; set; }

        [OnDeserialized]
		void OnDeserialized(StreamingContext context)
		{
			if (_components != null)
			{
				Components = new ReadOnlyObservableCollection<Component>(_components);
				OnPropertyChanged(nameof(Components));
            }

			RenameCommand = new RelayCommand<string>(x =>
			{
				var oldName = Name;
                Name = x;
				Project.UndoRedo.Add(new UndoRedoAction($"Rename entity '{oldName}' to '{x}'",
					nameof(Name), this, oldName, x));
            }, x=> x!=_name);
            IsEnabledCommand = new RelayCommand<bool>(x =>
            {
                var oldValue = IsEnabled;
                IsEnabled = x;
                Project.UndoRedo.Add(new UndoRedoAction(x ? $"Enable {Name} " :  $"Disable {Name}",
                    nameof(IsEnabled), this, oldValue, x));
            });

        }
		public GameEntity(Scene scene)
		{
			Debug.Assert(scene != null);
			ParentScene = scene;
			_components.Add(new Transform(this));
			OnDeserialized(new StreamingContext());
        }
	}
}
