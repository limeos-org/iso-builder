#pragma once
#include "../all.h"

/**
 * Resolves the latest release version within a major version for a component.
 *
 * Queries the GitHub API for the specified component repository and finds the
 * latest release that shares the same major version as the provided version.
 *
 * @param component The component name (without `limeos` suffix, 
 * e.g., "window-manager").
 * @param version The user-provided version (e.g., "1.0.0").
 * @param out_resolved The buffer to store the resolved version string.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates successful resolution.
 * @return - `-1` - Indicates invalid version format.
 * @return - `-2` - Indicates a network or API failure.
 * @return - `-3` - Indicates JSON parsing failure.
 * @return - `-4` - Indicates unexpected API response format.
 * @return - `-5` - Indicates no matching version was found.
 */
int resolve_version(
    const char *component,
    const char *version,
    char *out_resolved,
    size_t buffer_length
);
