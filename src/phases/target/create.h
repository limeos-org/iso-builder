#pragma once

/**
 * Creates the target rootfs by copying from base and installing packages.
 *
 * The target rootfs is the full system that gets installed to disk. It
 * includes bootloaders, networking, and other packages needed for a
 * functional system.
 *
 * @param base_path The path to the base rootfs to copy from.
 * @param path The directory where the rootfs will be created.
 *
 * @return - `0` - Indicates successful creation.
 * @return - `-1` - Indicates base path quoting failure.
 * @return - `-2` - Indicates destination path quoting failure.
 * @return - `-3` - Indicates base rootfs copy failure.
 * @return - `-4` - Indicates package installation failure.
 * @return - `-5` - Indicates GPU driver initramfs failure.
 * @return - `-6` - Indicates APT cache cleanup failure.
 */
int create_target_rootfs(const char *base_path, const char *path);
