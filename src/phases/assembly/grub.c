/**
 * This code is responsible for configuring GRUB for UEFI boot.
 */

#include "all.h"

int setup_grub(const char *rootfs_path)
{
    char grub_dir[COMMAND_PATH_MAX_LENGTH];
    char grub_cfg_path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Configuring GRUB for UEFI boot");

    // Construct the GRUB directory path.
    snprintf(grub_dir, sizeof(grub_dir), "%s/boot/grub", rootfs_path);

    // Create the GRUB directory.
    if (mkdir_p(grub_dir) != 0)
    {
        LOG_ERROR("Failed to create GRUB directory: %s", grub_dir);
        return -1;
    }

    // Construct the grub.cfg path.
    snprintf(grub_cfg_path, sizeof(grub_cfg_path), "%s/grub.cfg", grub_dir);

    // Define the GRUB configuration content.
    const char *grub_cfg =
        "set default=0\n"
        "set timeout_style=hidden\n"
        "set timeout=0\n"
        "\n"
        "menuentry \"" CONFIG_GRUB_MENU_ENTRY_NAME "\" {\n"
        "    linux " CONFIG_BOOT_KERNEL_PATH " " CONFIG_BOOT_KERNEL_PARAMS "\n"
        "    initrd " CONFIG_BOOT_INITRD_PATH "\n"
        "}\n";

    // Write the GRUB configuration file.
    if (write_file(grub_cfg_path, grub_cfg) != 0)
    {
        return -2;
    }

    LOG_INFO("GRUB configured successfully");

    return 0;
}
