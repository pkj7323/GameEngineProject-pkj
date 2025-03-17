﻿using PrimalEditor.Components;
using PrimalEditor.GameProject;
using PrimalEditor.Utilities;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Controls.Primitives;
using System.Globalization;

namespace PrimalEditor.Editors
{
    public class NullableBoolToBoolConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value is bool b && b == true;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value is bool b && b == true;
        }
    }
    /// <summary>
    /// GameEntityView.xaml에 대한 상호 작용 논리
    /// </summary>
    /// 

    public partial class GameEntityView : UserControl
    {
        private Action _undoAction;
        private string _propertyName;

        public static GameEntityView Instance { get; private set; }
        public GameEntityView()
        {
            InitializeComponent();
            DataContext = null;
            Instance = this;
            DataContextChanged += (_, __) =>
            {
                if (DataContext != null)
                {
                    (DataContext as MSEntity).PropertyChanged += (s, e) => _propertyName = e.PropertyName;
                }
            };
        }
        private Action GetRenameAction()
        {
            var vm = DataContext as MSEntity;
            var selection = vm.SelectedEntities.Select(entity => (entity, entity.Name)).ToList();
            return new Action(() =>
            {
                selection.ForEach(item => item.entity.Name = item.Name);
                (DataContext as MSEntity).Refresh();
            });
        }
        private Action GetIsEnabledAction()
        {
            var vm = DataContext as MSEntity;
            var selection = vm.SelectedEntities.Select(entity => (entity, entity.IsEnabled)).ToList();
            return new Action(() =>
            {
                selection.ForEach(item => item.entity.IsEnabled = item.IsEnabled);
                (DataContext as MSEntity).Refresh();
            });
        }
        private void OnName_TextBox_GotKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            _propertyName = string.Empty;
            _undoAction = GetRenameAction();
        }

        private void OnName_TextBox_LostKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            if (_propertyName == nameof(MSEntity.Name) && _undoAction != null)
            {
                var redoAction = GetRenameAction();
                Project.UndoRedo.Add(new UndoRedoAction("Name Changed", _undoAction, redoAction));
                _propertyName = null;
            }
            _undoAction = null;
        }

        private void OnIsEnabled_CheckBox_Click(object sender, RoutedEventArgs e)
        {
            var undoAction = GetIsEnabledAction();
            var vm = DataContext as MSEntity;
            vm.IsEnabled = (sender as CheckBox).IsChecked == true;
            var redoAction = GetIsEnabledAction();
            Project.UndoRedo.Add(new UndoRedoAction(
                vm.IsEnabled == true ? "Enable Game Entity" : "Disable Game Entity", undoAction, redoAction));
        }
        private void OnAddComponent_Button_PreviewMouse_LBD(object sender, MouseButtonEventArgs e)
        {
            var menu = FindResource("addComponentMenu") as ContextMenu;
            var btn = sender as ToggleButton;
            btn.IsChecked = true;
            menu.Placement = PlacementMode.Bottom;
            menu.PlacementTarget = btn;
            menu.MinWidth = btn.ActualWidth;
            menu.IsOpen = true;
        }

        private void AddComponent(ComponentType componentType, object data)
        {
            var creationFunction = ComponentFactory.GetCreationFunction(componentType);
            var chandedEntities = new List<(GameEntity entity, Component component)>();
            var vm = DataContext as MSEntity;
            foreach (var entity in vm.SelectedEntities)
            {
                var component = creationFunction(entity, data);
                if (entity.AddComponent(component))
                {
                    chandedEntities.Add((entity, component));
                }
            }

            if (chandedEntities.Any())
            {
                vm.Refresh();

                Project.UndoRedo.Add(new UndoRedoAction($"Add {componentType} component",
                () =>
                {
                    chandedEntities.ForEach(x => x.entity.RemoveComponent(x.component));
                    (DataContext as MSEntity).Refresh();
                },
                () =>
                {
                    chandedEntities.ForEach(x => x.entity.AddComponent(x.component));
                    (DataContext as MSEntity).Refresh();
                }
                ));
            }
        }

        private void OnAddScriptComponent(object sender, RoutedEventArgs e)
        {
            AddComponent(ComponentType.Script, (sender as MenuItem).Header.ToString());
        }
    }
}