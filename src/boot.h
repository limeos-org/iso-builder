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
 * @return - `-4` - Indicates isolinux config file write failure.
 */
int setup_isolinux(const char *rootfs_path);

/**
 * Configures Plymouth with the LimeOS boot splash.
 *
 * Installs the splash image and configures Plymouth to display it during boot.
 *
 * @param rootfs_path The path to the root filesystem directory.
 * @param logo_path The path to the splash logo image.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates splash logo not found.
 * @return - `-2` - Indicates theme directory creation failure.
 * @return - `-3` - Indicates splash logo copy failure.
 * @return - `-4` - Indicates theme file write failure.
 * @return - `-5` - Indicates script file write failure.
 */
int setup_splash(const char *rootfs_path, const char *logo_path);
