#pragma once

/**
 * Downloads and bundles bootloader .deb packages into the live rootfs.
 *
 * Downloads bootloader packages (grub-pc, grub-efi) that cannot be
 * pre-installed due to conflicts and stores them for the installer to
 * selectively install based on the target system's boot mode.
 *
 * @param live_rootfs_path The path to the live rootfs directory.
 *
 * @return - `0` - Indicates successful bundling.
 * @return - `-1` - Indicates directory creation failure.
 * @return - `-2` - Indicates package download failure.
 */
int bundle_live_packages(const char *live_rootfs_path);
