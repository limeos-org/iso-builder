/**
 * This code orchestrates carrier-specific branding.
 *
 * Applies OS identity and splash branding for the live environment.
 */

#include "all.h"

int brand_carrier_rootfs(const char *path, const char *version)
{
    LOG_INFO("Applying carrier branding...");

    // Apply OS identity branding.
    if (brand_os_identity(path, version) != 0)
    {
        return -1;
    }

    // Apply Plymouth splash branding.
    if (brand_splash(path, CONFIG_SPLASH_LOGO_PATH) != 0)
    {
        return -2;
    }

    LOG_INFO("Carrier branding applied successfully");

    return 0;
}
