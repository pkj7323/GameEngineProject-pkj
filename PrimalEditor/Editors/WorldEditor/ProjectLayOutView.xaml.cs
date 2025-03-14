﻿using PrimalEditor.Components;
using PrimalEditor.GameProject;
using PrimalEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace PrimalEditor.Editors
{
	/// <summary>
	/// ProjectLayOutView.xaml에 대한 상호 작용 논리
	/// </summary>
	public partial class ProjectLayOutView : UserControl
	{
		public ProjectLayOutView()
		{
			InitializeComponent();
		}

		private void OnAddGameEntity_Button_Click(object sender, RoutedEventArgs e)
		{
			var btn = sender as Button;
			var vm = btn.DataContext as Scene;
			vm.AddGameEntityCommand.Execute(new GameEntity(vm) { Name = "Empty Game Entity"});
		}

		private void OnGameEntites_ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{

			var listBox = sender as ListBox;

			var newSelection = listBox.SelectedItems.Cast<GameEntity>().ToList();
			var prevSelection = newSelection.Except(e.AddedItems.Cast<GameEntity>()).Concat(e.RemovedItems.Cast<GameEntity>()).ToList();

            Project.UndoRedo.Add(new UndoRedoAction("Selection Changed",
				() =>  
				{
					listBox.UnselectAll();
					prevSelection.ForEach(x => (listBox.ItemContainerGenerator.ContainerFromItem(x) as ListBoxItem).IsSelected = true);
                },
				() => 
				{
					listBox.UnselectAll();
                    newSelection.ForEach(x => (listBox.ItemContainerGenerator.ContainerFromItem(x) as ListBoxItem).IsSelected = true);
                }
			));

            MSGameEntity msGameEntity = null;
            if (newSelection.Any())
            {
                msGameEntity = new MSGameEntity(newSelection);
            }
            GameEntityView.Instance.DataContext = msGameEntity;
        }
	}
}
