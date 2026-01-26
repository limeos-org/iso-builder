#pragma once

/**
 * Runs the assembly phase.
 *
 * Configures GRUB for BIOS and EFI boot, creates a squashfs of
 * the live rootfs, and assembles the final bootable hybrid ISO image.
 *
 * @param rootfs_dir The live rootfs directory.
 * @param version The version string for the ISO filename.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_assembly_phase(const char *rootfs_dir, const char *version);
