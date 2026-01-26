/**
 * This code is responsible for assembling the final bootable ISO image.
 */

#include "all.h"

/** Squashfs compression. xz provides best ratio for live systems. */
#define SQUASHFS_COMPRESSION "xz"

/** Maximum cleanup retry attempts before giving up. */
#define CLEANUP_MAX_RETRIES 3

/** Seconds to wait between cleanup retries. */
#define CLEANUP_RETRY_DELAY_SECONDS 1

static int create_staging_directory(const char *staging_path)
{
    // Construct the live directory path inside staging.
    char live_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(live_path, sizeof(live_path), "%s/live", staging_path);

    // Create the live directory structure.
    if (mkdir_p(live_path) != 0)
    {
        LOG_ERROR("Failed to create staging directory");
        return -1;
    }

    return 0;
}

static int create_squashfs(const char *rootfs_path, const char *staging_path)
{
    LOG_INFO("Creating squashfs filesystem...");

    // Construct the squashfs output path.
    char squashfs_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(squashfs_path, sizeof(squashfs_path), "%s/live/filesystem.squashfs", staging_path);

    // Quote the rootfs path for shell safety.
    char quoted_rootfs[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(rootfs_path, quoted_rootfs, sizeof(quoted_rootfs)) != 0)
    {
        LOG_ERROR("Failed to quote rootfs path");
        return -1;
    }

    // Quote the squashfs path for shell safety.
    char quoted_squashfs[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(squashfs_path, quoted_squashfs, sizeof(quoted_squashfs)) != 0)
    {
        LOG_ERROR("Failed to quote squashfs path");
        return -1;
    }

    // Create the squashfs filesystem.
    char command[COMMAND_MAX_LENGTH];
    snprintf(
        command, sizeof(command),
        "mksquashfs %s %s -comp " SQUASHFS_COMPRESSION " -noappend",
        quoted_rootfs, quoted_squashfs
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create squashfs from %s", rootfs_path);
        return -1;
    }

    return 0;
}

static int copy_boot_files(const char *rootfs_path, const char *staging_path)
{
    char src_path[COMMAND_PATH_MAX_LENGTH];
    char dst_path[COMMAND_PATH_MAX_LENGTH];

    // Create the boot directory in staging.
    snprintf(dst_path, sizeof(dst_path), "%s/boot", staging_path);
    if (mkdir_p(dst_path) != 0)
    {
        LOG_ERROR("Failed to create boot directory");
        return -1;
    }

    // Copy the kernel to staging.
    snprintf(src_path, sizeof(src_path), "%s/boot/vmlinuz", rootfs_path);
    snprintf(dst_path, sizeof(dst_path), "%s/boot/vmlinuz", staging_path);
    if (copy_file(src_path, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy kernel");
        return -1;
    }

    // Copy the initrd to staging.
    snprintf(src_path, sizeof(src_path), "%s/boot/initrd.img", rootfs_path);
    snprintf(dst_path, sizeof(dst_path), "%s/boot/initrd.img", staging_path);
    if (copy_file(src_path, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy initrd");
        return -1;
    }

    return 0;
}

static int run_grub_mkrescue(const char *staging_path, const char *output_path)
{
    LOG_INFO("Running grub-mkrescue to create hybrid ISO...");

    // Quote paths for shell safety.
    char quoted_staging[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(staging_path, quoted_staging, sizeof(quoted_staging)) != 0)
    {
        LOG_ERROR("Failed to quote staging path");
        return -1;
    }
    char quoted_output[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(output_path, quoted_output, sizeof(quoted_output)) != 0)
    {
        LOG_ERROR("Failed to quote output path");
        return -1;
    }

    // Build hybrid ISO supporting both BIOS and UEFI boot.
    // grub-mkrescue handles all boot image creation automatically.
    char command[COMMAND_MAX_LENGTH];
    snprintf(
        command, sizeof(command),
        "grub-mkrescue "
        "-o %s "            // Output ISO file path.
        "--locales=\"\" "   // Skip locales (reduce size).
        "--fonts=\"\" "     // Skip fonts (hidden menu anyway).
        "--themes=\"\" "    // Skip themes.
        "%s",               // Source directory (staging).
        quoted_output, quoted_staging
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create ISO image: %s", output_path);
        return -1;
    }

    return 0;
}

static void cleanup_staging(const char *staging_path)
{
    // Attempt to remove the staging directory with retries.
    for (int attempt = 1; attempt <= CLEANUP_MAX_RETRIES; attempt++)
    {
        // Try to remove the staging directory.
        if (rm_rf(staging_path) == 0)
        {
            return;
        }

        // Wait briefly before retrying (files may be temporarily locked).
        if (attempt < CLEANUP_MAX_RETRIES)
        {
            LOG_WARNING("Cleanup attempt %d failed, retrying...", attempt);
            sleep(CLEANUP_RETRY_DELAY_SECONDS);
        }
    }

    // Log warning if cleanup failed after all retries.
    LOG_WARNING(
        "Failed to clean up staging directory after %d attempts: %s",
        CLEANUP_MAX_RETRIES, staging_path
    );
}

static void cleanup_live_boot(const char *rootfs_path)
{
    char path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Removing boot files from live rootfs...");

    // Remove versioned boot files (vmlinuz-*, initrd.img-*).
    cleanup_versioned_boot_files(rootfs_path);

    // Remove the generic kernel symlink.
    snprintf(path, sizeof(path), "%s/boot/vmlinuz", rootfs_path);
    rm_file(path);

    // Remove the generic initrd symlink.
    snprintf(path, sizeof(path), "%s/boot/initrd.img", rootfs_path);
    rm_file(path);
}

int create_iso(const char *rootfs_path, const char *output_path)
{
    LOG_INFO("Creating bootable ISO image...");

    // Construct the staging directory path adjacent to rootfs.
    char staging_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(staging_path, sizeof(staging_path), "%s/../staging-iso", rootfs_path);

    // Create the staging directory structure.
    if (create_staging_directory(staging_path) != 0)
    {
        return -1;
    }

    // Copy boot files to staging before cleanup removes them from rootfs.
    if (copy_boot_files(rootfs_path, staging_path) != 0)
    {
        cleanup_staging(staging_path);
        return -2;
    }

    // Configure GRUB bootloader in the staging directory.
    if (setup_grub(staging_path) != 0)
    {
        cleanup_staging(staging_path);
        return -3;
    }

    // Remove boot files from live rootfs to reduce squashfs size (~100MB).
    cleanup_live_boot(rootfs_path);

    // Create the squashfs filesystem from the live rootfs.
    if (create_squashfs(rootfs_path, staging_path) != 0)
    {
        cleanup_staging(staging_path);
        return -4;
    }

    // Assemble the final hybrid ISO with grub-mkrescue.
    if (run_grub_mkrescue(staging_path, output_path) != 0)
    {
        cleanup_staging(staging_path);
        return -5;
    }

    // Clean up the staging directory after successful ISO creation.
    cleanup_staging(staging_path);

    LOG_INFO("ISO created successfully: %s", output_path);

    return 0;
}
