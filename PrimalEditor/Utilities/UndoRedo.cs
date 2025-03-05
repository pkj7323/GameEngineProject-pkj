using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PrimalEditor.Utilities
{
    public interface IUndoRedo
    {
        string Name { get; }
        void Undo();
        void Redo();
    }
    public class UndoRedoAction : IUndoRedo
    {
        public string Name { get; }
        private readonly Action _undoAction;
        private readonly Action _redoAction;
        public UndoRedoAction(string name, Action undo, Action redo)
        {
            Debug.Assert(undo != null && redo != null);
            Name = name;
            _undoAction = undo;
            _redoAction = redo;
        }
        public UndoRedoAction(string name) 
        {
            Name = name;
        }

        public UndoRedoAction(string name ,string property, object instance, object undoValue, object redoValue):
            this(
                name,
                () => instance.GetType().GetProperty(property).SetValue(instance, undoValue),
                () => instance.GetType().GetProperty(property).SetValue(instance, redoValue)
                )
        {
            
        }

        public void Undo() => _undoAction();
        public void Redo() => _redoAction();
    }
    public class UndoRedo
    {
        private readonly ObservableCollection<IUndoRedo> _undoList = new ObservableCollection<IUndoRedo>();
        private readonly ObservableCollection<IUndoRedo> _redoList = new ObservableCollection<IUndoRedo>();
        public ReadOnlyObservableCollection<IUndoRedo> UndoList { get; }
        public ReadOnlyObservableCollection<IUndoRedo> RedoList { get; }
        private bool _enabledAdd = true;
        public UndoRedo()
        {
            UndoList = new ReadOnlyObservableCollection<IUndoRedo>(_undoList);
            RedoList = new ReadOnlyObservableCollection<IUndoRedo>(_redoList);
        }
        public void Reset()
        {
            _undoList.Clear();
            _redoList.Clear();
        }
        public void Add(IUndoRedo cmd)
        {
            if (_enabledAdd)
            {
                _undoList.Add(cmd);
                _redoList.Clear();
            }
        }
        public void Undo()
        {
            if (_undoList.Any())
            {
                var cmd = _undoList.Last();
                _undoList.RemoveAt(_undoList.Count - 1);
                _enabledAdd = false;
                cmd.Undo();
                _enabledAdd = true;
                _redoList.Insert(0, cmd);
            }
        }

        public void Redo()
        {
            if (_redoList.Any())
            {
                var cmd = _redoList.First();
                _redoList.RemoveAt(0);
                _enabledAdd = false;
                cmd.Redo();
                _enabledAdd = true;
                _undoList.Add(cmd);
            }
        }
    }
}
