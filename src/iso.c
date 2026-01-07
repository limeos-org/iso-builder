/**
 * This code is responsible for assembling the final bootable ISO image
 * using xorriso.
 */

#include "all.h"

/**
 * The size of the EFI boot image in megabytes.
 *
 * 4MB provides sufficient space for the GRUB EFI binary (~1MB) plus
 * headroom for FAT filesystem overhead and future additions.
 */
#define EFI_IMAGE_SIZE_MB 4

/** The path to the MBR template for hybrid ISO. */
#define ISOLINUX_MBR_PATH "/usr/lib/ISOLINUX/isohdpfx.bin"

static int create_staging_directory(const char *staging_path)
{
    char command[MAX_COMMAND_LENGTH];

    snprintf(command, sizeof(command), "mkdir -p %s/live", staging_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create staging directory");
        return -1;
    }

    return 0;
}

static int create_squashfs(const char *rootfs_path, const char *staging_path)
{
    char command[MAX_COMMAND_LENGTH];

    // Use xz compression for best compression ratio on live filesystem.
    LOG_INFO("Creating squashfs filesystem...");
    snprintf(
        command, sizeof(command),
        "mksquashfs %s %s/live/filesystem.squashfs -comp xz -noappend",
        rootfs_path, staging_path
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
    char command[MAX_COMMAND_LENGTH];

    // Create boot directory.
    snprintf(command, sizeof(command), "mkdir -p %s/boot/grub", staging_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create boot directory");
        return -1;
    }

    // Copy kernel.
    snprintf(
        command, sizeof(command),
        "cp %s/boot/vmlinuz %s/boot/vmlinuz",
        rootfs_path, staging_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy kernel");
        return -1;
    }

    // Copy initrd.
    snprintf(
        command, sizeof(command),
        "cp %s/boot/initrd.img %s/boot/initrd.img",
        rootfs_path, staging_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy initrd");
        return -1;
    }

    // Copy isolinux files.
    snprintf(
        command, sizeof(command),
        "cp -r %s/isolinux %s/",
        rootfs_path, staging_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy isolinux");
        return -1;
    }

    // Copy GRUB config.
    snprintf(
        command, sizeof(command),
        "cp %s/boot/grub/grub.cfg %s/boot/grub/",
        rootfs_path, staging_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy GRUB config");
        return -1;
    }

    return 0;
}

static int setup_efi_image(const char *staging_path)
{
    char command[MAX_COMMAND_LENGTH];
    char efi_img_path[MAX_PATH_LENGTH];
    char mount_path[MAX_PATH_LENGTH];

    snprintf(efi_img_path, sizeof(efi_img_path), "%s/boot/grub/efiboot.img", staging_path);
    snprintf(mount_path, sizeof(mount_path), "%s/efi_mount", staging_path);

    // Create empty EFI image.
    snprintf(
        command, sizeof(command),
        "dd if=/dev/zero of=%s bs=1M count=%d",
        efi_img_path, EFI_IMAGE_SIZE_MB
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create EFI image");
        return -1;
    }

    // Format as FAT12, appropriate for small (<16MB) EFI system partitions.
    snprintf(command, sizeof(command), "mkfs.fat -F 12 %s", efi_img_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to format EFI image");
        return -1;
    }

    // Mount the EFI image.
    snprintf(command, sizeof(command), "mkdir -p %s", mount_path);
    run_command(command);

    snprintf(command, sizeof(command), "mount -o loop %s %s", efi_img_path, mount_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to mount EFI image");
        return -1;
    }

    // Create EFI boot directory.
    snprintf(command, sizeof(command), "mkdir -p %s/EFI/BOOT", mount_path);
    run_command(command);

    // Copy GRUB EFI binary.
    snprintf(
        command, sizeof(command),
        "cp /usr/lib/grub/x86_64-efi/monolithic/grubx64.efi %s/EFI/BOOT/BOOTX64.EFI",
        mount_path
    );
    if (run_command(command) != 0)
    {
        LOG_WARNING("Failed to copy GRUB EFI binary, trying grub-mkimage");
        snprintf(
            command, sizeof(command),
            "grub-mkimage -o %s/EFI/BOOT/BOOTX64.EFI -p /boot/grub -O x86_64-efi "
            "normal boot linux part_gpt part_msdos fat iso9660",
            mount_path
        );
        if (run_command(command) != 0)
        {
            LOG_ERROR("Failed to create GRUB EFI image");
            snprintf(command, sizeof(command), "umount %s", mount_path);
            run_command(command);
            return -1;
        }
    }

    // Unmount and clean up.
    snprintf(command, sizeof(command), "umount %s", mount_path);
    run_command(command);

    snprintf(command, sizeof(command), "rmdir %s", mount_path);
    run_command(command);

    return 0;
}

static int assemble_iso(const char *staging_path, const char *output_path)
{
    char command[MAX_COMMAND_LENGTH];

    // Build hybrid ISO supporting both BIOS (isolinux) and UEFI (EFI image) boot.
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
        output_path, ISOLINUX_MBR_PATH, staging_path
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
    char command[MAX_COMMAND_LENGTH];

    snprintf(command, sizeof(command), "rm -rf %s", staging_path);
    if (run_command(command) != 0)
    {
        LOG_WARNING("Failed to clean up staging directory: %s", staging_path);
    }
}

int create_iso(const char *rootfs_path, const char *output_path)
{
    char staging_path[MAX_PATH_LENGTH];

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
    if (assemble_iso(staging_path, output_path) != 0)
    {
        cleanup_staging(staging_path);
        return -5;
    }

    // Clean up staging directory.
    cleanup_staging(staging_path);

    LOG_INFO("ISO created successfully: %s", output_path);

    return 0;
}
