/**
 * This code is responsible for aggressively stripping unnecessary files
 * from the base rootfs to minimize size for both payload and carrier.
 */

#include "all.h"

int strip_base_rootfs(const char *path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];

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
        return -1;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/info", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove info directory");
        return -1;
    }

    // Remove non-English locales.
    char command[COMMAND_MAX_LENGTH];
    char quoted_locale_dir[COMMAND_QUOTED_MAX_LENGTH];
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/locale", path);
    if (shell_quote_path(dir_path, quoted_locale_dir, sizeof(quoted_locale_dir)) != 0)
    {
        LOG_ERROR("Failed to quote locale directory");
        return -2;
    }
    snprintf(
        command, sizeof(command),
        "find %s -mindepth 1 -maxdepth 1 ! -name 'en*' -exec rm -rf {} +",
        quoted_locale_dir
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to remove non-English locales");
        return -2;
    }

    // Remove unnecessary firmware while keeping GPU and CPU microcode.
    cleanup_unnecessary_firmware(path);

    // Blacklist kernel modules for removed firmware so they don't try to load.
    blacklist_wireless_modules(path);

    // Mask rfkill service since there's no RF hardware to manage.
    mask_rfkill_service(path);

    // NOTE: Do NOT cleanup apt directories here. The payload and carrier
    // phases need apt to install packages after copying from base.

    LOG_INFO("Base rootfs stripped successfully");

    return 0;
}
