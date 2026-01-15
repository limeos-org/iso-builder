/**
 * This code configures APT package sources for the target system.
 */

#include "all.h"

int brand_apt_sources(const char *rootfs_path)
{
    char content[1024];
    char path[COMMAND_PATH_MAX_LENGTH];

    LOG_INFO("Configuring APT sources...");

    // Write /etc/apt/sources.list with Debian bookworm repositories.
    snprintf(content, sizeof(content),
        "deb http://deb.debian.org/debian " CONFIG_DEBIAN_RELEASE " main contrib non-free non-free-firmware\n"
        "deb http://deb.debian.org/debian " CONFIG_DEBIAN_RELEASE "-updates main contrib non-free non-free-firmware\n"
        "deb http://security.debian.org/debian-security " CONFIG_DEBIAN_RELEASE "-security main contrib non-free non-free-firmware\n");

    snprintf(path, sizeof(path), "%s/etc/apt/sources.list", rootfs_path);
    if (write_file(path, content) != 0)
    {
        LOG_ERROR("Failed to write /etc/apt/sources.list");
        return -1;
    }

    LOG_INFO("APT sources configured successfully");

    return 0;
}
