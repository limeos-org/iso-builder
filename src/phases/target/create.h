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
 * @param use_cache Whether to use package caching.
 *
 * @return - `0` - Indicates successful creation.
 * @return - `-1` - Indicates copy failure.
 * @return - `-2` - Indicates package installation failure.
 */
int create_target_rootfs(const char *base_path, const char *path, int use_cache);
