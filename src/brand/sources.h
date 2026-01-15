#pragma once

/**
 * Configures APT package sources for the target system.
 *
 * Writes /etc/apt/sources.list with Debian repositories including
 * main, updates, and security sources.
 *
 * @param rootfs_path The path to the target rootfs directory.
 *
 * @return - `0` - Success.
 * @return - `-1` - Failed to write sources.list.
 */
int brand_apt_sources(const char *rootfs_path);
