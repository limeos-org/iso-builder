/**
 * This code enables essential systemd services for the target system.
 */

#include "all.h"

int brand_enable_services(const char *rootfs_path)
{
    LOG_INFO("Enabling essential services...");

    // Enable NetworkManager for network connectivity.
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command),
        "chroot %s systemctl enable NetworkManager", rootfs_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to enable NetworkManager");
        return -1;
    }

    LOG_INFO("Essential services enabled successfully");

    return 0;
}
