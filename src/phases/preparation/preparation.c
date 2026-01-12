/**
 * This code is responsible for orchestrating the preparation phase.
 */

#include "all.h"

int run_preparation_phase(const char *version, const char *components_dir)
{
    if (init_fetch() != 0)
    {
        LOG_ERROR("Failed to initialize fetch module");
        return -1;
    }

    if (fetch_all_components(version, components_dir) != 0)
    {
        LOG_ERROR("Failed to fetch components");
        cleanup_fetch();
        return -1;
    }

    cleanup_fetch();
    LOG_INFO("Phase 1 complete: Preparation finished");
    return 0;
}
