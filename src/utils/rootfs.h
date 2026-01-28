#pragma once
#include "../all.h"

/**
 * Cleans apt cache and lists directories in a rootfs.
 *
 * Removes /var/cache/apt and /var/lib/apt/lists, then recreates the
 * empty directories. This reduces rootfs size while keeping apt functional.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates apt cache removal failure.
 * @return - `-2` - Indicates apt lists removal failure.
 */
int cleanup_apt_directories(const char *rootfs_path);

/**
 * Copies kernel and initrd to standard boot paths.
 *
 * Finds vmlinuz-* and initrd.img-* using glob patterns and copies them
 * to /boot/vmlinuz and /boot/initrd.img respectively. The versioned
 * source files are preserved for update-initramfs; use
 * cleanup_versioned_boot_files() to remove them after Plymouth theming.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Kernel not found.
 * @return - `-2` - Kernel copy failed.
 * @return - `-3` - Initrd not found.
 * @return - `-4` - Initrd copy failed.
 */
int copy_kernel_and_initrd(const char *rootfs_path);

/**
 * Removes all versioned boot files from a rootfs.
 *
 * Removes vmlinuz-*, initrd.img-*, config-*, and System.map-* files
 * from the /boot directory. Use this after regenerating initramfs to
 * clean up old versions that accumulate.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates path quoting failure.
 */
int cleanup_versioned_boot_files(const char *rootfs_path);

