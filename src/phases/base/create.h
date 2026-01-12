#pragma once

/**
 * Creates a minimal base rootfs using debootstrap.
 *
 * This creates the foundation that both payload and carrier rootfs will
 * be copied from. Only runs debootstrap and apt-get update, no packages.
 *
 * @param path The path to create the base rootfs.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates debootstrap failure.
 * @return - `-2` - Indicates apt-get update failure.
 */
int create_base_rootfs(const char *path);
