﻿using PrimalEditor.Components;
using PrimalEditor.DllWrappers;
using PrimalEditor.GameProject;
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
	class GameEntity : ViewModelBase
	{
		private int _entityId = ID.INVALID_ID;
		public int EntityId
		{
			get => _entityId;
			set
			{
				if (_entityId != value)
				{
					_entityId = value;
					OnPropertyChanged(nameof(EntityId));
				}
			}
		}

		private bool _isActive;
		public bool IsActive
		{
			get => _isActive;
			set
			{
				if (_isActive != value)
				{
					_isActive = value;
					if (_isActive)
					{
						EntityId = EngineAPI.EntityAPI.CreateGameEntity(this);
						Debug.Assert(ID.IsValid(_entityId));
					}
					else
					{
						EntityId = EngineAPI.EntityAPI.RemoveGameEntity(this);
                    }
					OnPropertyChanged(nameof(IsActive));
				}
			}
		}


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

		public Component GetComponent(Type type) => Components.FirstOrDefault(c => c.GetType() == type);
		public T GetComponent<T>() where T : Component => GetComponent(typeof(T)) as T;

        [OnDeserialized]
		void OnDeserialized(StreamingContext context)
		{
			if (_components != null)
			{
				Components = new ReadOnlyObservableCollection<Component>(_components);
				OnPropertyChanged(nameof(Components));
			}

		}
		public GameEntity(Scene scene)
		{
			Debug.Assert(scene != null);
			ParentScene = scene;
			_components.Add(new Transform(this));
			OnDeserialized(new StreamingContext());
		}
	}
	abstract class MSEntity : ViewModelBase
	{
		// Enable updates to a selected entites
		private bool _enableUpdates = true;
		private bool? _isEnalbed;
		public bool? IsEnabled
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
		private readonly ObservableCollection<IMSComponent> _components = new ObservableCollection<IMSComponent>();
		public ReadOnlyObservableCollection<IMSComponent> Components { get; private set; }
        public T GetMSComponent<T>() where T : IMSComponent
        {
            return (T)Components.FirstOrDefault(x => x.GetType() == typeof(T));
        }

        public List<GameEntity> SelectedEntities { get; }
        private void MakeComponentList()
        {
            _components.Clear();
			var firstEntity = SelectedEntities.FirstOrDefault();
			if (firstEntity == null) return;
            foreach (var component in firstEntity.Components)
			{
				var type = component.GetType();
                if (!SelectedEntities.Skip(1).Any(entity => entity.GetComponent(type) == null))
				{
					Debug.Assert(Components.FirstOrDefault(x => x.GetType() == type) == null);
					_components.Add(component.GetMultiSelectComponent(this));
                }
            }
        }

        protected virtual bool UpdateGameEntites(string propertyName)
		{
			switch (propertyName)
			{
				case nameof(IsEnabled): SelectedEntities.ForEach(x => x.IsEnabled = IsEnabled.Value); return true;
				case nameof(Name): SelectedEntities.ForEach(x => x.Name = Name); return true;
			}
			return false;
		}
		public void Refresh()
		{
			_enableUpdates = false;
			UpdateMSGameEntity();
            MakeComponentList();
			_enableUpdates = true;
		}

        

        protected virtual bool UpdateMSGameEntity()
		{
			IsEnabled = GetMixedValue(SelectedEntities, new Func<GameEntity, bool>(x => x.IsEnabled));
			Name = GetMixedValue(SelectedEntities, new Func<GameEntity, string>(x => x.Name));
			return true;
		}

		public static float? GetMixedValue<T>(List<T> objects, Func<T, float> getProperty)
		{
			var value = getProperty(objects.First());
			return objects.Skip(1).Any(x => !getProperty(x).IsTheSameAs(value)) ? (float?)null : value;
            
		}

		private static bool? GetMixedValue<T>(List<T> objects, Func<T, bool> getProperty)
		{
			var value = getProperty(objects.First());
            return objects.Skip(1).Any(x => value != getProperty(x)) ? (bool?)null : value;
        }

		private static string GetMixedValue<T>(List<T> objects, Func<T, string> getProperty)
		{
			var value = getProperty(objects.First());
            return objects.Skip(1).Any(x => value != getProperty(x)) ? null : value;
        }

		public MSEntity(List<GameEntity> entities)
		{
			Debug.Assert(entities?.Any() == true);
			Components = new ReadOnlyObservableCollection<IMSComponent>(_components);
			SelectedEntities = entities;
			PropertyChanged += (s, e) => { if (_enableUpdates) UpdateGameEntites(e.PropertyName); };
		}
	}

	class MSGameEntity : MSEntity
	{
		public MSGameEntity(List<GameEntity> entities) : base(entities)
		{
			Refresh();
		}
	}
}