/**
 * This code orchestrates payload-specific branding.
 *
 * Combines shared branding (identity, splash) with payload-specific
 * GRUB configuration for the installed system.
 */

#include "all.h"

int brand_payload_rootfs(const char *path, const char *version)
{
    LOG_INFO("Applying payload branding...");

    if (brand_os_identity(path, version) != 0)
    {
        return -1;
    }

    if (brand_splash(path, CONFIG_SPLASH_LOGO_PATH) != 0)
    {
        return -2;
    }

    if (brand_grub(path) != 0)
    {
        return -3;
    }

    LOG_INFO("Payload branding applied successfully");

    return 0;
}
