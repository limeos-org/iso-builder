/**
 * This code is responsible for semantic version parsing and comparison.
 */

#include "all.h"

const char *skip_version_prefix(const char *version)
{
    if (*version == 'v' || *version == 'V')
    {
        return version + 1;
    }
    return version;
}

int validate_version(const char *version)
{
    const char *p = skip_version_prefix(version);

    // Parse major version (required, at least one digit).
    if (*p < '0' || *p > '9')
    {
        return -1;
    }
    while (*p >= '0' && *p <= '9')
    {
        p++;
    }

    // Require dot separator.
    if (*p != '.')
    {
        return -1;
    }
    p++;

    // Parse minor version (required, at least one digit).
    if (*p < '0' || *p > '9')
    {
        return -1;
    }
    while (*p >= '0' && *p <= '9')
    {
        p++;
    }

    // Require dot separator.
    if (*p != '.')
    {
        return -1;
    }
    p++;

    // Parse patch version (required, at least one digit).
    if (*p < '0' || *p > '9')
    {
        return -1;
    }
    while (*p >= '0' && *p <= '9')
    {
        p++;
    }

    // Ensure no trailing characters.
    if (*p != '\0')
    {
        return -1;
    }

    return 0;
}

int extract_major_version(const char *version)
{
    const char *start = skip_version_prefix(version);

    // Parse the major version number.
    char *end;
    long major = strtol(start, &end, 10);
    if (end == start || major < 0)
    {
        return -1;
    }

    return (int)major;
}

int compare_versions(const char *v1, const char *v2)
{
    const char *p1 = skip_version_prefix(v1);
    const char *p2 = skip_version_prefix(v2);

    // Compare each numeric component.
    while (*p1 && *p2)
    {
        // Parse the next numeric segment from each version.
        long n1 = strtol(p1, (char **)&p1, 10);
        long n2 = strtol(p2, (char **)&p2, 10);

        // Compare the numeric segments.
        if (n1 > n2)
        {
            return 1;
        }
        if (n1 < n2)
        {
            return -1;
        }

        // Skip the dot separator if present.
        if (*p1 == '.')
        {
            p1++;
        }
        if (*p2 == '.')
        {
            p2++;
        }
    }

    // Check if both strings are fully consumed.
    if (*p1 == '\0' && *p2 == '\0')
    {
        return 0;
    }

    // Version with more components is greater (e.g., "1.2.3" > "1.2").
    if (*p1 == '\0')
    {
        return -1;
    }

    return 1;
}
