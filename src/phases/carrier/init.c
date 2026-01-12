/**
 * This code is responsible for configuring the init system in the
 * carrier rootfs.
 */

#include "all.h"

static int write_installer_service(const char *rootfs_path)
{
    char path[COMMAND_PATH_MAX_LENGTH];

    // Create the systemd service directory.
    snprintf(path, sizeof(path), "%s/etc/systemd/system", rootfs_path);
    if (mkdir_p(path) != 0)
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
    snprintf(path, sizeof(path), "%s/etc/systemd/system/" CONFIG_INSTALLER_SERVICE_NAME ".service", rootfs_path);
    if (write_file(path, service_content) != 0)
    {
        return -1;
    }

    return 0;
}

static int enable_installer_service(const char *rootfs_path)
{
    char wants_dir[COMMAND_PATH_MAX_LENGTH];
    char link_path[COMMAND_PATH_MAX_LENGTH];

    // Create the target wants directory.
    snprintf(wants_dir, sizeof(wants_dir), "%s/etc/systemd/system/multi-user.target.wants", rootfs_path);
    if (mkdir_p(wants_dir) != 0)
    {
        return -1;
    }

    // Create symlink to enable the service.
    snprintf(link_path, sizeof(link_path), "%s/" CONFIG_INSTALLER_SERVICE_NAME ".service", wants_dir);
    if (symlink_file("../" CONFIG_INSTALLER_SERVICE_NAME ".service", link_path) != 0)
    {
        LOG_ERROR("Failed to enable installer service");
        return -1;
    }

    return 0;
}

static int set_default_systemd_target(const char *rootfs_path)
{
    char link_path[COMMAND_PATH_MAX_LENGTH];

    // Set multi-user target as default.
    snprintf(link_path, sizeof(link_path), "%s/etc/systemd/system/default.target", rootfs_path);
    if (symlink_file("/lib/systemd/system/multi-user.target", link_path) != 0)
    {
        LOG_ERROR("Failed to set default target");
        return -1;
    }

    return 0;
}

static int disable_tty1_getty(const char *rootfs_path)
{
    char getty_path[COMMAND_PATH_MAX_LENGTH];

    // Remove getty on tty1 to prevent conflict with installer.
    snprintf(
        getty_path, sizeof(getty_path),
        "%s/etc/systemd/system/getty.target.wants/getty@tty1.service",
        rootfs_path
    );
    if (rm_file(getty_path) != 0)
    {
        LOG_ERROR("Failed to disable getty on tty1");
        return -1;
    }

    return 0;
}

int configure_carrier_init(const char *rootfs_path)
{
    LOG_INFO("Configuring carrier init system...");

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

    LOG_INFO("Carrier init system configured successfully");

    return 0;
}
