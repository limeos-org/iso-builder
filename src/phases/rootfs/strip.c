/**
 * This code is responsible for stripping unnecessary files from the rootfs.
 */

#include "all.h"

int strip_rootfs(const char *path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];

    // Log the start of rootfs stripping.
    LOG_INFO("Stripping rootfs at %s", path);

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

    // Remove apt cache.
    snprintf(dir_path, sizeof(dir_path), "%s/var/cache/apt", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove apt cache");
        return -3;
    }

    // Recreate apt cache directory.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to recreate apt cache directory");
    }

    // Remove apt lists.
    snprintf(dir_path, sizeof(dir_path), "%s/var/lib/apt/lists", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove apt lists");
        return -4;
    }

    // Recreate apt lists directory.
    if (mkdir_p(dir_path) != 0)
    {
        LOG_WARNING("Failed to recreate apt lists directory");
    }

    LOG_INFO("Rootfs stripped successfully");

    return 0;
}
