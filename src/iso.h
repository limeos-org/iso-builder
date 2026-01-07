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
 * @return - `-1` - Indicates staging directory creation failure.
 * @return - `-2` - Indicates squashfs creation failure.
 * @return - `-3` - Indicates boot files copy failure.
 * @return - `-4` - Indicates EFI image setup failure.
 * @return - `-5` - Indicates ISO assembly failure.
 */
int create_iso(const char *rootfs_path, const char *output_path);
