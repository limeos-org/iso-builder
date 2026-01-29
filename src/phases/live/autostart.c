/**
 * This code is responsible for configuring the live system to autostart
 * the installation wizard.
 */

#include "all.h"

/**
 * Writes the installer systemd service unit file.
 *
 * @return - `0` - Success.
 * @return - `-1` - Directory creation failure.
 * @return - `-2` - Service file write failure.
 */
static int write_installer_service(const char *rootfs_path)
{
    char path[COMMON_MAX_PATH_LENGTH];

    // Create the systemd service directory.
    snprintf(path, sizeof(path), "%s/etc/systemd/system", rootfs_path);
    if (common.mkdir_p(path) != 0)
    {
        return -1;
    }

    // Define the installer service unit content.
    const char *service_content =
        "[Unit]\n"
        "Description=LimeOS Installation Wizard\n"
        "After=systemd-user-sessions.service\n"
        "After=plymouth-quit-wait.service\n"
        "\n"
        "[Service]\n"
        "Type=simple\n"
        "Environment=PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin\n"
        "ExecStart=" CONFIG_INSTALL_BIN_PATH "/" CONFIG_INSTALLER_SERVICE_NAME "\n"
        "StandardInput=tty\n"
        "StandardOutput=tty\n"
        "TTYPath=/dev/tty1\n"
        "TTYReset=yes\n"
        "TTYVHangup=yes\n"
        "Restart=on-failure\n"
        "RestartSec=1\n"
        "\n"
        "[Install]\n"
        "WantedBy=multi-user.target\n";

    // Write the installer service unit file.
    snprintf(
        path, sizeof(path),
        "%s/etc/systemd/system/" CONFIG_INSTALLER_SERVICE_NAME ".service",
        rootfs_path
    );
    if (common.write_file(path, service_content) != 0)
    {
        return -2;
    }

    return 0;
}

/**
 * Enables the installer service to start at boot.
 *
 * @return - `0` - Success.
 * @return - `-1` - Directory creation failure.
 * @return - `-2` - Symlink creation failure.
 */
static int enable_installer_service(const char *rootfs_path)
{
    // Create the "target wants" directory.
    char wants_dir[COMMON_MAX_PATH_LENGTH];
    snprintf(
        wants_dir, sizeof(wants_dir),
        "%s/etc/systemd/system/multi-user.target.wants",
        rootfs_path
    );
    if (common.mkdir_p(wants_dir) != 0)
    {
        return -1;
    }

    // Create symlink to enable the service.
    char link_path[COMMON_MAX_PATH_LENGTH];
    snprintf(
        link_path, sizeof(link_path),
        "%s/" CONFIG_INSTALLER_SERVICE_NAME ".service",
        wants_dir
    );
    if (common.symlink_file("../" CONFIG_INSTALLER_SERVICE_NAME ".service", link_path) != 0)
    {
        LOG_ERROR("Failed to enable installer service");
        return -2;
    }

    return 0;
}

/**
 * Sets the default systemd target to multi-user.
 *
 * @return - `0` - Success.
 * @return - `-1` - Symlink creation failure.
 */
static int set_default_systemd_target(const char *rootfs_path)
{
    char link_path[COMMON_MAX_PATH_LENGTH];
    snprintf(
        link_path, sizeof(link_path),
        "%s/etc/systemd/system/default.target",
        rootfs_path
    );
    if (common.symlink_file("/lib/systemd/system/multi-user.target", link_path) != 0)
    {
        LOG_ERROR("Failed to set default target");
        return -1;
    }

    return 0;
}

/**
 * Disables getty on tty1 to prevent conflict with installer.
 *
 * @return - `0` - Success.
 * @return - `-1` - File removal failure.
 */
static int disable_tty1_getty(const char *rootfs_path)
{
    char getty_path[COMMON_MAX_PATH_LENGTH];
    snprintf(
        getty_path, sizeof(getty_path),
        "%s/etc/systemd/system/getty.target.wants/getty@tty1.service",
        rootfs_path
    );
    if (common.rm_file(getty_path) != 0)
    {
        LOG_ERROR("Failed to disable getty on tty1");
        return -1;
    }

    return 0;
}

int configure_live_autostart(const char *rootfs_path)
{
    LOG_INFO("Configuring live autostart...");

    // Write the installer service unit file.
    if (write_installer_service(rootfs_path) != 0)
    {
        return -1;
    }

    // Enable the installer service to start at boot.
    if (enable_installer_service(rootfs_path) != 0)
    {
        return -2;
    }

    // Set multi-user target as the default boot target.
    if (set_default_systemd_target(rootfs_path) != 0)
    {
        return -3;
    }

    // Disable getty on tty1 to prevent login prompt conflicts.
    if (disable_tty1_getty(rootfs_path) != 0)
    {
        return -4;
    }

    LOG_INFO("Live autostart configured successfully");

    return 0;
}
