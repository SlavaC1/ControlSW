using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows.Interactivity;

namespace BSS.MVVM.View.Behaviors
{
    /// <summary>
    /// A behavior of scroll list box to last item.
    /// </summary>
    internal class ScrollToLastItemBehavior : Behavior<ListBox>
    {
        /// <summary>
        /// Called after the behavior is attached to an AssociatedObject.
        /// </summary>
        /// <remarks>
        /// Override this to hook up functionality to the AssociatedObject.
        /// </remarks>
        protected override void OnAttached()
        {
            base.OnAttached();

            INotifyCollectionChanged sourceCollection = AssociatedObject.Items.SourceCollection as INotifyCollectionChanged;
            sourceCollection.CollectionChanged += HandleSourceCollectionChanged;
        }

        /// <summary>
        /// Called when the behavior is being detached from its AssociatedObject, but before it has actually occurred.
        /// </summary>
        /// <remarks>
        /// Override this to unhook functionality from the AssociatedObject.
        /// </remarks>
        protected override void OnDetaching()
        {
            INotifyCollectionChanged sourceCollection = AssociatedObject.Items.SourceCollection as INotifyCollectionChanged;
            sourceCollection.CollectionChanged -= HandleSourceCollectionChanged;

            base.OnDetaching();
        }

        /// <summary>
        /// Handles the source collection changed event: scroll to last item.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="NotifyCollectionChangedEventArgs"/> instance containing the event data.</param>
        private void HandleSourceCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (AssociatedObject.Items.Count > 0)
            {
                AssociatedObject.Items.MoveCurrentToLast();
                AssociatedObject.ScrollIntoView(AssociatedObject.Items.CurrentItem);
            }
        }
    }
}
