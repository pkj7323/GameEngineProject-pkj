﻿using PrimalEditor.Components;
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

namespace PrimalEditor.GameProject
{
	[DataContract]
	class Scene : ViewModelBase
	{

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
		public Project Project { get; private set; }




		public bool _isActive;
		[DataMember]
		public bool IsActive
		{
			get => _isActive;
			set
			{
				if (_isActive != value)
				{
					_isActive = value;
					OnPropertyChanged(nameof(IsActive));
				}
			}
		}

		[DataMember(Name = nameof(GameEntities))]
		private readonly ObservableCollection<GameEntity> _gameEntities = new ObservableCollection<GameEntity>();
		public ReadOnlyObservableCollection<GameEntity> GameEntities { get; private set; }
        public ICommand AddGameEntityCommand { get; set; }
        public ICommand RemoveGameEntityCommand { get; set; }

        private void AddGameEntity(GameEntity gameEntity, int index = -1)
        {
            Debug.Assert(!_gameEntities.Contains(gameEntity));
			gameEntity.IsActive = IsActive;
            if (index == -1)
            {
				_gameEntities.Add(gameEntity);
            }
			else
			{
                _gameEntities.Insert(index, gameEntity);
            }
        }
		private void RemoveGameEntity(GameEntity gameEntity)
        {
            Debug.Assert(_gameEntities.Contains(gameEntity));
			gameEntity.IsActive = false;
            _gameEntities.Remove(gameEntity);
        }


        [OnDeserialized]
		private void OnDeserialized(StreamingContext context)
		{
			//if (_gameEntities == null) _gameEntities = new ObservableCollection<GameEntity>();
            if (_gameEntities != null)
			{
				GameEntities = new ReadOnlyObservableCollection<GameEntity>(_gameEntities);
				OnPropertyChanged(nameof(GameEntities));
			}
            foreach (var entity in _gameEntities)
            {
                entity.IsActive = IsActive;
            }

            AddGameEntityCommand = new RelayCommand<GameEntity>(x =>
			{
				AddGameEntity(x);
				var entityIndex = _gameEntities.Count - 1;

				Project.UndoRedo.Add(new UndoRedoAction($"Add {x.Name} to {Name}",
					() => RemoveGameEntity(x),
					() => AddGameEntity(x, entityIndex)
                    ));
			});

			RemoveGameEntityCommand = new RelayCommand<GameEntity>(x =>
			{
                var entityIndex = _gameEntities.IndexOf(x);
                RemoveGameEntity(x);
				Project.UndoRedo.Add(new UndoRedoAction($"Remove {x.Name}",
					() => AddGameEntity(x, entityIndex),
					() => RemoveGameEntity(x)
					));
			});

			
		}
		public Scene(Project project,string name)
		{
			Debug.Assert(project != null);
			Project = project;
			Name = name;
			OnDeserialized(new StreamingContext());
        }
	}
}
