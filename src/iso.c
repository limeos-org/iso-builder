/**
 * This code is responsible for assembling the final bootable ISO image
 * using xorriso.
 */

#include "all.h"

/** The size of the EFI boot image in megabytes. */
#define EFI_IMAGE_SIZE_MB 4

int create_iso(const char *rootfs_path, const char *output_path)
{
    char command[MAX_COMMAND_LENGTH];
    char staging_path[MAX_PATH_LENGTH];
    char efi_img_path[MAX_PATH_LENGTH];

    LOG_INFO("Creating bootable ISO image...");

    // Construct the staging directory path.
    snprintf(staging_path, sizeof(staging_path), "%s/../iso_staging", rootfs_path);

    // Create the staging directory structure.
    snprintf(command, sizeof(command), "mkdir -p %s/live", staging_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create staging directory");
        return -1;
    }

    // Create squashfs of the rootfs.
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

    // Copy kernel and initrd to staging.
    snprintf(command, sizeof(command), "mkdir -p %s/boot", staging_path);
    run_command(command);

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

    // Copy isolinux files to staging.
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

    // Copy GRUB files to staging.
    snprintf(command, sizeof(command), "mkdir -p %s/boot/grub", staging_path);
    run_command(command);

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

    // Create EFI boot image.
    snprintf(efi_img_path, sizeof(efi_img_path), "%s/boot/grub/efiboot.img", staging_path);

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

    // Format the EFI image as FAT.
    snprintf(command, sizeof(command), "mkfs.fat -F 12 %s", efi_img_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to format EFI image");
        return -1;
    }

    // Create mount point and mount the EFI image.
    snprintf(command, sizeof(command), "mkdir -p %s/efi_mount", staging_path);
    run_command(command);

    snprintf(
        command, sizeof(command),
        "mount -o loop %s %s/efi_mount",
        efi_img_path, staging_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to mount EFI image");
        return -1;
    }

    // Create EFI boot directory structure.
    snprintf(command, sizeof(command), "mkdir -p %s/efi_mount/EFI/BOOT", staging_path);
    run_command(command);

    // Copy GRUB EFI binary.
    snprintf(
        command, sizeof(command),
        "cp /usr/lib/grub/x86_64-efi/monolithic/grubx64.efi %s/efi_mount/EFI/BOOT/BOOTX64.EFI",
        staging_path
    );
    if (run_command(command) != 0)
    {
        LOG_WARNING("Failed to copy GRUB EFI binary, trying grub-mkimage");
        snprintf(
            command, sizeof(command),
            "grub-mkimage -o %s/efi_mount/EFI/BOOT/BOOTX64.EFI -p /boot/grub -O x86_64-efi "
            "normal boot linux part_gpt part_msdos fat iso9660",
            staging_path
        );
        if (run_command(command) != 0)
        {
            LOG_ERROR("Failed to create GRUB EFI image");
            snprintf(command, sizeof(command), "umount %s/efi_mount", staging_path);
            run_command(command);
            return -1;
        }
    }

    // Unmount the EFI image.
    snprintf(command, sizeof(command), "umount %s/efi_mount", staging_path);
    run_command(command);

    // Remove the mount point.
    snprintf(command, sizeof(command), "rmdir %s/efi_mount", staging_path);
    run_command(command);

    // Create the hybrid ISO using xorriso.
    LOG_INFO("Running xorriso to create hybrid ISO...");
    snprintf(
        command, sizeof(command),
        "xorriso -as mkisofs "
        "-o %s "
        "-isohybrid-mbr /usr/lib/ISOLINUX/isohdpfx.bin "
        "-c isolinux/boot.cat "
        "-b isolinux/isolinux.bin "
        "-no-emul-boot -boot-load-size 4 -boot-info-table "
        "-eltorito-alt-boot "
        "-e boot/grub/efiboot.img "
        "-no-emul-boot -isohybrid-gpt-basdat "
        "%s",
        output_path, staging_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create ISO image");
        return -1;
    }

    // Clean up staging directory.
    snprintf(command, sizeof(command), "rm -rf %s", staging_path);
    run_command(command);

    LOG_INFO("ISO created successfully: %s", output_path);

    return 0;
}
