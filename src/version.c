/**
 * This code is responsible for resolving GitHub release versions by querying
 * the GitHub API and finding the latest release within a major version.
 */

#include "all.h"
#include <json-c/json.h>

/** The GitHub organization hosting LimeOS component repositories. */
#define VERSION_GITHUB_ORG "limeos-org"

/** The GitHub API base URL for releases. */
#define VERSION_API_BASE "https://api.github.com/repos"

/**
 * The initial buffer size for API response data.
 *
 * 8KB is sufficient for typical GitHub releases API responses containing
 * 10-20 releases. The buffer grows dynamically if needed.
 */
#define VERSION_INITIAL_BUFFER_SIZE 8192

/** A type representing a dynamically growing buffer for HTTP response data. */
typedef struct
{
    char *data;
    size_t size;
    size_t capacity;
} ResponseBuffer;

static size_t handle_api_response(
    void *contents,
    size_t size,
    size_t count,
    void *userdata
)
{
    size_t total_size = size * count;
    ResponseBuffer *buffer = (ResponseBuffer *)userdata;

    // Expand the buffer if needed.
    while (buffer->size + total_size >= buffer->capacity)
    {
        size_t new_capacity = buffer->capacity * 2;
        char *new_data = realloc(buffer->data, new_capacity);
        if (!new_data)
        {
            return 0;
        }
        buffer->data = new_data;
        buffer->capacity = new_capacity;
    }

    // Append the new data to the buffer.
    memcpy(buffer->data + buffer->size, contents, total_size);
    buffer->size += total_size;
    buffer->data[buffer->size] = '\0';

    return total_size;
}

static int fetch_releases_json(
    const char *component,
    char **out_json_data
)
{
    CURL *curl;
    CURLcode result;
    char url[MAX_URL_LENGTH];
    struct curl_slist *headers = NULL;
    ResponseBuffer buffer = {0};

    // Construct the GitHub API URL.
    snprintf(
        url, sizeof(url),
        "%s/%s/%s/releases",
        VERSION_API_BASE, VERSION_GITHUB_ORG, component
    );

    // Allocate the initial response buffer.
    buffer.data = malloc(VERSION_INITIAL_BUFFER_SIZE);
    if (!buffer.data)
    {
        return -1;
    }
    buffer.data[0] = '\0';
    buffer.size = 0;
    buffer.capacity = VERSION_INITIAL_BUFFER_SIZE;

    // Initialize the curl session.
    curl = curl_easy_init();
    if (!curl)
    {
        free(buffer.data);
        return -1;
    }

    // Set up required headers for GitHub API.
    headers = curl_slist_append(headers, "Accept: application/vnd.github+json");
    headers = curl_slist_append(headers, "X-GitHub-Api-Version: 2022-11-28");

    // Configure curl options for the API request.
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_api_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "limeos-iso-builder/1.0");

    // Perform the API request.
    result = curl_easy_perform(curl);

    // Get the HTTP response code.
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    // Clean up curl resources.
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    // Check for curl errors.
    if (result != CURLE_OK)
    {
        LOG_ERROR("GitHub API request failed: %s", curl_easy_strerror(result));
        free(buffer.data);
        return -1;
    }

    // Check for HTTP errors.
    if (http_code != 200)
    {
        LOG_ERROR("GitHub API returned HTTP %ld", http_code);
        free(buffer.data);
        return -1;
    }

    // Transfer ownership of the buffer to the caller.
    *out_json_data = buffer.data;

    return 0;
}

int resolve_version(
    const char *component,
    const char *version,
    char *out_resolved,
    size_t buffer_length
)
{
    char *json_data = NULL;
    json_object *root = NULL;
    int target_major;
    const char *best_version = NULL;

    // Extract the target major version from the user-provided version.
    target_major = extract_major_version(version);
    if (target_major < 0)
    {
        LOG_ERROR("Invalid version format: %s", version);
        return -2;
    }

    // Fetch the releases JSON from GitHub API.
    if (fetch_releases_json(component, &json_data) != 0)
    {
        return -1;
    }

    // Parse the JSON response.
    root = json_tokener_parse(json_data);
    if (!root)
    {
        LOG_ERROR("Failed to parse GitHub API response");
        free(json_data);
        return -3;
    }

    // Validate the response is an array.
    if (!json_object_is_type(root, json_type_array))
    {
        LOG_ERROR("Unexpected GitHub API response format");
        json_object_put(root);
        free(json_data);
        return -3;
    }

    // Iterate through releases to find the best matching version.
    size_t release_count = json_object_array_length(root);
    for (size_t i = 0; i < release_count; i++)
    {
        json_object *release = json_object_array_get_idx(root, i);
        json_object *tag_name_obj;

        // Extract the tag_name field.
        if (!json_object_object_get_ex(release, "tag_name", &tag_name_obj))
        {
            continue;
        }

        const char *tag_name = json_object_get_string(tag_name_obj);
        if (!tag_name)
        {
            continue;
        }

        // Check if this release matches the target major version.
        int release_major = extract_major_version(tag_name);
        if (release_major != target_major)
        {
            continue;
        }

        // Check if this is a prerelease or draft (skip them).
        json_object *prerelease_obj;
        json_object *draft_obj;
        if (json_object_object_get_ex(release, "prerelease", &prerelease_obj))
        {
            if (json_object_get_boolean(prerelease_obj))
            {
                continue;
            }
        }
        if (json_object_object_get_ex(release, "draft", &draft_obj))
        {
            if (json_object_get_boolean(draft_obj))
            {
                continue;
            }
        }

        // Update the best version if this one is newer.
        if (!best_version || compare_versions(tag_name, best_version) > 0)
        {
            best_version = tag_name;
        }
    }

    // Check if a matching version was found.
    if (!best_version)
    {
        LOG_WARNING(
            "No release found for %s with major version %d",
            component, target_major
        );
        json_object_put(root);
        free(json_data);
        return -2;
    }

    // Copy the resolved version to the output buffer.
    strncpy(out_resolved, best_version, buffer_length - 1);
    out_resolved[buffer_length - 1] = '\0';

    // Log the resolution result.
    LOG_INFO("Resolved %s version: %s -> %s", component, version, out_resolved);

    // Clean up JSON resources.
    json_object_put(root);
    free(json_data);

    return 0;
}
