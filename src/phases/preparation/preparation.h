#pragma once

/**
 * Runs the preparation phase.
 *
 * Initializes the fetch module, downloads all required LimeOS components
 * from GitHub releases (or uses local binaries if available), and cleans up.
 *
 * @param version The version tag to fetch.
 * @param components_dir The directory to store downloaded components.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_preparation_phase(const char *version, const char *components_dir);
