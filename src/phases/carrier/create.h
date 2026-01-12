#pragma once

/**
 * Creates the carrier rootfs by copying from base and installing packages.
 *
 * The carrier rootfs is optimized for running the installer from the ISO.
 * It includes only the packages necessary to boot and run the installation
 * wizard.
 *
 * @param base_path The path to the base rootfs to copy from.
 * @param path The directory where the rootfs will be created.
 *
 * @return - `0` - Indicates successful creation.
 * @return - `-1` - Indicates copy failure.
 * @return - `-2` - Indicates package installation failure.
 * @return - `-3` - Indicates kernel copy failure.
 */
int create_carrier_rootfs(const char *base_path, const char *path);
