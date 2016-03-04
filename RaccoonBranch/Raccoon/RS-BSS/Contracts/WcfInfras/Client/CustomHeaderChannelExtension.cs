using System.ServiceModel;

namespace WcfInfras.Client
{
    /// <summary>
    /// Provides a channel extension for adding runtime customized headers.
    /// </summary>
    public class CustomHeaderChannelExtension : IExtension<IContextChannel>
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

        #region Public Methods

        /// <summary>
        /// Attaches the specified owner.
        /// </summary>
        /// <param name="owner">The owner.</param>
        public void Attach(IContextChannel owner)
        {
        }

        /// <summary>
        /// Detaches the specified owner.
        /// </summary>
        /// <param name="owner">The owner.</param>
        public void Detach(IContextChannel owner)
        {
        }

        #endregion Public Methods
    }
}