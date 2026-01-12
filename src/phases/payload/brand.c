/**
 * This code is responsible for applying OS branding to the payload rootfs.
 *
 * Replaces default Debian identification with LimeOS branding so the
 * installed system identifies itself correctly.
 */

#include "all.h"

/** Maximum length for branding file content. */
#define BRAND_CONTENT_MAX_LENGTH 512

static int write_os_release(const char *rootfs_path, const char *version)
{
    // Prepare content for /etc/os-release.
    char content[BRAND_CONTENT_MAX_LENGTH];
    snprintf(content, sizeof(content),
    "PRETTY_NAME=\"" CONFIG_OS_NAME " %s\"\n"
    "NAME=\"" CONFIG_OS_NAME "\"\n"
    "VERSION_ID=\"%s\"\n"
    "VERSION=\"%s\"\n"
    "ID=" CONFIG_OS_ID "\n"
    "ID_LIKE=" CONFIG_OS_BASE_ID "\n"
    "HOME_URL=\"" CONFIG_OS_HOME_URL "\"\n",
    version, version, version);
    
    // Write to /etc/os-release.
    char path[COMMAND_PATH_MAX_LENGTH];
    snprintf(path, sizeof(path), "%s/etc/os-release", rootfs_path);
    if (write_file(path, content) != 0)
    {
        LOG_ERROR("Failed to write /etc/os-release");
        return -1;
    }

    return 0;
}

static int write_issue(const char *rootfs_path, const char *version)
{
    // Prepare content for /etc/issue
    char content[BRAND_CONTENT_MAX_LENGTH];
    snprintf(
        content, sizeof(content),
        CONFIG_OS_NAME " %s \\n \\l\n\n", version
    );

    // Write to /etc/issue for local logins.
    char path[COMMAND_PATH_MAX_LENGTH];
    snprintf(path, sizeof(path), "%s/etc/issue", rootfs_path);
    if (write_file(path, content) != 0)
    {
        LOG_ERROR("Failed to write /etc/issue");
        return -1;
    }

    // Write the same content to /etc/issue.net for network logins.
    snprintf(content, sizeof(content), CONFIG_OS_NAME " %s\n", version);
    snprintf(path, sizeof(path), "%s/etc/issue.net", rootfs_path);
    if (write_file(path, content) != 0)
    {
        LOG_ERROR("Failed to write /etc/issue.net");
        return -1;
    }

    return 0;
}

static int write_grub_distributor(const char *rootfs_path)
{
    // Use drop-in directory to override GRUB_DISTRIBUTOR without replacing
    // the package-managed /etc/default/grub file.
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(dir_path, sizeof(dir_path), "%s/etc/default/grub.d", rootfs_path);
    if (mkdir_p(dir_path) != 0)
    {
        LOG_ERROR("Failed to create /etc/default/grub.d");
        return -1;
    }
    
    // Write the distributor config file.
    char file_path[COMMAND_PATH_MAX_LENGTH];
    const char *content = "GRUB_DISTRIBUTOR=\"" CONFIG_OS_NAME "\"\n";
    snprintf(file_path, sizeof(file_path), "%s/distributor.cfg", dir_path);
    if (write_file(file_path, content) != 0)
    {
        LOG_ERROR("Failed to write GRUB distributor config");
        return -1;
    }

    return 0;
}

int brand_payload_rootfs(const char *path, const char *version)
{
    LOG_INFO("Applying OS branding to payload rootfs...");

    // Strip the 'v' prefix if present for cleaner version display.
    const char *clean_version = skip_version_prefix(version);

    if (write_os_release(path, clean_version) != 0)
    {
        return -1;
    }

    if (write_issue(path, clean_version) != 0)
    {
        return -2;
    }

    if (write_grub_distributor(path) != 0)
    {
        return -3;
    }

    LOG_INFO("OS branding applied successfully");

    return 0;
}
