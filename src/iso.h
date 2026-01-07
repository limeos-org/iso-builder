#pragma once

/**
 * Creates a hybrid bootable ISO image from the root filesystem.
 *
 * Uses xorriso to create an ISO that supports both UEFI and legacy BIOS boot.
 *
 * @param rootfs_path The path to the prepared root filesystem directory.
 * @param output_path The path where the ISO file will be created.
 *
 * @return - `0` - Indicates successful ISO creation.
 * @return - `-1` - Indicates ISO creation failure.
 */
int create_iso(const char *rootfs_path, const char *output_path);
