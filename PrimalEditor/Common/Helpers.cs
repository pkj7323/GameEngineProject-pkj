﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace PrimalEditor
{
    public static class  VisualExtension {
        public static T FindVisualParent<T>(this DependencyObject dependencyObject) where T : DependencyObject {
            if (!(dependencyObject is Visual)) return null;

            DependencyObject parent = VisualTreeHelper.GetParent(dependencyObject);
            while (parent != null) {
                if (parent is T type) {
                    return type;
                }
                parent = VisualTreeHelper.GetParent(parent);
            }
            return null;
        }
    }
}
