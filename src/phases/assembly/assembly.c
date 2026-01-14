/**
 * This code is responsible for orchestrating the assembly phase.
 */

#include "all.h"

/** Maximum length for the ISO output path. */
#define ISO_OUTPUT_PATH_MAX_LENGTH 256

int run_assembly_phase(const char *rootfs_dir, const char *version)
{
    // Configure GRUB for UEFI boot.
    if (setup_grub(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure GRUB");
        return -1;
    }

    // Configure isolinux for BIOS boot.
    if (setup_isolinux(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure isolinux");
        return -1;
    }

    LOG_INFO("Boot configuration complete");

    // Construct the ISO output path.
    char iso_output_path[ISO_OUTPUT_PATH_MAX_LENGTH];
    snprintf(
        iso_output_path, sizeof(iso_output_path),
        CONFIG_ISO_FILENAME_PREFIX "-%s.iso", version
    );

    // Create the final ISO image.
    if (create_iso(rootfs_dir, iso_output_path) != 0)
    {
        LOG_ERROR("Failed to create ISO image");
        return -1;
    }

    LOG_INFO("Assembly phase complete: ISO created at %s", iso_output_path);
    return 0;
}
