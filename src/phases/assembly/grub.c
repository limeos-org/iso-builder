/**
 * This code is responsible for configuring GRUB for both BIOS and UEFI boot.
 */

#include "all.h"

int setup_grub(const char *staging_path)
{
    LOG_INFO("Configuring GRUB for live ISO boot");

    // Construct the GRUB directory path.
    char grub_dir[COMMON_MAX_PATH_LENGTH];
    snprintf(grub_dir, sizeof(grub_dir), "%s/boot/grub", staging_path);

    // Create the GRUB directory.
    if (common.mkdir_p(grub_dir) != 0)
    {
        LOG_ERROR("Failed to create GRUB directory: %s", grub_dir);
        return -1;
    }

    // Construct the grub.cfg path.
    char grub_cfg_path[COMMON_MAX_PATH_LENGTH];
    snprintf(grub_cfg_path, sizeof(grub_cfg_path), "%s/grub.cfg", grub_dir);

    // Define the GRUB configuration content.
    // Switch to graphics terminal immediately to hide the "GRUB" text.
    const char *grub_cfg =
        "# Switch to graphics mode immediately to clear boot text.\n"
        "insmod all_video\n"
        "insmod gfxterm\n"
        "set gfxmode=auto\n"
        "terminal_output gfxterm\n"
        "clear\n"
        "\n"
        "set gfxpayload=keep      # Preserve video mode during kernel handoff.\n"
        "set default=0            # Boot first menu entry.\n"
        "set timeout_style=hidden # Hide the menu completely.\n"
        "set timeout=0            # No delay before booting.\n"
        "\n"
        "menuentry \"" CONFIG_GRUB_MENU_ENTRY_NAME "\" {\n"
        "    linux " CONFIG_BOOT_KERNEL_PATH " " CONFIG_LIVE_KERNEL_PARAMS "\n"
        "    initrd " CONFIG_BOOT_INITRD_PATH "\n"
        "}\n";

    // Write the GRUB configuration file.
    if (common.write_file(grub_cfg_path, grub_cfg) != 0)
    {
        LOG_ERROR("Failed to write GRUB config");
        return -2;
    }

    LOG_INFO("GRUB configured successfully");

    return 0;
}
