/**
 * This code provides OS identity branding (os-release, issue files).
 */

#include "all.h"

/** Maximum length for branding file content. */
#define BRAND_CONTENT_MAX_LENGTH 512

int brand_os_identity(const char *rootfs_path, const char *version)
{
    char content[BRAND_CONTENT_MAX_LENGTH];
    char path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Applying OS identity branding...");

    // Strip the 'v' prefix if present for cleaner version display.
    const char *clean_version = skip_version_prefix(version);

    // Write /etc/os-release for programmatic identification.
    snprintf(content, sizeof(content),
        "PRETTY_NAME=\"" CONFIG_OS_NAME " %s\"\n"
        "NAME=\"" CONFIG_OS_NAME "\"\n"
        "VERSION_ID=\"%s\"\n"
        "VERSION=\"%s\"\n"
        "ID=" CONFIG_OS_ID "\n"
        "ID_LIKE=" CONFIG_OS_BASE_ID "\n"
        "HOME_URL=\"" CONFIG_OS_HOME_URL "\"\n",
        clean_version, clean_version, clean_version);
    snprintf(path, sizeof(path), "%s/etc/os-release", rootfs_path);
    if (write_file(path, content) != 0)
    {
        LOG_ERROR("Failed to write /etc/os-release");
        return -1;
    }

    // Write /etc/issue for local login prompts.
    snprintf(content, sizeof(content),
        CONFIG_OS_NAME " %s \\n \\l\n\n", clean_version);

    snprintf(path, sizeof(path), "%s/etc/issue", rootfs_path);
    if (write_file(path, content) != 0)
    {
        LOG_ERROR("Failed to write /etc/issue");
        return -2;
    }

    // Write /etc/issue.net for network login prompts.
    snprintf(content, sizeof(content), CONFIG_OS_NAME " %s\n", clean_version);

    snprintf(path, sizeof(path), "%s/etc/issue.net", rootfs_path);
    if (write_file(path, content) != 0)
    {
        LOG_ERROR("Failed to write /etc/issue.net");
        return -3;
    }

    LOG_INFO("OS identity branding applied successfully");

    return 0;
}
