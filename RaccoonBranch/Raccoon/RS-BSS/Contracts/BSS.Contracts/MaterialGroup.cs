using System;

namespace BSS.Contracts
{
    [Flags]
    public enum MaterialKind
    {
        /// <summary>
        /// No group.
        /// </summary>
        None = 0,

        /// <summary>
        /// Model material.
        /// </summary>
        M = 1,

        /// <summary>
        /// Support material.
        /// </summary>
        S = 2,

        /// <summary>
        /// Both model and support material.
        /// </summary>
        MS = M | S
    }
}