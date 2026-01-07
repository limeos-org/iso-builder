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
