#pragma once

/**
 * Applies OS identity branding to a rootfs.
 *
 * Writes /etc/os-release, /etc/issue, and /etc/issue.net with
 * LimeOS branding and the specified version.
 *
 * @param rootfs_path The path to the rootfs directory.
 * @param version The version string for the OS (e.g., "1.0.0").
 *
 * @return - `0` - Success.
 * @return - `-1` - Failed to write /etc/os-release.
 * @return - `-2` - Failed to write /etc/issue.
 * @return - `-3` - Failed to write /etc/issue.net.
 */
int brand_os_identity(const char *rootfs_path, const char *version);
