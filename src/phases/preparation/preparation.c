/**
 * This code is responsible for orchestrating the preparation phase.
 */

#include "all.h"

int run_preparation_phase(const char *version, const char *components_dir)
{
    // Initialize the fetch module.
    if (init_fetch() != 0)
    {
        LOG_ERROR("Failed to initialize fetch module");
        return -1;
    }

    // Fetch all LimeOS components.
    if (fetch_all_components(version, components_dir) != 0)
    {
        LOG_ERROR("Failed to fetch components");
        cleanup_fetch();
        return -2;
    }

    // Clean up the fetch module.
    cleanup_fetch();
    LOG_INFO("Phase 1 complete: Preparation finished");
    return 0;
}
