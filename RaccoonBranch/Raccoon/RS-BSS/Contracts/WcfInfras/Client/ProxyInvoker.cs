using System;
using System.Linq;
using System.Net;
using System.Security.Authentication;
using System.ServiceModel;
using System.ServiceModel.Security;
using System.Threading.Tasks;

namespace WcfInfras.Client
{
    /// <summary>
    /// Holds common credential properties for <see cref="ProxyInvoker{T}"/> classes.
    /// </summary>
    public static class ProxyInvokerCredentials
    {
        #region Public Properties

        /// <summary>
        /// Gets or sets the client credential.
        /// </summary>
        /// <value>
        /// The client credential.
        /// </value>
        public static NetworkCredential ClientCredential { get; set; }

        #endregion Public Properties
    }

    /// <summary>
    /// Provides argument for Error event.
    /// </summary>
    public class ErrorEventArgs : EventArgs
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ErrorEventArgs"/> class.
        /// </summary>
        /// <param name="error">The error.</param>
        public ErrorEventArgs(string error)
        {
            Error = error;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the error.
        /// </summary>
        /// <value>
        /// The error.
        /// </value>
        public string Error { get; private set; }

        #endregion Public Properties
    }

    /// <summary>
    /// Provides argument for Exception event.
    /// </summary>
    public class ExceptionEventArgs : EventArgs
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ExceptionEventArgs"/> class.
        /// </summary>
        /// <param name="exception">The exception.</param>
        public ExceptionEventArgs(Exception exception)
        {
            Exception = exception;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the exception.
        /// </summary>
        /// <value>
        /// The exception.
        /// </value>
        public Exception Exception { get; private set; }

        #endregion Public Properties
    }

    /// <summary>
    /// An invoker for proxy methods.
    /// </summary>
    /// <typeparam name="T">Type of service contract implemented by proxy.</typeparam>
    public class ProxyInvoker<T> : IDisposable
    {
        #region Private Fields

        /// <summary>
        /// The channel factory.
        /// </summary>
        private ChannelFactory<T> _channelFactory;

        /// <summary>
        /// The proxy.
        /// </summary>
        private IClientChannel _proxy;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ProxyInvoker{T}"/> class.
        /// </summary>
        /// <param name="endpointConfigurationName">Name of the endpoint configuration.</param>
        public ProxyInvoker(string endpointConfigurationName)
            : this(null, endpointConfigurationName)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ProxyInvoker{T}"/> class.
        /// </summary>
        /// <param name="callbackObject">The callback object.</param>
        /// <param name="endpointConfigurationName">Name of the endpoint configuration.</param>
        public ProxyInvoker(object callbackObject, string endpointConfigurationName)
        {
            if (callbackObject == null)
            {
                _channelFactory = new ChannelFactory<T>(endpointConfigurationName ?? String.Empty);
            }
            else
            {
                _channelFactory = new DuplexChannelFactory<T>(callbackObject, endpointConfigurationName ?? String.Empty);
            }

            if (ProxyInvokerCredentials.ClientCredential != null)
            {
                _channelFactory.Credentials.Windows.ClientCredential = ProxyInvokerCredentials.ClientCredential;
            }
        }

        #endregion Public Constructors

        #region Public Events

        /// <summary>
        /// Occurs when error is detected.
        /// </summary>
        public event EventHandler<ErrorEventArgs> Error;

        /// <summary>
        /// Occurs when exception is raised.
        /// </summary>
        public event EventHandler<ExceptionEventArgs> Exception;

        #endregion Public Events

        #region Protected Methods

        /// <summary>
        /// Raises the <see cref="E:Error" /> event.
        /// </summary>
        /// <param name="e">The <see cref="ErrorEventArgs"/> instance containing the event data.</param>
        protected virtual void OnError(ErrorEventArgs e)
        {
            EventHandler<ErrorEventArgs> temp = Error;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Raises the <see cref="E:Exception" /> event.
        /// </summary>
        /// <param name="e">The <see cref="ExceptionEventArgs"/> instance containing the event data.</param>
        protected virtual void OnException(ExceptionEventArgs e)
        {
            EventHandler<ExceptionEventArgs> temp = Exception;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        #endregion Protected Methods

        #region Public Properties

        /// <summary>
        /// Gets a value indicating whether proxy is rejected by service.
        /// </summary>
        /// <value>
        /// <c>true</c> if proxy is rejected by service; otherwise, <c>false</c>.
        /// </value>
        public bool IsRejected { get; private set; }

        /// <summary>
        /// Gets the last error.
        /// </summary>
        /// <value>
        /// The last error.
        /// </value>
        public string LastError { get; private set; }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Creates the proxy.
        /// </summary>
        public void CreateProxy()
        {
            RemoveProxy();
            _proxy = CreateProxyInternal();
        }

        /// <summary>
        /// Invokes the specified code block.
        /// </summary>
        /// <param name="codeBlock">The code block.</param>
        /// <returns></returns>
        public bool Invoke(Action<T> codeBlock)
        {
            bool isConnected = false;
            IsRejected = false;

            IClientChannel proxy = _proxy;
            try
            {
                if (proxy == null)
                {
                    proxy = CreateProxyInternal();
                }

                codeBlock((T)proxy);
                isConnected = true;
                LastError = null;
            }
            catch (FaultException<ExceptionDetail> customFaultEx)
            {
                LastError = customFaultEx.Message;
                OnException(new ExceptionEventArgs(new ApplicationException(LastError)));
                isConnected = true;
            }
            catch (FaultException customFaultEx)
            {
                LastError = customFaultEx.Message;
                OnException(new ExceptionEventArgs(new ApplicationException(LastError)));
                isConnected = true;
            }
            catch (Exception ex)
            {
                Exception innermostException = GetInnermostException(ex);
                LastError = innermostException.Message;
                bool isWcfException = IsWcfException(innermostException);
                if (isWcfException)
                {
                    OnError(new ErrorEventArgs(LastError));
                }
                else
                {
                    IsRejected = ex is SecurityNegotiationException || ex is InvalidCredentialException;
                    OnException(new ExceptionEventArgs(ex));
                }
            }

            if (isConnected == false)
            {
                if (proxy != null)
                {
                    proxy.Abort();
                }

                if (_proxy != null)
                {
                    CreateProxy();
                }
            }
            else
            {
                if (_proxy == null)
                {
                    proxy.Close();
                }
            }

            return isConnected;
        }

        /// <summary>
        /// Invokes the specified code block.
        /// </summary>
        /// <param name="codeBlock">The code block.</param>
        /// <returns></returns>
        public async Task<bool> InvokeAsync(Func<T, Task> codeBlock)
        {
            bool isConnected = false;
            IsRejected = false;

            IClientChannel proxy = _proxy;
            try
            {
                if (proxy == null)
                {
                    proxy = CreateProxyInternal();
                }

                await codeBlock((T)proxy);
                isConnected = true;
                LastError = null;
            }
            catch (FaultException<ExceptionDetail> customFaultEx)
            {
                LastError = customFaultEx.Message;
                OnException(new ExceptionEventArgs(new ApplicationException(LastError)));
                isConnected = true;
            }
            catch (FaultException customFaultEx)
            {
                LastError = customFaultEx.Message;
                OnException(new ExceptionEventArgs(new ApplicationException(LastError)));
                isConnected = true;
            }
            catch (Exception ex)
            {
                Exception innermostException = GetInnermostException(ex);
                LastError = innermostException.Message;
                bool isWcfException = IsWcfException(innermostException);
                if (isWcfException)
                {
                    OnError(new ErrorEventArgs(LastError));
                }
                else
                {
                    IsRejected = ex is SecurityNegotiationException || ex is InvalidCredentialException;
                    OnException(new ExceptionEventArgs(ex));
                }
            }

            if (isConnected == false)
            {
                if (proxy != null)
                {
                    proxy.Abort();
                }

                if (_proxy != null)
                {
                    CreateProxy();
                }
            }
            else
            {
                if (_proxy == null)
                {
                    proxy.Close();
                }
            }

            return isConnected;
        }

        /// <summary>
        /// Removes the proxy.
        /// </summary>
        public void RemoveProxy()
        {
            if (_proxy != null)
            {
                _proxy.Close();
                _proxy = null;
            }
        }

        /// <summary>
        /// Resets the outgoing message header.
        /// </summary>
        public void ResetHeader()
        {
            SetHeader(null, null, null);
        }

        /// <summary>
        /// Sets the outgoing message header.
        /// </summary>
        /// <param name="headerName">Name of the header.</param>
        /// <param name="headerNamespace">The header namespace.</param>
        /// <param name="headerContent">Content of the header.</param>
        public void SetHeader(string headerName, string headerNamespace, string headerContent)
        {
            CustomHeaderBehavior customHeaderBehavior = _channelFactory.Endpoint.EndpointBehaviors
                .OfType<CustomHeaderBehavior>()
                .FirstOrDefault();

            if (customHeaderBehavior != null)
            {
                _channelFactory.Endpoint.EndpointBehaviors.Remove(customHeaderBehavior);
            }

            _channelFactory.Endpoint.EndpointBehaviors.Add(new CustomHeaderBehavior
            {
                HeaderName = headerName,
                HeaderNamespace = headerNamespace,
                HeaderContent = headerContent,
            });
        }

        #endregion Public Methods

        #region Private Methods

        private IClientChannel CreateProxyInternal()
        {
            IClientChannel proxy = _channelFactory.CreateChannel() as IClientChannel;
            if (proxy != null)
            {
                CustomHeaderBehavior customHeaderBehavior = _channelFactory.Endpoint.EndpointBehaviors
                    .OfType<CustomHeaderBehavior>()
                    .FirstOrDefault();

                if (customHeaderBehavior != null)
                {
                    proxy.Extensions.Add(new CustomHeaderChannelExtension
                    {
                        HeaderName = customHeaderBehavior.HeaderName,
                        HeaderNamespace = customHeaderBehavior.HeaderNamespace,
                        HeaderContent = customHeaderBehavior.HeaderContent,
                    });
                }
            }

            return proxy;
        }

        /// <summary>
        /// Gets the innermost exception.
        /// </summary>
        /// <param name="ex">The exception.</param>
        /// <returns></returns>
        private static Exception GetInnermostException(Exception ex)
        {
            if (ex == null)
            {
                return null;
            }

            Exception innerException = ex.InnerException;
            while (innerException != null)
            {
                ex = innerException;
                innerException = innerException.InnerException;
            }

            return ex;
        }

        /// <summary>
        /// Determines whether <paramref name="ex"/> represents a WCF exception.
        /// </summary>
        /// <param name="ex">The exception.</param>
        /// <returns></returns>
        private static bool IsWcfException(Exception ex)
        {
            return (ex is ChannelTerminatedException) || // Typically thrown on the client when a channel is terminated due to the server closing the connection.
                (ex is EndpointNotFoundException) || // Remote endpoint could not be found or reached. The endpoint may not be found or reachable because the remote endpoint is down, the remote endpoint is unreachable, or because the remote network is unreachable.
                (ex is ServerTooBusyException) ||
                (ex is TimeoutException) ||
                (ex is CommunicationException) ||
                (ex is ObjectDisposedException);
        }

        #endregion Private Methods

        #region IDisposable Members

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(dispose: true);
        }

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources.
        /// </summary>
        /// <param name="dispose"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected virtual void Dispose(bool dispose)
        {
            if (dispose)
            {
                RemoveProxy();
            }
        }

        #endregion IDisposable Members
    }
}