/**
 * This code is responsible for assembling the final bootable ISO image.
 */

#include "all.h"

static int create_staging_directory(const char *staging_path)
{
    char live_path[COMMAND_PATH_MAX_LENGTH];

    snprintf(live_path, sizeof(live_path), "%s/live", staging_path);
    if (mkdir_p(live_path) != 0)
    {
        LOG_ERROR("Failed to create staging directory");
        return -1;
    }

    return 0;
}

static int create_squashfs(const char *rootfs_path, const char *staging_path)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_rootfs[COMMAND_QUOTED_MAX_LENGTH];
    char squashfs_path[COMMAND_PATH_MAX_LENGTH];
    char quoted_squashfs[COMMAND_QUOTED_MAX_LENGTH];

    // Use xz compression for best compression ratio on live filesystem.
    LOG_INFO("Creating squashfs filesystem...");

    snprintf(squashfs_path, sizeof(squashfs_path), "%s/live/filesystem.squashfs", staging_path);

    if (shell_quote_path(rootfs_path, quoted_rootfs, sizeof(quoted_rootfs)) != 0)
    {
        LOG_ERROR("Failed to quote rootfs path");
        return -1;
    }
    if (shell_quote_path(squashfs_path, quoted_squashfs, sizeof(quoted_squashfs)) != 0)
    {
        LOG_ERROR("Failed to quote squashfs path");
        return -1;
    }

    snprintf(
        command, sizeof(command),
        "mksquashfs %s %s -comp xz -noappend",
        quoted_rootfs, quoted_squashfs
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create squashfs");
        return -1;
    }

    return 0;
}

static int copy_boot_files(const char *rootfs_path, const char *staging_path)
{
    char src_path[COMMAND_PATH_MAX_LENGTH];
    char dst_path[COMMAND_PATH_MAX_LENGTH];

    // Create boot directory.
    snprintf(dst_path, sizeof(dst_path), "%s/boot/grub", staging_path);
    if (mkdir_p(dst_path) != 0)
    {
        LOG_ERROR("Failed to create boot directory");
        return -1;
    }

    // Copy kernel.
    snprintf(src_path, sizeof(src_path), "%s/boot/vmlinuz", rootfs_path);
    snprintf(dst_path, sizeof(dst_path), "%s/boot/vmlinuz", staging_path);
    if (copy_file(src_path, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy kernel");
        return -1;
    }

    // Copy initrd.
    snprintf(src_path, sizeof(src_path), "%s/boot/initrd.img", rootfs_path);
    snprintf(dst_path, sizeof(dst_path), "%s/boot/initrd.img", staging_path);
    if (copy_file(src_path, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy initrd");
        return -1;
    }

    // Copy isolinux files.
    char command[COMMAND_MAX_LENGTH];
    char quoted_src[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_dst[COMMAND_QUOTED_MAX_LENGTH];

    snprintf(src_path, sizeof(src_path), "%s/isolinux", rootfs_path);
    if (shell_quote_path(src_path, quoted_src, sizeof(quoted_src)) != 0)
    {
        LOG_ERROR("Failed to quote isolinux source path");
        return -1;
    }
    if (shell_quote_path(staging_path, quoted_dst, sizeof(quoted_dst)) != 0)
    {
        LOG_ERROR("Failed to quote staging path");
        return -1;
    }
    snprintf(command, sizeof(command), "cp -r %s %s/", quoted_src, quoted_dst);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy isolinux");
        return -1;
    }

    // Copy GRUB config.
    snprintf(src_path, sizeof(src_path), "%s/boot/grub/grub.cfg", rootfs_path);
    snprintf(dst_path, sizeof(dst_path), "%s/boot/grub/grub.cfg", staging_path);
    if (copy_file(src_path, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy GRUB config");
        return -1;
    }

    return 0;
}

static int setup_efi_image(const char *staging_path)
{
    char command[COMMAND_MAX_LENGTH];
    char efi_img_path[COMMAND_PATH_MAX_LENGTH];
    char mount_path[COMMAND_PATH_MAX_LENGTH];
    char efi_boot_dir[COMMAND_PATH_MAX_LENGTH];
    char efi_binary_dst[COMMAND_PATH_MAX_LENGTH];
    char quoted_efi_img[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_mount[COMMAND_QUOTED_MAX_LENGTH];

    snprintf(efi_img_path, sizeof(efi_img_path), "%s/boot/grub/efiboot.img", staging_path);
    snprintf(mount_path, sizeof(mount_path), "%s/efi_mount", staging_path);

    if (shell_quote_path(efi_img_path, quoted_efi_img, sizeof(quoted_efi_img)) != 0)
    {
        LOG_ERROR("Failed to quote EFI image path");
        return -1;
    }
    if (shell_quote_path(mount_path, quoted_mount, sizeof(quoted_mount)) != 0)
    {
        LOG_ERROR("Failed to quote mount path");
        return -1;
    }

    // Create empty EFI image.
    snprintf(
        command, sizeof(command),
        "dd if=/dev/zero of=%s bs=1M count=%d",
        quoted_efi_img, CONFIG_EFI_IMAGE_SIZE_MB
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create EFI image");
        return -1;
    }

    // Format as FAT12, appropriate for small (<16MB) EFI system partitions.
    snprintf(command, sizeof(command), "mkfs.fat -F 12 %s", quoted_efi_img);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to format EFI image");
        return -1;
    }

    // Mount the EFI image.
    if (mkdir_p(mount_path) != 0)
    {
        LOG_WARNING("Failed to create EFI mount directory: %s", mount_path);
    }

    snprintf(command, sizeof(command), "mount -o loop %s %s", quoted_efi_img, quoted_mount);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to mount EFI image");
        return -1;
    }

    // Create EFI boot directory.
    snprintf(efi_boot_dir, sizeof(efi_boot_dir), "%s/EFI/BOOT", mount_path);
    if (mkdir_p(efi_boot_dir) != 0)
    {
        LOG_WARNING("Failed to create EFI boot directory");
    }

    // Copy GRUB EFI binary.
    snprintf(efi_binary_dst, sizeof(efi_binary_dst), "%s/EFI/BOOT/BOOTX64.EFI", mount_path);
    if (copy_file(CONFIG_GRUB_EFI_PATH, efi_binary_dst) != 0)
    {
        LOG_WARNING("Failed to copy GRUB EFI binary, trying grub-mkimage");

        char quoted_efi_dst[COMMAND_QUOTED_MAX_LENGTH];
        if (shell_quote_path(efi_binary_dst, quoted_efi_dst, sizeof(quoted_efi_dst)) != 0)
        {
            LOG_ERROR("Failed to quote EFI binary destination path");
            snprintf(command, sizeof(command), "umount %s", quoted_mount);
            run_command(command);
            return -1;
        }

        snprintf(
            command, sizeof(command),
            "grub-mkimage -o %s -p /boot/grub -O x86_64-efi "
            "normal boot linux part_gpt part_msdos fat iso9660",
            quoted_efi_dst
        );
        if (run_command(command) != 0)
        {
            LOG_ERROR("Failed to create GRUB EFI image");
            snprintf(command, sizeof(command), "umount %s", quoted_mount);
            if (run_command(command) != 0)
            {
                LOG_WARNING("Failed to unmount EFI image on error: %s", mount_path);
            }
            return -1;
        }
    }

    // Unmount and clean up.
    snprintf(command, sizeof(command), "umount %s", quoted_mount);
    if (run_command(command) != 0)
    {
        LOG_WARNING("Failed to unmount EFI image: %s", mount_path);
    }

    snprintf(command, sizeof(command), "rmdir %s", quoted_mount);
    if (run_command(command) != 0)
    {
        LOG_WARNING("Failed to remove EFI mount directory: %s", mount_path);
    }

    return 0;
}

static int run_xorriso(const char *staging_path, const char *output_path)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_output[COMMAND_QUOTED_MAX_LENGTH];
    char quoted_staging[COMMAND_QUOTED_MAX_LENGTH];

    if (shell_quote_path(output_path, quoted_output, sizeof(quoted_output)) != 0)
    {
        LOG_ERROR("Failed to quote output path");
        return -1;
    }
    if (shell_quote_path(staging_path, quoted_staging, sizeof(quoted_staging)) != 0)
    {
        LOG_ERROR("Failed to quote staging path");
        return -1;
    }

    // Build hybrid ISO supporting both BIOS (isolinux) and UEFI (EFI image)
    // boot.
    // -boot-load-size 4: Load 4 sectors (2KB) of boot image per El Torito spec.
    // -isohybrid-gpt-basdat: Mark EFI partition as basic data for GPT systems.
    LOG_INFO("Running xorriso to create hybrid ISO...");
    snprintf(
        command, sizeof(command),
        "xorriso -as mkisofs "
        "-o %s "
        "-isohybrid-mbr %s "
        "-c isolinux/boot.cat "
        "-b isolinux/isolinux.bin "
        "-no-emul-boot -boot-load-size 4 -boot-info-table "
        "-eltorito-alt-boot "
        "-e boot/grub/efiboot.img "
        "-no-emul-boot -isohybrid-gpt-basdat "
        "%s",
        quoted_output, CONFIG_ISOLINUX_MBR_PATH, quoted_staging
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create ISO image");
        return -1;
    }

    return 0;
}

static void cleanup_staging(const char *staging_path)
{
    if (rm_rf(staging_path) != 0)
    {
        LOG_WARNING("Failed to clean up staging directory: %s", staging_path);
    }
}

int create_iso(const char *rootfs_path, const char *output_path)
{
    char staging_path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Creating bootable ISO image...");

    // Construct the staging directory path.
    snprintf(staging_path, sizeof(staging_path), "%s/../iso_staging", rootfs_path);

    // Create staging directory.
    if (create_staging_directory(staging_path) != 0)
    {
        return -1;
    }

    // Create squashfs filesystem.
    if (create_squashfs(rootfs_path, staging_path) != 0)
    {
        cleanup_staging(staging_path);
        return -2;
    }

    // Copy boot files to staging.
    if (copy_boot_files(rootfs_path, staging_path) != 0)
    {
        cleanup_staging(staging_path);
        return -3;
    }

    // Set up EFI boot image.
    if (setup_efi_image(staging_path) != 0)
    {
        cleanup_staging(staging_path);
        return -4;
    }

    // Assemble the final ISO.
    if (run_xorriso(staging_path, output_path) != 0)
    {
        cleanup_staging(staging_path);
        return -5;
    }

    // Clean up staging directory.
    cleanup_staging(staging_path);

    LOG_INFO("ISO created successfully: %s", output_path);

    return 0;
}
