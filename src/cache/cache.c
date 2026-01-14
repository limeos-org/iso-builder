/**
 * This code provides the base cache directory resolution used by all
 * caching subsystems.
 */

#include "all.h"

int get_cache_dir(char *out_path, size_t buffer_length)
{
    // Check for XDG_CACHE_HOME environment variable.
    const char *cache_home = getenv("XDG_CACHE_HOME");
    if (cache_home && cache_home[0] != '\0')
    {
        snprintf(out_path, buffer_length, "%s/%s",
            cache_home, CONFIG_CACHE_DIR_NAME);
        return 0;
    }

    // Fall back to ~/.cache if HOME is set.
    const char *home = getenv("HOME");
    if (home && home[0] != '\0')
    {
        snprintf(
            out_path, buffer_length, "%s/.cache/%s",
            home, CONFIG_CACHE_DIR_NAME
        );
        return 0;
    }

    LOG_ERROR("Cannot determine cache directory: HOME not set");
    return -1;
}
