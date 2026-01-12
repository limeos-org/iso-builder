#pragma once

/**
 * Configures GRUB for silent boot on the installed system.
 *
 * Writes a drop-in config to /etc/default/grub.d/ with settings
 * for hidden menu, zero timeout, and quiet kernel boot.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Success.
 * @return - `-1` - Failed to create grub.d directory.
 * @return - `-2` - Failed to write config file.
 */
int brand_grub(const char *rootfs_path);
