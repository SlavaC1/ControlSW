using System;
using System.Configuration;
using System.ServiceModel.Configuration;

namespace WcfInfras.Client
{
    /// <summary>
    /// Represents a configuration element that contains sub-elements that specify custom header behavior extensions.
    /// </summary>
    public class CustomHeaderBehaviorExtension : BehaviorExtensionElement
    {
        #region Public Properties

        /// <summary>
        /// Gets or sets the content of the header.
        /// </summary>
        /// <value>
        /// The content of the header.
        /// </value>
        [ConfigurationProperty("headerContent")]
        public string HeaderContent
        {
            get { return (string)this["headerContent"]; }
            set { this["headerContent"] = value; }
        }

        /// <summary>
        /// Gets or sets the name of the header.
        /// </summary>
        /// <value>
        /// The name of the header.
        /// </value>
        [ConfigurationProperty("headerName")]
        public string HeaderName
        {
            get { return (string)this["headerName"]; }
            set { this["headerName"] = value; }
        }

        /// <summary>
        /// Gets or sets the header namespace.
        /// </summary>
        /// <value>
        /// The header namespace.
        /// </value>
        [ConfigurationProperty("headerNamespace")]
        public string HeaderNamespace
        {
            get { return (string)this["headerNamespace"]; }
            set { this["headerNamespace"] = value; }
        }

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
    }
}