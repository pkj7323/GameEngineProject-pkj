using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;

namespace PrimalEditor.Utilities
{
    enum MessageType
    {
        Info = 0x01,
        Warning = 0x02,
        Error = 0x04,
    }

    class LogMessage
    {
        public MessageType Type { get; set; }
        public string Message { get; set; }
        public DateTime Time { get; set; }
        public string File { get; set; }
        public string Caller { get; set; }
        public int Line { get; set; }
        public string MetaData => $"{File}: {Caller} ({Line})";

        public LogMessage(MessageType type, string message, string file, string caller, int line)
        {
            Time = DateTime.Now;
            Type = type;
            Message = message;
            File = Path.GetFileName(file);
            Caller = caller;
            Line = line;
        }
    }
    static class Logger
    {
        private static int _messageFilter =(int)(MessageType.Info | MessageType.Warning | MessageType.Error);   
        private readonly static ObservableCollection<LogMessage> _logMessages = new ObservableCollection<LogMessage>();
        public static ReadOnlyObservableCollection<LogMessage> LogMessages 
        { get; } = new ReadOnlyObservableCollection<LogMessage>(_logMessages);
        public static CollectionViewSource FilteredMessage 
        { get; } = new CollectionViewSource() { Source = LogMessages };

        public static async void Log(MessageType type, string msg,
            [CallerFilePath] string file = "", [CallerMemberName] string caller = "",
            [CallerLineNumber] int line = 0)
        {
            await Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            {
                _logMessages.Add(new LogMessage(type, msg, file, caller, line));
            }));
        }

        public static async Task Clear()
        {
            await Application.Current.Dispatcher.BeginInvoke(new Action(() =>
            {
                _logMessages.Clear();
            }));
        }
        public static void SetMessageFilter(int mask)
        {
            _messageFilter = mask;
            FilteredMessage.View.Refresh();
        }
        static Logger()
        {
            FilteredMessage.Filter += (sender, e) =>
            {
                //_messageFilter == 0101
                var type =(int)(e.Item as LogMessage).Type;
                e.Accepted = (type & _messageFilter) != 0;
            };
        }
    }
}
