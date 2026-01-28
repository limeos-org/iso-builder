#pragma once

/**
 * Installs LimeOS component binaries into the live rootfs.
 *
 * Copies all required components and any available optional components from
 * the components directory into the live rootfs bin directory.
 *
 * @param rootfs_path The path to the live rootfs directory.
 * @param components_path The path to the directory containing component
 * binaries.
 *
 * @return - `0` - Indicates successful installation.
 * @return - `-1` - Indicates bin directory creation failure.
 * @return - `-2` - Indicates component copy failure.
 * @return - `-3` - Indicates chmod failure.
 */
int install_live_components(const char *rootfs_path, const char *components_path);
