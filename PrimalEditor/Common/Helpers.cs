﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace PrimalEditor
{
    public static class VisualExtension {
        public static T FindVisualParent<T>(this DependencyObject dependencyObject) where T : DependencyObject {
            if (!(dependencyObject is Visual)) return null;

            DependencyObject parent = VisualTreeHelper.GetParent(dependencyObject);
            while (parent != null) 
            {
                if (parent is T type) {
                    return type;
                }
                parent = VisualTreeHelper.GetParent(parent);
            }
            return null;
        }
    }

    public static class ContentHelper {
        public static string GetRandomString(int length = 8) {
            if (length <= 0) length = 8;
            var n = length / 11;
            var sb = new StringBuilder();
            for (int i = 0; i <= n; ++i) {
                sb.Append(Path.GetRandomFileName().Replace(".", ""));
            }

            return sb.ToString(0, length);
        }
    }
}
