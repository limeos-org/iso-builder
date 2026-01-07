#pragma once

/**
 * Copies component binaries into the rootfs.
 *
 * @param rootfs_path The path to the rootfs directory.
 * @param components_path The path to the directory containing component
 *        binaries.
 *
 * @return - `0` - Indicates successful installation.
 * @return - `-1` - Indicates installation failure.
 */
int install_components(const char *rootfs_path, const char *components_path);
