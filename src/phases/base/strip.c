/**
 * This code is responsible for aggressively stripping noncritical files
 * from the base rootfs to minimize size for both target and live.
 */

#include "all.h"

static void mask_rfkill_service(const char *rootfs_path)
{
    char dir_path[COMMON_MAX_PATH_LENGTH];
    char mask_path[COMMON_MAX_PATH_LENGTH];

    // Construct the systemd system directory path.
    snprintf(dir_path, sizeof(dir_path), "%s/etc/systemd/system", rootfs_path);

    // Create systemd system directory if needed.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to create systemd system directory");
        return;
    }

    // Mask rfkill service (no RF devices to manage).
    snprintf(mask_path, sizeof(mask_path), "%s/systemd-rfkill.service", dir_path);
    if (symlink_file("/dev/null", mask_path) != 0)
    {
        LOG_WARNING("Failed to mask systemd-rfkill.service");
    }

    // Mask rfkill socket.
    snprintf(mask_path, sizeof(mask_path), "%s/systemd-rfkill.socket", dir_path);
    if (symlink_file("/dev/null", mask_path) != 0)
    {
        LOG_WARNING("Failed to mask systemd-rfkill.socket");
    }
}

int strip_base_rootfs(const char *path)
{
    char dir_path[COMMON_MAX_PATH_LENGTH];

    LOG_INFO("Stripping base rootfs at %s", path);

    // Remove documentation files.
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/doc", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove doc directory");
        return -1;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/man", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove man directory");
        return -2;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/info", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove info directory");
        return -3;
    }

    // Remove non-English locales.
    char command[COMMON_MAX_COMMAND_LENGTH];
    char quoted_locale_dir[COMMON_MAX_QUOTED_LENGTH];
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/locale", path);
    if (shell_escape_path(dir_path, quoted_locale_dir, sizeof(quoted_locale_dir)) != 0)
    {
        LOG_ERROR("Failed to quote locale directory");
        return -4;
    }
    snprintf(
        command, sizeof(command),
        "find %s -mindepth 1 -maxdepth 1 ! -name 'en*' -exec rm -rf {} +",
        quoted_locale_dir
    );
    if (run_command_indented(command) != 0)
    {
        LOG_ERROR("Failed to remove non-English locales");
        return -5;
    }

    // Mask rfkill service since there's no RF hardware to manage.
    mask_rfkill_service(path);

    // Clear MOTD files that display Debian messages on login.
    snprintf(dir_path, sizeof(dir_path), "%s/etc/motd", path);
    if (write_file(dir_path, "") != 0)
    {
        LOG_ERROR("Failed to clear /etc/motd");
        return -6;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/etc/update-motd.d", path);
    rm_rf(dir_path);  // OK if it doesn't exist.

    LOG_INFO("Base rootfs stripped successfully");

    return 0;
}
