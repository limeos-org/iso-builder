#pragma once

#include <stddef.h>

/** The maximum length for version strings (e.g., "v1.2.345"). */
#define VERSION_MAX_LENGTH 32

/**
 * Validates that a version string is in semantic versioning format.
 *
 * Accepts formats: "X.Y.Z" or "vX.Y.Z" where X, Y, Z are non-negative integers.
 *
 * @param version The version string to validate.
 *
 * @return - `0` - Indicates a valid version format.
 * @return - `-1` - Indicates an invalid version format.
 */
int validate_version(const char *version);

/**
 * Extracts the major version number from a semantic version string.
 *
 * @param version The version string (e.g., "1.2.3" or "v1.2.3").
 *
 * @return The major version number, or -1 if parsing fails.
 */
int extract_major_version(const char *version);

/**
 * Compares two semantic version strings.
 *
 * @param v1 The first version string.
 * @param v2 The second version string.
 *
 * @return - `1` - Indicates v1 is greater than v2.
 * @return - `0` - Indicates v1 equals v2.
 * @return - `-1` - Indicates v1 is less than v2.
 */
int compare_versions(const char *v1, const char *v2);

/**
 * Skips the optional 'v' or 'V' prefix from a version string.
 *
 * @param version The version string to process.
 *
 * @return Pointer to the first character after the prefix, or the original
 *         pointer if no prefix exists.
 */
const char *skip_version_prefix(const char *version);
