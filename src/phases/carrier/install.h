#pragma once

/**
 * Copies the installer binary into the carrier rootfs.
 *
 * @param rootfs_path The path to the carrier rootfs directory.
 * @param components_path The path to the directory containing component
 *        binaries.
 *
 * @return - `0` - Indicates successful installation.
 * @return - `-1` - Indicates installation failure.
 */
int install_carrier_components(const char *rootfs_path, const char *components_path);
