#pragma once

/**
 * Configures GRUB for UEFI boot.
 *
 * Sets up GRUB bootloader configuration with quiet splash kernel parameters.
 *
 * @param rootfs_path The path to the root filesystem directory.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates GRUB directory creation failure.
 * @return - `-2` - Indicates GRUB config file write failure.
 */
int setup_grub(const char *rootfs_path);
