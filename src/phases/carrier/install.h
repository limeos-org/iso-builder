#pragma once

/**
 * Installs LimeOS component binaries into the carrier rootfs.
 *
 * Copies all required components and any available optional components from
 * the components directory into the carrier rootfs bin directory.
 *
 * @param rootfs_path The path to the carrier rootfs directory.
 * @param components_path The path to the directory containing component
 * binaries.
 *
 * @return - `0` - Indicates successful installation.
 * @return - `-1` - Indicates installation failure.
 */
int install_carrier_components(const char *rootfs_path, const char *components_path);
