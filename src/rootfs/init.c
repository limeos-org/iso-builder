/**
 * This code is responsible for configuring the init system in the rootfs.
 */

#include "all.h"

static int write_installer_service(const char *rootfs_path)
{
    char path[MAX_PATH_LENGTH];

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
        "ExecStart=/usr/local/bin/installation-wizard\n"
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
    snprintf(path, sizeof(path), "%s/etc/systemd/system/limeos-installer.service", rootfs_path);
    if (write_file(path, service_content) != 0)
    {
        return -1;
    }

    return 0;
}

static int enable_installer_service(const char *rootfs_path)
{
    char command[MAX_COMMAND_LENGTH];
    char path[MAX_PATH_LENGTH];

    // Create the target wants directory.
    snprintf(path, sizeof(path), "%s/etc/systemd/system/multi-user.target.wants", rootfs_path);
    if (mkdir_p(path) != 0)
    {
        return -1;
    }

    // Create symlink to enable the service.
    snprintf(
        command, sizeof(command),
        "ln -sf ../limeos-installer.service %s/etc/systemd/system/multi-user.target.wants/limeos-installer.service",
        rootfs_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

static int set_default_target(const char *rootfs_path)
{
    char command[MAX_COMMAND_LENGTH];

    // Set multi-user target as default.
    snprintf(
        command, sizeof(command),
        "ln -sf /lib/systemd/system/multi-user.target %s/etc/systemd/system/default.target",
        rootfs_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

static int disable_getty(const char *rootfs_path)
{
    char command[MAX_COMMAND_LENGTH];

    // Remove getty on tty1 to prevent conflict with installer.
    snprintf(
        command, sizeof(command),
        "rm -f %s/etc/systemd/system/getty.target.wants/getty@tty1.service",
        rootfs_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

int configure_init(const char *rootfs_path)
{
    LOG_INFO("Configuring init system...");

    if (write_installer_service(rootfs_path) != 0)
    {
        return -1;
    }
    if (enable_installer_service(rootfs_path) != 0)
    {
        return -2;
    }
    if (set_default_target(rootfs_path) != 0)
    {
        return -3;
    }
    if (disable_getty(rootfs_path) != 0)
    {
        return -4;
    }

    LOG_INFO("Init system configured successfully");

    return 0;
}
