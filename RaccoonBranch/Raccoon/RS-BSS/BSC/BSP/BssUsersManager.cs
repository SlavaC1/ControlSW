using System.Linq;
using System.Security.Permissions;

namespace CBS.BSP
{
    /// <summary>
    /// Manages users of burning station software.
    /// </summary>
    public class BssUsersManager
    {
        #region Private Fields

        /// <summary>
        /// A collection of names of the allowed users.
        /// </summary>
        private string[] allowedUsers;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BssUsersManager"/> class.
        /// </summary>
        /// <param name="allowedUsers">The allowed users.</param>
        public BssUsersManager(string[] allowedUsers)
        {
            this.allowedUsers = allowedUsers;
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Checks if user name is allowed.
        /// </summary>
        /// <param name="userName">Name of the user.</param>
        /// <returns></returns>
        public bool CheckUserName(string userName)
        {
            if (allowedUsers != null)
            {
                if (!allowedUsers.Contains(userName))
                {
                    return false;
                }
            }

            PrincipalPermission principalPermission = new PrincipalPermission(userName, null);
            principalPermission.Demand();

            return true;
        }

        #endregion Public Methods
    }
}