#pragma once

/**
 * Enables essential systemd services for the target system.
 *
 * Enables NetworkManager for network connectivity on boot.
 *
 * @param rootfs_path The path to the target rootfs directory.
 *
 * @return - `0` - Success.
 * @return - `-1` - Failed to enable NetworkManager.
 */
int brand_enable_services(const char *rootfs_path);
