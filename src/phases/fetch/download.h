#pragma once

/** The maximum length for URL strings. */
#define FETCH_URL_MAX_LENGTH 512

/**
 * Initializes the fetch module.
 *
 * Must be called before any other fetch functions. Initializes libcurl
 * globally.
 *
 * @return - `0` - Indicates successful initialization.
 * @return - `-1` - Indicates initialization failure.
 */
int init_fetch(void);

/**
 * Cleans up the fetch module.
 *
 * Should be called when the fetch module is no longer needed. Cleans up
 * libcurl.
 */
void cleanup_fetch(void);

/**
 * Fetches a component binary from local cache or GitHub releases.
 *
 * @param component The component definition with repo and binary names.
 * @param version The release version tag to download.
 * @param output_directory The directory to save the binary.
 *
 * @return - `0` - Indicates successful fetch.
 * @return - `-1` - Indicates a failure.
 */
int fetch_component(
    const Component *component,
    const char *version,
    const char *output_directory
);

/**
 * Fetches all LimeOS components from local cache or GitHub releases.
 *
 * Fetches window-manager, display-manager, and installation-wizard binaries.
 *
 * @param version The release version tag to download.
 * @param output_directory The directory to save the binaries.
 *
 * @return - `0` - Indicates all components fetched successfully.
 * @return - `-1` - Indicates one or more components failed.
 */
int fetch_all_components(const char *version, const char *output_directory);
