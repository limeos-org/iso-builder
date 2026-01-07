#pragma once

/**
 * Creates a minimal Debian root filesystem using debootstrap.
 *
 * @param path The directory where the rootfs will be created.
 *
 * @return - `0` - Indicates successful creation.
 * @return - `-1` - Indicates debootstrap failure.
 * @return - `-2` - Indicates package list update failure.
 * @return - `-3` - Indicates package installation failure.
 * @return - `-4` - Indicates kernel copy failure.
 * @return - `-5` - Indicates initrd copy failure.
 */
int create_rootfs(const char *path);

/**
 * Strips unnecessary packages, docs, and locales from the rootfs.
 *
 * @param path The path to the rootfs directory.
 *
 * @return - `0` - Indicates successful stripping.
 * @return - `-1` - Indicates documentation removal failure.
 * @return - `-2` - Indicates locale removal failure.
 * @return - `-3` - Indicates apt cache removal failure.
 * @return - `-4` - Indicates apt lists removal failure.
 */
int strip_rootfs(const char *path);
