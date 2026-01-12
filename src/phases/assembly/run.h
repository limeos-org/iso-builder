#pragma once

/**
 * Runs the assembly phase.
 *
 * Configures bootloaders (GRUB, isolinux), sets up the splash screen,
 * and creates the final bootable hybrid ISO image.
 *
 * @param rootfs_dir The carrier rootfs directory.
 * @param version The version string for the ISO filename.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_assembly_phase(const char *rootfs_dir, const char *version);
