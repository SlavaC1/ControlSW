using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace BSS.MVVM.View.Behaviors
{
    /// <summary>
    /// Static class used to attach to wpf control
    /// </summary>
    public static class GridViewColumnResize
    {
        #region Dependency Properties

        public static readonly DependencyProperty EnabledProperty = DependencyProperty.RegisterAttached(
            "Enabled",
            typeof(bool),
            typeof(GridViewColumnResize),
            new PropertyMetadata(OnSetEnabledCallback));

        public static readonly DependencyProperty GridViewColumnResizeBehaviorProperty = DependencyProperty.RegisterAttached(
            "GridViewColumnResizeBehavior",
            typeof(GridViewColumnResizeBehavior),
            typeof(GridViewColumnResize),
            null);

        public static readonly DependencyProperty ListViewResizeBehaviorProperty = DependencyProperty.RegisterAttached(
            "ListViewResizeBehaviorProperty",
            typeof(ListViewResizeBehavior),
            typeof(GridViewColumnResize),
            null);

        public static readonly DependencyProperty WidthProperty = DependencyProperty.RegisterAttached(
            "Width",
            typeof(string),
            typeof(GridViewColumnResize),
            new PropertyMetadata(OnSetWidthCallback));

        public static readonly DependencyProperty MarginProperty = DependencyProperty.RegisterAttached(
            "Margin",
            typeof(int),
            typeof(GridViewColumnResize),
            new PropertyMetadata(28, OnSetMarginCallback));

        #endregion Dependency Properties

        #region Public Methods

        public static bool GetEnabled(DependencyObject obj)
        {
            return (bool)obj.GetValue(EnabledProperty);
        }

        public static int GetMargin(DependencyObject obj)
        {
            return (int)obj.GetValue(MarginProperty);
        }

        public static string GetWidth(DependencyObject obj)
        {
            return (string)obj.GetValue(WidthProperty);
        }

        public static void SetEnabled(DependencyObject obj, bool value)
        {
            obj.SetValue(EnabledProperty, value);
        }

        public static void SetMargin(DependencyObject obj, int value)
        {
            obj.SetValue(MarginProperty, value);
        }

        public static void SetWidth(DependencyObject obj, string value)
        {
            obj.SetValue(WidthProperty, value);
        }

        #endregion Public Methods

        #region CallBack

        private static ListViewResizeBehavior GetOrCreateBehavior(ListView element)
        {
            var behavior = element.GetValue(GridViewColumnResizeBehaviorProperty) as ListViewResizeBehavior;
            if (behavior == null)
            {
                behavior = new ListViewResizeBehavior(element);
                element.SetValue(ListViewResizeBehaviorProperty, behavior);
            }

            return behavior;
        }

        private static GridViewColumnResizeBehavior GetOrCreateBehavior(GridViewColumn element)
        {
            var behavior = element.GetValue(GridViewColumnResizeBehaviorProperty) as GridViewColumnResizeBehavior;
            if (behavior == null)
            {
                behavior = new GridViewColumnResizeBehavior(element);
                element.SetValue(GridViewColumnResizeBehaviorProperty, behavior);
            }

            return behavior;
        }

        private static void OnSetEnabledCallback(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
        {
            var element = dependencyObject as ListView;
            if (element != null)
            {
                ListViewResizeBehavior behavior = GetOrCreateBehavior(element);
                behavior.Enabled = (bool)e.NewValue;
            }
        }

        private static void OnSetMarginCallback(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
        {
            var element = dependencyObject as ListView;
            if (element != null)
            {
                ListViewResizeBehavior behavior = GetOrCreateBehavior(element);
                behavior.Margin = (int)e.NewValue;
            }
        }

        private static void OnSetWidthCallback(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
        {
            var element = dependencyObject as GridViewColumn;
            if (element != null)
            {
                GridViewColumnResizeBehavior behavior = GetOrCreateBehavior(element);
                behavior.Width = e.NewValue as string;
            }
        }

        #endregion CallBack

        #region Nested type: GridViewColumnResizeBehavior

        /// <summary>
        /// GridViewColumn class that gets attached to the GridViewColumn control
        /// </summary>
        public class GridViewColumnResizeBehavior
        {
            #region Private Fields

            private readonly GridViewColumn _element;

            #endregion Private Fields

            #region Public Constructors

            public GridViewColumnResizeBehavior(GridViewColumn element)
            {
                _element = element;
            }

            #endregion Public Constructors

            #region Public Properties

            public bool IsStatic
            {
                get { return StaticWidth >= 0; }
            }

            public double Mulitplier
            {
                get
                {
                    if (Width == "*" || Width == "1*") return 1;
                    if (Width.EndsWith("*"))
                    {
                        double perc;
                        if (double.TryParse(Width.Substring(0, Width.Length - 1), out perc))
                        {
                            return perc;
                        }
                    }
                    return 1;
                }
            }

            public double Percentage
            {
                get
                {
                    if (!IsStatic)
                    {
                        return Mulitplier * 100;
                    }

                    return 0;
                }
            }

            public double StaticWidth
            {
                get
                {
                    double result;
                    return double.TryParse(Width, out result) ? result : -1;
                }
            }

            public string Width { get; set; }

            #endregion Public Properties

            #region Public Methods

            public void SetWidth(double allowedSpace, double totalPercentage)
            {
                if (IsStatic)
                {
                    _element.Width = StaticWidth;
                }
                else
                {
                    double width = allowedSpace * (Percentage / totalPercentage);
                    _element.Width = width;
                }
            }

            #endregion Public Methods
        }

        #endregion Nested type: GridViewColumnResizeBehavior

        #region Nested type: ListViewResizeBehavior

        /// <summary>
        /// ListViewResizeBehavior class that gets attached to the ListView control
        /// </summary>
        public class ListViewResizeBehavior
        {
            #region Private Fields

            private static readonly TimeSpan Delay = TimeSpan.FromMilliseconds(500);
            private readonly ListView _element;
            private int _margin;

            #endregion Private Fields

            #region Public Constructors

            /// <summary>
            /// Initializes a new instance of the <see cref="ListViewResizeBehavior"/> class.
            /// </summary>
            /// <param name="element">The element.</param>
            /// <exception cref="ArgumentNullException">element</exception>
            public ListViewResizeBehavior(ListView element)
            {
                if (element == null)
                {
                    throw new ArgumentNullException("element");
                }

                _element = element;
                element.Loaded += OnLoaded;
                Margin = Convert.ToInt32(SystemParameters.VerticalScrollBarWidth) + 10;

                ResizeAndEnableSize().ConfigureAwait(continueOnCapturedContext: false);
            }

            #endregion Public Constructors

            #region Public Properties

            public bool Enabled
            {
                get;
                set;
            }

            public int Margin
            {
                get
                {
                    return _margin;
                }
                set
                {
                    _margin = value;
                    ResizeAndEnableSize().ConfigureAwait(continueOnCapturedContext: false);
                }
            }

            #endregion Public Properties

            #region Private Methods

            private static double GetAllocatedSpace(GridView gv)
            {
                double totalWidth = 0;
                foreach (GridViewColumn t in gv.Columns)
                {
                    var gridViewColumnResizeBehavior = t.GetValue(GridViewColumnResizeBehaviorProperty) as GridViewColumnResizeBehavior;
                    if (gridViewColumnResizeBehavior != null)
                    {
                        if (gridViewColumnResizeBehavior.IsStatic)
                        {
                            totalWidth += gridViewColumnResizeBehavior.StaticWidth;
                        }
                    }
                    else
                    {
                        totalWidth += t.ActualWidth;
                    }
                }
                return totalWidth;
            }

            private static IEnumerable<GridViewColumnResizeBehavior> GridViewColumnResizeBehaviors(GridView gv)
            {
                foreach (GridViewColumn t in gv.Columns)
                {
                    var gridViewColumnResizeBehavior = t.GetValue(GridViewColumnResizeBehaviorProperty) as GridViewColumnResizeBehavior;
                    if (gridViewColumnResizeBehavior != null)
                    {
                        yield return gridViewColumnResizeBehavior;
                    }
                }
            }

            private void OnLoaded(object sender, RoutedEventArgs e)
            {
                _element.SizeChanged += OnSizeChanged;
            }

            private async void OnSizeChanged(object sender, SizeChangedEventArgs e)
            {
                if (e.WidthChanged)
                {
                    _element.SizeChanged -= OnSizeChanged;
                    await ResizeAndEnableSize().ConfigureAwait(continueOnCapturedContext: false);
                }
            }

            private void Resize()
            {
                if (Enabled)
                {
                    double totalWidth = _element.ActualWidth;
                    var gv = _element.View as GridView;
                    if (gv != null)
                    {
                        double allowedSpace = totalWidth - GetAllocatedSpace(gv);
                        allowedSpace = allowedSpace - Margin;
                        double totalPercentage = GridViewColumnResizeBehaviors(gv).Sum(x => x.Percentage);
                        foreach (GridViewColumnResizeBehavior behavior in GridViewColumnResizeBehaviors(gv))
                        {
                            behavior.SetWidth(allowedSpace, totalPercentage);
                        }
                    }
                }
            }

            /// <summary>
            /// Action for resizing and re-enable the size lookup.
            /// This stops the columns from constantly resizing to improve performance.
            /// </summary>
            private async Task ResizeAndEnableSize()
            {
                await Task.Delay(Delay);
                Resize();
                _element.SizeChanged += OnSizeChanged;
            }

            #endregion Private Methods
        }

        #endregion Nested type: ListViewResizeBehavior
    }
}