#pragma once

/**
 * Gets the APT package cache directory path.
 *
 * Returns the path to ~/.cache/limeos-iso-builder/packages/apt/ (or equivalent
 * with XDG_CACHE_HOME).
 *
 * @param out_path The buffer to store the cache directory path.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure (HOME not set).
 */
int get_package_cache_dir(char *out_path, size_t buffer_length);

/**
 * Sets up package cache bind mount for a rootfs.
 *
 * Creates the host cache directory if needed, then bind mounts it to
 * the rootfs's /var/cache/apt/archives directory.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int setup_package_cache_mount(const char *rootfs_path);

/**
 * Tears down package cache bind mount for a rootfs.
 *
 * Unmounts the bind mount from the rootfs's /var/cache/apt/archives.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int teardown_package_cache_mount(const char *rootfs_path);
