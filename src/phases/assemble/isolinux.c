/**
 * This code is responsible for configuring isolinux for legacy BIOS boot.
 */

#include "all.h"

int setup_isolinux(const char *rootfs_path)
{
    char isolinux_dir[COMMAND_PATH_MAX_LENGTH];
    char isolinux_cfg_path[COMMAND_PATH_MAX_LENGTH];
    char dst_path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Configuring isolinux for BIOS boot");

    // Construct the isolinux directory path.
    snprintf(isolinux_dir, sizeof(isolinux_dir), "%s/isolinux", rootfs_path);

    // Create the isolinux directory.
    if (mkdir_p(isolinux_dir) != 0)
    {
        LOG_ERROR("Failed to create isolinux directory: %s", isolinux_dir);
        return -1;
    }

    // Copy isolinux.bin from the system.
    snprintf(dst_path, sizeof(dst_path), "%s/isolinux.bin", isolinux_dir);
    if (copy_file(CONFIG_ISOLINUX_BIN_PATH, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy isolinux.bin");
        return -2;
    }

    // Copy ldlinux.c32 from the system.
    snprintf(dst_path, sizeof(dst_path), "%s/ldlinux.c32", isolinux_dir);
    if (copy_file(CONFIG_LDLINUX_PATH, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy ldlinux.c32");
        return -3;
    }

    // Copy vesamenu.c32 from the system for clean graphical boot.
    snprintf(dst_path, sizeof(dst_path), "%s/vesamenu.c32", isolinux_dir);
    if (copy_file(CONFIG_VESAMENU_C32_PATH, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy vesamenu.c32");
        return -4;
    }

    // Copy libutil.c32 (required dependency for vesamenu.c32).
    snprintf(dst_path, sizeof(dst_path), "%s/libutil.c32", isolinux_dir);
    if (copy_file(CONFIG_LIBUTIL_C32_PATH, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy libutil.c32");
        return -5;
    }

    // Copy libcom32.c32 (required dependency for vesamenu.c32).
    snprintf(dst_path, sizeof(dst_path), "%s/libcom32.c32", isolinux_dir);
    if (copy_file(CONFIG_LIBCOM32_C32_PATH, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy libcom32.c32");
        return -6;
    }

    // Copy black background image for clean boot.
    snprintf(dst_path, sizeof(dst_path), "%s/black.png", isolinux_dir);
    if (copy_file(CONFIG_BLACK_PNG_PATH, dst_path) != 0)
    {
        LOG_ERROR("Failed to copy black.png");
        return -7;
    }

    // Construct the isolinux.cfg path.
    snprintf(
        isolinux_cfg_path, sizeof(isolinux_cfg_path),
        "%s/isolinux.cfg", isolinux_dir
    );

    // Define the isolinux configuration content.
    // vesamenu.c32 with black background image for completely clean boot.
    // TIMEOUT 1 = 0.1 seconds (minimum, since 0 means wait forever).
    const char *isolinux_cfg =
        "UI vesamenu.c32\n"
        "DEFAULT limeos\n"
        "PROMPT 0\n"
        "TIMEOUT 1\n"
        "TOTALTIMEOUT 1\n"
        "MENU HIDDEN\n"
        "MENU BACKGROUND black.png\n"
        "MENU COLOR screen 0 #00000000 #00000000 none\n"
        "MENU COLOR border 0 #00000000 #00000000 none\n"
        "MENU COLOR title 0 #00000000 #00000000 none\n"
        "MENU COLOR unsel 0 #00000000 #00000000 none\n"
        "MENU COLOR sel 0 #00000000 #00000000 none\n"
        "MENU COLOR hotsel 0 #00000000 #00000000 none\n"
        "MENU COLOR hotkey 0 #00000000 #00000000 none\n"
        "MENU COLOR timeout_msg 0 #00000000 #00000000 none\n"
        "MENU COLOR timeout 0 #00000000 #00000000 none\n"
        "\n"
        "LABEL limeos\n"
        "  KERNEL " CONFIG_BOOT_KERNEL_PATH "\n"
        "  INITRD " CONFIG_BOOT_INITRD_PATH "\n"
        "  APPEND " CONFIG_BOOT_KERNEL_PARAMS "\n";

    // Write the isolinux configuration file.
    if (write_file(isolinux_cfg_path, isolinux_cfg) != 0)
    {
        return -8;
    }

    LOG_INFO("isolinux configured successfully");

    return 0;
}
