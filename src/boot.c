/**
 * This code is responsible for configuring boot loaders and splash screen
 * for the LimeOS installer ISO.
 */

#include "all.h"

/** The kernel command line parameters for boot. */
#define BOOT_KERNEL_PARAMS "boot=live quiet splash loglevel=0"

/** The default kernel image path within the ISO. */
#define BOOT_KERNEL_PATH "/boot/vmlinuz"

/** The default initrd image path within the ISO. */
#define BOOT_INITRD_PATH "/boot/initrd.img"

static int write_file(const char *path, const char *content)
{
    // Open the file for writing.
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        LOG_ERROR("Failed to create file %s: %s", path, strerror(errno));
        return -1;
    }

    // Write the content to the file.
    if (fputs(content, file) == EOF)
    {
        LOG_ERROR("Failed to write to file %s", path);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int setup_grub(const char *rootfs_path)
{
    char grub_dir[MAX_PATH_LENGTH];
    char grub_cfg_path[MAX_PATH_LENGTH];

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
        "set timeout=0\n"
        "set default=0\n"
        "\n"
        "menuentry \"LimeOS Installer\" {\n"
        "    linux " BOOT_KERNEL_PATH " " BOOT_KERNEL_PARAMS "\n"
        "    initrd " BOOT_INITRD_PATH "\n"
        "}\n";

    // Write the GRUB configuration file.
    if (write_file(grub_cfg_path, grub_cfg) != 0)
    {
        return -1;
    }

    LOG_INFO("GRUB configured successfully");

    return 0;
}

int setup_isolinux(const char *rootfs_path)
{
    char isolinux_dir[MAX_PATH_LENGTH];
    char isolinux_cfg_path[MAX_PATH_LENGTH];
    char command[MAX_COMMAND_LENGTH];

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
    snprintf(
        command, sizeof(command),
        "cp /usr/lib/ISOLINUX/isolinux.bin %s/",
        isolinux_dir
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy isolinux.bin");
        return -1;
    }

    // Copy ldlinux.c32 from the system.
    snprintf(
        command, sizeof(command),
        "cp /usr/lib/syslinux/modules/bios/ldlinux.c32 %s/",
        isolinux_dir
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to copy ldlinux.c32");
        return -1;
    }

    // Construct the isolinux.cfg path.
    snprintf(
        isolinux_cfg_path, sizeof(isolinux_cfg_path),
        "%s/isolinux.cfg", isolinux_dir
    );

    // Define the isolinux configuration content.
    const char *isolinux_cfg =
        "DEFAULT limeos\n"
        "TIMEOUT 0\n"
        "PROMPT 0\n"
        "\n"
        "LABEL limeos\n"
        "    MENU LABEL LimeOS Installer\n"
        "    KERNEL " BOOT_KERNEL_PATH "\n"
        "    APPEND initrd=" BOOT_INITRD_PATH " " BOOT_KERNEL_PARAMS "\n";

    // Write the isolinux configuration file.
    if (write_file(isolinux_cfg_path, isolinux_cfg) != 0)
    {
        return -1;
    }

    LOG_INFO("isolinux configured successfully");

    return 0;
}

int setup_splash(const char *rootfs_path, const char *logo_path)
{
    char theme_dir[MAX_PATH_LENGTH];
    char theme_file_path[MAX_PATH_LENGTH];
    char splash_dest[MAX_PATH_LENGTH];
    char command[MAX_COMMAND_LENGTH];

    LOG_INFO("Configuring Plymouth splash screen");

    // Verify the logo file exists.
    if (!file_exists(logo_path))
    {
        LOG_ERROR("Splash logo not found: %s", logo_path);
        return -1;
    }

    // Construct the Plymouth theme directory path.
    snprintf(
        theme_dir, sizeof(theme_dir),
        "%s/usr/share/plymouth/themes/limeos", rootfs_path
    );

    // Create the Plymouth theme directory.
    if (mkdir_p(theme_dir) != 0)
    {
        LOG_ERROR("Failed to create Plymouth theme directory: %s", theme_dir);
        return -1;
    }

    // Construct the destination path for the splash image.
    snprintf(splash_dest, sizeof(splash_dest), "%s/splash.png", theme_dir);

    // Copy the logo to the theme directory.
    if (copy_file(logo_path, splash_dest) != 0)
    {
        LOG_ERROR("Failed to copy splash logo");
        return -1;
    }

    // Construct the theme file path.
    snprintf(theme_file_path, sizeof(theme_file_path), "%s/limeos.plymouth", theme_dir);

    // Define the Plymouth theme configuration.
    const char *theme_cfg =
        "[Plymouth Theme]\n"
        "Name=LimeOS\n"
        "Description=LimeOS boot splash\n"
        "ModuleName=script\n"
        "\n"
        "[script]\n"
        "ImageDir=/usr/share/plymouth/themes/limeos\n"
        "ScriptFile=/usr/share/plymouth/themes/limeos/limeos.script\n";

    // Write the Plymouth theme file.
    if (write_file(theme_file_path, theme_cfg) != 0)
    {
        return -1;
    }

    // Construct the script file path.
    char script_path[MAX_PATH_LENGTH];
    snprintf(script_path, sizeof(script_path), "%s/limeos.script", theme_dir);

    // Define the Plymouth script.
    const char *script_cfg =
        "Window.SetBackgroundTopColor(0, 0, 0);\n"
        "Window.SetBackgroundBottomColor(0, 0, 0);\n"
        "splash_image = Image(\"splash.png\");\n"
        "sprite = Sprite(splash_image);\n"
        "sprite.SetX(Window.GetWidth() / 2 - splash_image.GetWidth() / 2);\n"
        "sprite.SetY(Window.GetHeight() / 2 - splash_image.GetHeight() / 2);\n";

    // Write the Plymouth script file.
    if (write_file(script_path, script_cfg) != 0)
    {
        return -1;
    }

    // Set LimeOS as the default Plymouth theme.
    snprintf(
        command, sizeof(command),
        "chroot %s plymouth-set-default-theme limeos",
        rootfs_path
    );
    if (run_command(command) != 0)
    {
        LOG_WARNING("Failed to set Plymouth theme (plymouth may not be installed)");
    }

    // Regenerate initramfs to include the new Plymouth theme.
    LOG_INFO("Regenerating initramfs with new theme...");
    snprintf(
        command, sizeof(command),
        "chroot %s update-initramfs -u",
        rootfs_path
    );
    if (run_command(command) != 0)
    {
        LOG_WARNING("Failed to regenerate initramfs");
    }

    // Re-copy the updated initrd.
    snprintf(
        command, sizeof(command),
        "cp $(ls %s/boot/initrd.img-* | head -1) %s/boot/initrd.img",
        rootfs_path, rootfs_path
    );
    run_command(command);

    LOG_INFO("Plymouth splash configured successfully");

    return 0;
}
