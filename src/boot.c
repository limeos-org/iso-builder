/**
 * This code is responsible for configuring boot loaders and splash screen
 * for the LimeOS installer ISO.
 */

#include "all.h"
#include <glob.h>

/**
 * Finds the first file matching a glob pattern.
 *
 * @param pattern The glob pattern to match.
 * @param out_path The buffer to store the matched path.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates a match was found.
 * @return - `-1` - Indicates no match was found.
 */
static int find_first_glob(
    const char *pattern,
    char *out_path,
    size_t buffer_length
)
{
    glob_t glob_result;
    int ret;

    ret = glob(pattern, GLOB_NOSORT, NULL, &glob_result);
    if (ret != 0 || glob_result.gl_pathc == 0)
    {
        globfree(&glob_result);
        return -1;
    }

    strncpy(out_path, glob_result.gl_pathv[0], buffer_length - 1);
    out_path[buffer_length - 1] = '\0';

    globfree(&glob_result);
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

int setup_isolinux(const char *rootfs_path)
{
    char isolinux_dir[MAX_PATH_LENGTH];
    char isolinux_cfg_path[MAX_PATH_LENGTH];
    char dst_path[MAX_PATH_LENGTH];

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
        "TIMEOUT 0\n"
        "PROMPT 0\n"
        "\n"
        "LABEL limeos\n"
        "    MENU LABEL LimeOS Installer\n"
        "    KERNEL " CONFIG_BOOT_KERNEL_PATH "\n"
        "    APPEND initrd=" CONFIG_BOOT_INITRD_PATH " " CONFIG_BOOT_KERNEL_PARAMS "\n";

    // Write the isolinux configuration file.
    if (write_file(isolinux_cfg_path, isolinux_cfg) != 0)
    {
        return -4;
    }

    LOG_INFO("isolinux configured successfully");

    return 0;
}

int setup_splash(const char *rootfs_path, const char *logo_path)
{
    char theme_dir[MAX_PATH_LENGTH];
    char theme_file_path[MAX_PATH_LENGTH];
    char splash_dest[MAX_PATH_LENGTH];
    char theme_cmd[MAX_PATH_LENGTH];

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
        "%s" CONFIG_PLYMOUTH_THEMES_DIR "/" CONFIG_PLYMOUTH_THEME_NAME, rootfs_path
    );

    // Create the Plymouth theme directory.
    if (mkdir_p(theme_dir) != 0)
    {
        LOG_ERROR("Failed to create Plymouth theme directory: %s", theme_dir);
        return -2;
    }

    // Construct the destination path for the splash image.
    snprintf(splash_dest, sizeof(splash_dest), "%s/splash.png", theme_dir);

    // Copy the logo to the theme directory.
    if (copy_file(logo_path, splash_dest) != 0)
    {
        LOG_ERROR("Failed to copy splash logo");
        return -3;
    }

    // Construct the theme file path.
    snprintf(theme_file_path, sizeof(theme_file_path), "%s/" CONFIG_PLYMOUTH_THEME_NAME ".plymouth", theme_dir);

    // Define the Plymouth theme configuration.
    const char *theme_cfg =
        "[Plymouth Theme]\n"
        "Name=LimeOS\n"
        "Description=LimeOS boot splash\n"
        "ModuleName=script\n"
        "\n"
        "[script]\n"
        "ImageDir=" CONFIG_PLYMOUTH_THEMES_DIR "/" CONFIG_PLYMOUTH_THEME_NAME "\n"
        "ScriptFile=" CONFIG_PLYMOUTH_THEMES_DIR "/" CONFIG_PLYMOUTH_THEME_NAME "/" CONFIG_PLYMOUTH_THEME_NAME ".script\n";

    // Write the Plymouth theme file.
    if (write_file(theme_file_path, theme_cfg) != 0)
    {
        return -4;
    }

    // Construct the script file path.
    char script_path[MAX_PATH_LENGTH];
    snprintf(script_path, sizeof(script_path), "%s/" CONFIG_PLYMOUTH_THEME_NAME ".script", theme_dir);

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
        return -5;
    }

    // Set LimeOS as the default Plymouth theme.
    snprintf(theme_cmd, sizeof(theme_cmd), "plymouth-set-default-theme %s", CONFIG_PLYMOUTH_THEME_NAME);
    if (run_chroot(rootfs_path, theme_cmd) != 0)
    {
        LOG_WARNING("Failed to set Plymouth theme (plymouth may not be installed)");
    }

    // Regenerate initramfs to include the new Plymouth theme.
    LOG_INFO("Regenerating initramfs with new theme...");
    if (run_chroot(rootfs_path, "update-initramfs -u") != 0)
    {
        LOG_WARNING("Failed to regenerate initramfs");
    }

    // Re-copy the updated initrd using safe glob.
    char initrd_pattern[MAX_PATH_LENGTH];
    char initrd_src[MAX_PATH_LENGTH];
    char initrd_dst[MAX_PATH_LENGTH];

    snprintf(initrd_pattern, sizeof(initrd_pattern), "%s/boot/initrd.img-*", rootfs_path);
    if (find_first_glob(initrd_pattern, initrd_src, sizeof(initrd_src)) == 0)
    {
        snprintf(initrd_dst, sizeof(initrd_dst), "%s/boot/initrd.img", rootfs_path);
        copy_file(initrd_src, initrd_dst);
    }

    LOG_INFO("Plymouth splash configured successfully");

    return 0;
}
