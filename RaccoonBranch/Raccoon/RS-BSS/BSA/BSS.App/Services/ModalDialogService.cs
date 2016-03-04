#pragma warning disable 1998

using BSS.App.Properties;
using GalaSoft.MvvmLight.Threading;
using GalaSoft.MvvmLight.Views;
using System;
using System.Threading.Tasks;
using System.Windows;

namespace BSS.App.Services
{
    /// <summary>
    /// A service for displaying modal dialogs.
    /// </summary>
    public class ModalDialogService : IDialogService
    {
        #region Public Methods

        /// <summary>
        /// Displays information about an error.
        /// </summary>
        /// <param name="error">The exception of which the message must be shown to the user.</param>
        /// <param name="title">The title of the dialog box. This may be null.</param>
        /// <param name="buttonText">ignored.</param>
        /// <param name="afterHideCallback">A callback that should be executed after
        /// the dialog box is closed by the user.</param>
        /// <returns>
        /// A Task allowing this async method to be awaited.
        /// </returns>
        public async Task ShowError(Exception error, string title, string buttonText, Action afterHideCallback)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    MessageBox.Show(Application.Current.MainWindow, error.Message, title, MessageBoxButton.OK, MessageBoxImage.Error);
                    if (afterHideCallback != null)
                    {
                        afterHideCallback();
                    }
                });
        }

        /// <summary>
        /// Displays information about an error.
        /// </summary>
        /// <param name="message">The message to be shown to the user.</param>
        /// <param name="title">The title of the dialog box. This may be null.</param>
        /// <param name="buttonText">The text shown in the only button
        /// in the dialog box. If left null, the text "OK" will be used.</param>
        /// <param name="afterHideCallback">A callback that should be executed after
        /// the dialog box is closed by the user.</param>
        /// <returns>
        /// A Task allowing this async method to be awaited.
        /// </returns>
        public async Task ShowError(string message, string title, string buttonText, Action afterHideCallback)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    MessageBox.Show(Application.Current.MainWindow, message, title, MessageBoxButton.OK, MessageBoxImage.Exclamation);
                    if (afterHideCallback != null)
                    {
                        afterHideCallback();
                    }
                });
        }

        /// <summary>
        /// Displays information to the user. The dialog box will have only
        /// one button.
        /// </summary>
        /// <param name="message">The message to be shown to the user.</param>
        /// <param name="title">The title of the dialog box. This may be null.</param>
        /// <param name="buttonConfirmText">The text shown in the "confirm" button
        /// in the dialog box. If left null, the text "OK" will be used.</param>
        /// <param name="buttonCancelText">The text shown in the "cancel" button
        /// in the dialog box. If left null, the text "Cancel" will be used.</param>
        /// <param name="afterHideCallback">A callback that should be executed after
        /// the dialog box is closed by the user. The callback method will get a boolean
        /// parameter indicating if the "confirm" button (true) or the "cancel" button
        /// (false) was pressed by the user.</param>
        /// <returns>
        /// A Task allowing this async method to be awaited. The task will return
        /// true or false depending on the dialog result.
        /// </returns>
        public async Task<bool> ShowMessage(string message, string title, string buttonConfirmText, string buttonCancelText, Action<bool> afterHideCallback)
        {
            return DispatcherHelper.UIDispatcher.Invoke(() =>
                {
                    MessageBoxButton button = MessageBoxButton.OKCancel;
                    if (buttonConfirmText == Resources.Yes)
                    {
                        button = MessageBoxButton.YesNo;
                    }

                    MessageBoxResult result = MessageBox.Show(Application.Current.MainWindow, message, title, button, MessageBoxImage.Question);
                    MessageBoxResult confirmButton = (button == MessageBoxButton.YesNo)
                        ? MessageBoxResult.Yes
                        : MessageBoxResult.OK;

                    bool confirm = (result == confirmButton);
                    if (afterHideCallback != null)
                    {
                        afterHideCallback(confirm);
                    }

                    return confirm;
                });
        }

        /// <summary>
        /// Displays information to the user. The dialog box will have only
        /// one button.
        /// </summary>
        /// <param name="message">The message to be shown to the user.</param>
        /// <param name="title">The title of the dialog box. This may be null.</param>
        /// <param name="buttonText">The text shown in the only button
        /// in the dialog box. If left null, the text "OK" will be used.</param>
        /// <param name="afterHideCallback">A callback that should be executed after
        /// the dialog box is closed by the user.</param>
        /// <returns>
        /// A Task allowing this async method to be awaited.
        /// </returns>
        public async Task ShowMessage(string message, string title, string buttonText, Action afterHideCallback)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    MessageBox.Show(Application.Current.MainWindow, message, title, MessageBoxButton.OK, MessageBoxImage.Information);
                    if (afterHideCallback != null)
                    {
                        afterHideCallback();
                    }
                });
        }

        /// <summary>
        /// Displays information to the user. The dialog box will have only
        /// one button with the text "OK".
        /// </summary>
        /// <param name="message">The message to be shown to the user.</param>
        /// <param name="title">The title of the dialog box. This may be null.</param>
        /// <returns>
        /// A Task allowing this async method to be awaited.
        /// </returns>
        public async Task ShowMessage(string message, string title)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    MessageBox.Show(Application.Current.MainWindow, message, title, MessageBoxButton.OK, MessageBoxImage.Information);
                });
        }

        /// <summary>
        /// Displays information to the user in a simple dialog box. The dialog box will have only
        /// one button with the text "OK". This method should be used for debugging purposes.
        /// </summary>
        /// <param name="message">The message to be shown to the user.</param>
        /// <param name="title">The title of the dialog box. This may be null.</param>
        /// <returns>
        /// A Task allowing this async method to be awaited.
        /// </returns>
        public async Task ShowMessageBox(string message, string title)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    MessageBox.Show(Application.Current.MainWindow, message, title, MessageBoxButton.OK, MessageBoxImage.Information);
                });
        }

        #endregion Public Methods
    }
}

#pragma warning restore 1998