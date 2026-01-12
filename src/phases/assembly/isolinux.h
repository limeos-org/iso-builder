#pragma once

/**
 * Configures isolinux for legacy BIOS boot.
 *
 * Sets up isolinux bootloader configuration with quiet splash kernel
 * parameters.
 *
 * @param rootfs_path The path to the root filesystem directory.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates isolinux directory creation failure.
 * @return - `-2` - Indicates isolinux.bin copy failure.
 * @return - `-3` - Indicates ldlinux.c32 copy failure.
 * @return - `-4` - Indicates vesamenu.c32 copy failure.
 * @return - `-5` - Indicates libutil.c32 copy failure.
 * @return - `-6` - Indicates libcom32.c32 copy failure.
 * @return - `-7` - Indicates black.png creation failure.
 * @return - `-8` - Indicates isolinux config file write failure.
 */
int setup_isolinux(const char *rootfs_path);
