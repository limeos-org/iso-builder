/**
 * This code is responsible for collecting LimeOS component binaries from
 * local cache or GitHub releases using libcurl.
 */

#include "all.h"

static int copy_local(const char *name, const char *output_directory)
{
    char local_path[MAX_PATH_LENGTH];
    char output_path[MAX_PATH_LENGTH];

    // Construct the local binary path with limeos- prefix.
    snprintf(local_path, sizeof(local_path), "./bin/limeos-%s", name);

    // Check if the local binary exists.
    if (!file_exists(local_path))
    {
        return -1;
    }

    // Create the output directory if it does not exist.
    mkdir_p(output_directory);

    // Construct the output path.
    snprintf(output_path, sizeof(output_path), "%s/%s", output_directory, name);

    // Copy the local binary to the output directory.
    if (copy_file(local_path, output_path) != 0)
    {
        return -1;
    }

    LOG_INFO("Using local limeos-%s", name);

    return 0;
}

int init_collector(void)
{
    // Initialize the curl library globally.
    CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (result != CURLE_OK)
    {
        return -1;
    }

    return 0;
}

void cleanup_collector(void)
{
    // Clean up the curl library globally.
    curl_global_cleanup();
}

static size_t handle_write_data(
    void *data, size_t size, size_t count, void *stream
)
{
    return fwrite(data, size, count, (FILE *)stream);
}

static int download_remote(
    const char *name,
    const char *version,
    const char *output_directory
)
{
    CURL *curl;
    CURLcode result;
    FILE *output_file;
    char url[MAX_URL_LENGTH];
    char output_path[MAX_PATH_LENGTH];
    char resolved_version[CONFIG_VERSION_MAX_LENGTH];

    // Resolve the version to the latest within the major version.
    int resolve_result = resolve_version(
        name, version, resolved_version, sizeof(resolved_version)
    );
    if (resolve_result == -1)
    {
        // API failure - fall back to exact version.
        LOG_WARNING(
            "Version resolution failed for %s, using exact version %s",
            name, version
        );
        strncpy(resolved_version, version, sizeof(resolved_version) - 1);
        resolved_version[sizeof(resolved_version) - 1] = '\0';
    }
    else if (resolve_result < 0)
    {
        // Other failures (no matching version, parse error).
        return -1;
    }

    // Construct the GitHub release download URL.
    snprintf(
        url, sizeof(url),
        "https://github.com/%s/%s/releases/download/%s/%s",
        CONFIG_GITHUB_ORG, name, resolved_version, name
    );

    // Construct the local output file path.
    snprintf(output_path, sizeof(output_path), "%s/%s", output_directory, name);

    // Log the fetch operation.
    LOG_INFO("Fetching %s %s", name, resolved_version);

    // Create the output directory if it does not exist.
    mkdir_p(output_directory);

    // Open the output file for writing.
    output_file = fopen(output_path, "wb");
    if (!output_file)
    {
        LOG_ERROR("Failed to create file %s: %s", output_path, strerror(errno));
        return -1;
    }

    // Initialize the curl session.
    curl = curl_easy_init();
    if (!curl)
    {
        LOG_ERROR("Failed to initialize curl");
        fclose(output_file);
        return -1;
    }

    // Configure curl options for the download.
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, CONFIG_USER_AGENT);

    // Perform the download.
    result = curl_easy_perform(curl);

    // Get the HTTP response code.
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    // Clean up curl resources.
    curl_easy_cleanup(curl);
    fclose(output_file);

    // Check for curl errors.
    if (result != CURLE_OK)
    {
        remove(output_path);
        LOG_ERROR("Download failed: %s", curl_easy_strerror(result));
        return -1;
    }

    // Check for HTTP errors.
    if (http_code != 200)
    {
        remove(output_path);
        LOG_ERROR("Download failed: HTTP %ld", http_code);
        return -1;
    }

    LOG_INFO("Downloaded %s", name);

    return 0;
}

int fetch_component(
    const char *name,
    const char *version,
    const char *output_directory
)
{
    // Try local binary first.
    if (copy_local(name, output_directory) == 0)
    {
        return 0;
    }

    // Fall back to remote download.
    return download_remote(name, version, output_directory);
}

int fetch_all_components(const char *version, const char *output_directory)
{
    LOG_INFO("Fetching LimeOS components...");

    // Fetch required components.
    for (int i = 0; i < REQUIRED_COMPONENTS_COUNT; i++)
    {
        if (fetch_component(REQUIRED_COMPONENTS[i], version, output_directory) != 0)
        {
            LOG_ERROR("Required component failed: %s", REQUIRED_COMPONENTS[i]);
            return -1;
        }
    }

    // Fetch optional components.
    for (int i = 0; i < OPTIONAL_COMPONENTS_COUNT; i++)
    {
        if (fetch_component(OPTIONAL_COMPONENTS[i], version, output_directory) != 0)
        {
            LOG_WARNING("Optional component skipped: %s", OPTIONAL_COMPONENTS[i]);
        }
    }

    LOG_INFO("All required components fetched successfully");

    return 0;
}
