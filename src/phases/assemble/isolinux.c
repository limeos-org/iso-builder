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

    // Construct the isolinux.cfg path.
    snprintf(
        isolinux_cfg_path, sizeof(isolinux_cfg_path),
        "%s/isolinux.cfg", isolinux_dir
    );

    // Define the isolinux configuration content.
    const char *isolinux_cfg =
        "DEFAULT limeos\n"
        "PROMPT 0\n"
        "NOESCAPE 1\n"
        "TIMEOUT 1\n"
        "\n"
        "LABEL limeos\n"
        "  KERNEL " CONFIG_BOOT_KERNEL_PATH "\n"
        "  INITRD " CONFIG_BOOT_INITRD_PATH "\n"
        "  APPEND " CONFIG_BOOT_KERNEL_PARAMS "\n";


    // Write the isolinux configuration file.
    if (write_file(isolinux_cfg_path, isolinux_cfg) != 0)
    {
        return -4;
    }

    LOG_INFO("isolinux configured successfully");

    return 0;
}
