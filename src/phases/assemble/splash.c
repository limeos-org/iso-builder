/**
 * This code is responsible for configuring the Plymouth splash screen.
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

int setup_splash(const char *rootfs_path, const char *logo_path)
{
    char theme_dir[COMMAND_PATH_MAX_LENGTH];
    char theme_file_path[COMMAND_PATH_MAX_LENGTH];
    char splash_dest[COMMAND_PATH_MAX_LENGTH];
    char theme_cmd[COMMAND_PATH_MAX_LENGTH];

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
    char script_path[COMMAND_PATH_MAX_LENGTH];
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
    char initrd_pattern[COMMAND_PATH_MAX_LENGTH];
    char initrd_src[COMMAND_PATH_MAX_LENGTH];
    char initrd_dst[COMMAND_PATH_MAX_LENGTH];

    snprintf(initrd_pattern, sizeof(initrd_pattern), "%s/boot/initrd.img-*", rootfs_path);
    if (find_first_glob(initrd_pattern, initrd_src, sizeof(initrd_src)) == 0)
    {
        snprintf(initrd_dst, sizeof(initrd_dst), "%s/boot/initrd.img", rootfs_path);
        copy_file(initrd_src, initrd_dst);
    }

    LOG_INFO("Plymouth splash configured successfully");

    return 0;
}
