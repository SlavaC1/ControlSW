using System;

namespace BSS.Contracts
{
    /// <summary>
    /// Defines tag registration status.
    /// </summary>
    public enum TagRegistration
    {
        /// <summary>
        /// Tag is not found in list.
        /// </summary>
        Unknown,
        /// <summary>
        /// Tag is new.
        /// </summary>
        New,
        /// <summary>
        /// Tag is used.
        /// </summary>
        Used,
    }
}
