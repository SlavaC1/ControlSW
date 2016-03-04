using System;
using System.ServiceModel.Channels;
using System.ServiceModel.Configuration;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;

namespace WcfInfras.Client
{
    /// <summary>
    /// Represents an endpoint behavior supporting custom header message inspectors.
    /// </summary>
    public class CustomHeaderBehavior : BehaviorExtensionElement, IEndpointBehavior
    {
        #region Public Properties

        /// <summary>
        /// Gets or sets the content of the header.
        /// </summary>
        /// <value>
        /// The content of the header.
        /// </value>
        public string HeaderContent { get; set; }

        /// <summary>
        /// Gets or sets the name of the header.
        /// </summary>
        /// <value>
        /// The name of the header.
        /// </value>
        public string HeaderName { get; set; }
        /// <summary>
        /// Gets or sets the header namespace.
        /// </summary>
        /// <value>
        /// The header namespace.
        /// </value>
        public string HeaderNamespace { get; set; }

        #endregion Public Properties

        #region BehaviorExtensionElement Members

        /// <summary>
        /// Gets the type of behavior.
        /// </summary>
        public override Type BehaviorType
        {
            get { return typeof(CustomHeaderBehavior); }
        }

        /// <summary>
        /// Creates a behavior extension based on the current configuration settings.
        /// </summary>
        /// <returns>
        /// The behavior extension.
        /// </returns>
        protected override object CreateBehavior()
        {
            return new CustomHeaderBehavior
            {
                HeaderContent = this.HeaderContent,
                HeaderName = this.HeaderName,
                HeaderNamespace = this.HeaderNamespace,
            };
        }

        #endregion BehaviorExtensionElement Members

        #region IEndpointBehavior Members

        /// <summary>
        /// Implement to pass data at runtime to bindings to support custom behavior.
        /// </summary>
        /// <param name="endpoint">The endpoint to modify.</param>
        /// <param name="bindingParameters">The objects that binding elements require to support the behavior.</param>
        public void AddBindingParameters(ServiceEndpoint endpoint, BindingParameterCollection bindingParameters)
        {
            // do nothing.
        }

        /// <summary>
        /// Implements a modification or extension of the client across an endpoint.
        /// </summary>
        /// <param name="endpoint">The endpoint that is to be customized.</param>
        /// <param name="clientRuntime">The client runtime to be customized.</param>
        public void ApplyClientBehavior(ServiceEndpoint endpoint, ClientRuntime clientRuntime)
        {
            clientRuntime.MessageInspectors.Add(new CustomHeaderClientMessageInspector());
        }

        /// <summary>
        /// Implements a modification or extension of the service across an endpoint.
        /// </summary>
        /// <param name="endpoint">The endpoint that exposes the contract.</param>
        /// <param name="endpointDispatcher">The endpoint dispatcher to be modified or extended.</param>
        public void ApplyDispatchBehavior(ServiceEndpoint endpoint, EndpointDispatcher endpointDispatcher)
        {
            // do nothing.
        }

        /// <summary>
        /// Implement to confirm that the endpoint meets some intended criteria.
        /// </summary>
        /// <param name="endpoint">The endpoint to validate.</param>
        public void Validate(ServiceEndpoint endpoint)
        {
            // do nothing.
        }

        #endregion IEndpointBehavior Members
    }
}