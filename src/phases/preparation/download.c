/**
 * This code is responsible for downloading LimeOS component binaries from
 * GitHub releases or loading them from the local filesystem.
 */

#include "all.h"

/** Size of the read buffer for computing file checksums. */
#define CHECKSUM_BUFFER_SIZE 8192

/** Length of SHA256 hash (32 bytes). */
#define SHA256_DIGEST_LEN 32

/** Length of SHA256 hash in hex format (64 chars + null). */
#define SHA256_HEX_LENGTH 65

semistatic int compute_file_sha256(
    const char *path, char *out_hash, size_t hash_len
)
{
    unsigned char hash[SHA256_DIGEST_LEN];
    unsigned char buffer[CHECKSUM_BUFFER_SIZE];
    unsigned int digest_len = 0;
    EVP_MD_CTX *ctx;
    FILE *file;
    size_t bytes_read;

    // Validate output buffer size.
    if (hash_len < SHA256_HEX_LENGTH)
    {
        return -1;
    }

    // Open the file for reading.
    file = fopen(path, "rb");
    if (!file)
    {
        LOG_ERROR("Failed to open file for checksum: %s", path);
        return -1;
    }

    // Initialize the digest context.
    ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        fclose(file);
        return -1;
    }

    // Configure the context for SHA256.
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1)
    {
        EVP_MD_CTX_free(ctx);
        fclose(file);
        return -1;
    }

    // Process the file in chunks.
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        if (EVP_DigestUpdate(ctx, buffer, bytes_read) != 1)
        {
            EVP_MD_CTX_free(ctx);
            fclose(file);
            return -1;
        }
    }

    // Finalize the hash computation.
    if (EVP_DigestFinal_ex(ctx, hash, &digest_len) != 1)
    {
        EVP_MD_CTX_free(ctx);
        fclose(file);
        return -1;
    }

    // Clean up resources.
    EVP_MD_CTX_free(ctx);
    fclose(file);

    // Convert the hash to a hex string.
    for (unsigned int i = 0; i < digest_len; i++)
    {
        snprintf(out_hash + (i * 2), 3, "%02x", hash[i]);
    }
    out_hash[SHA256_HEX_LENGTH - 1] = '\0';

    return 0;
}

static int fetch_expected_checksum(
    const char *repo_name,
    const char *version,
    const char *binary_name,
    char *out_hash,
    size_t hash_len
)
{
    CURL *curl;
    CURLcode result;
    char url[FETCH_URL_MAX_LENGTH];
    char *checksums_data = NULL;
    size_t checksums_size = 0;
    FILE *checksums_stream;
    long http_code = 0;

    // Validate buffer size.
    if (hash_len < SHA256_HEX_LENGTH)
    {
        return -1;
    }

    // Construct the checksums file URL.
    snprintf(
        url, sizeof(url),
        "https://github.com/%s/%s/releases/download/%s/" CONFIG_CHECKSUMS_FILENAME,
        CONFIG_GITHUB_ORG, repo_name, version
    );

    // Create an in-memory stream for the checksums data.
    checksums_stream = open_memstream(&checksums_data, &checksums_size);
    if (!checksums_stream)
    {
        LOG_ERROR("Failed to create memory stream for checksums");
        return -1;
    }

    // Initialize the curl session.
    curl = curl_easy_init();
    if (!curl)
    {
        fclose(checksums_stream);
        free(checksums_data);
        return -1;
    }

    // Configure curl options and perform the download.
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, checksums_stream);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, CONFIG_USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, FETCH_TIMEOUT_SECONDS);
    result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);
    fclose(checksums_stream);

    // Check for download errors.
    if (result != CURLE_OK || http_code != 200)
    {
        free(checksums_data);
        return -1;
    }

    // Parse the checksums file (format: "hash  filename").
    char *line = checksums_data;
    char *next_line;
    int found = 0;

    while (line && *line)
    {
        next_line = strchr(line, '\n');
        if (next_line)
        {
            *next_line = '\0';
            next_line++;
        }

        // Skip empty lines.
        if (strlen(line) < SHA256_HEX_LENGTH)
        {
            line = next_line;
            continue;
        }

        // Check if this line is for our binary (format: "hash  filename").
        char *filename_start = strstr(line, "  ");
        if (filename_start)
        {
            filename_start += 2;
            if (strcmp(filename_start, binary_name) == 0)
            {
                strncpy(out_hash, line, SHA256_HEX_LENGTH - 1);
                out_hash[SHA256_HEX_LENGTH - 1] = '\0';
                found = 1;
                break;
            }
        }

        line = next_line;
    }

    // Clean up and return the result.
    free(checksums_data);
    return found ? 0 : -1;
}

static int verify_checksum(
    const char *file_path,
    const char *repo_name,
    const char *version,
    const char *binary_name
)
{
    char expected_hash[SHA256_HEX_LENGTH];
    char actual_hash[SHA256_HEX_LENGTH];

    // Fetch the expected checksum from the release.
    if (fetch_expected_checksum(repo_name, version, binary_name, expected_hash, sizeof(expected_hash)) != 0)
    {
        LOG_WARNING("No checksum available for %s - skipping verification", binary_name);
        return 0;
    }

    // Compute the actual checksum of the downloaded file.
    if (compute_file_sha256(file_path, actual_hash, sizeof(actual_hash)) != 0)
    {
        LOG_ERROR("Failed to compute checksum for %s", file_path);
        return -1;
    }

    // Compare checksums.
    if (strcasecmp(expected_hash, actual_hash) != 0)
    {
        LOG_ERROR("Checksum mismatch for %s", binary_name);
        LOG_ERROR("  Expected: %s", expected_hash);
        LOG_ERROR("  Actual:   %s", actual_hash);
        return -1;
    }

    LOG_INFO("Checksum verified for %s", binary_name);
    return 0;
}

static int copy_local_component(
    const Component *component, const char *output_directory
)
{
    char local_path[COMMAND_PATH_MAX_LENGTH];
    char output_path[COMMAND_PATH_MAX_LENGTH];

    // Construct the local binary path using the binary name.
    snprintf(local_path, sizeof(local_path), CONFIG_LOCAL_BIN_DIR "/%s", component->binary_name);

    // Check if the local binary exists.
    if (!file_exists(local_path))
    {
        return -1;
    }

    // Create the output directory if it does not exist.
    mkdir_p(output_directory);

    // Construct the output path using the repo name.
    snprintf(output_path, sizeof(output_path), "%s/%s", output_directory, component->repo_name);

    // Copy the local binary to the output directory.
    if (copy_file(local_path, output_path) != 0)
    {
        return -1;
    }

    LOG_INFO("Using local %s", component->binary_name);

    return 0;
}

static size_t write_download_chunk(
    void *data, size_t size, size_t count, void *stream
)
{
    return fwrite(data, size, count, (FILE *)stream);
}

static int download_remote(
    const Component *component,
    const char *version,
    const char *output_directory
)
{
    CURL *curl;
    CURLcode result;
    FILE *output_file;
    char url[FETCH_URL_MAX_LENGTH];
    char output_path[COMMAND_PATH_MAX_LENGTH];
    char resolved_version[VERSION_MAX_LENGTH];

    // Resolve the version to the latest within the major version.
    int resolve_result = resolve_version(
        component->repo_name, version, resolved_version, sizeof(resolved_version)
    );
    if (resolve_result == -1)
    {
        // API failure - fall back to exact version.
        LOG_WARNING(
            "Version resolution failed for %s, using exact version %s",
            component->repo_name, version
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
        CONFIG_GITHUB_ORG, component->repo_name, resolved_version, component->repo_name
    );

    // Construct the local output file path.
    snprintf(output_path, sizeof(output_path), "%s/%s", output_directory, component->repo_name);

    // Log the fetch operation.
    LOG_INFO("Fetching %s %s", component->repo_name, resolved_version);

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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_download_chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, CONFIG_USER_AGENT);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, FETCH_TIMEOUT_SECONDS);

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

    // Validate downloaded file size.
    struct stat file_stat;
    if (stat(output_path, &file_stat) != 0 || file_stat.st_size == 0)
    {
        remove(output_path);
        LOG_ERROR("Download failed: empty or missing file for %s", component->repo_name);
        return -1;
    }

    LOG_INFO("Downloaded %s (%ld bytes)", component->repo_name, (long)file_stat.st_size);

    // Verify the checksum of the downloaded file.
    if (verify_checksum(output_path, component->repo_name, resolved_version, component->repo_name) != 0)
    {
        remove(output_path);
        return -1;
    }

    return 0;
}

int init_fetch(void)
{
    // Initialize the curl library globally.
    CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (result != CURLE_OK)
    {
        return -1;
    }

    return 0;
}

void cleanup_fetch(void)
{
    // Clean up the curl library globally.
    curl_global_cleanup();
}

int fetch_component(
    const Component *component,
    const char *version,
    const char *output_directory
)
{
    // Try local binary first.
    if (copy_local_component(component, output_directory) == 0)
    {
        return 0;
    }

    // Fall back to remote download.
    return download_remote(component, version, output_directory);
}

int fetch_all_components(const char *version, const char *output_directory)
{
    LOG_INFO("Fetching LimeOS components...");

    // Fetch required components.
    for (int i = 0; i < CONFIG_REQUIRED_COMPONENTS_COUNT; i++)
    {
        const Component *component = &CONFIG_REQUIRED_COMPONENTS[i];
        if (fetch_component(component, version, output_directory) != 0)
        {
            LOG_ERROR("Required component failed: %s", component->repo_name);
            return -1;
        }
    }

    // Fetch optional components.
    for (int i = 0; i < CONFIG_OPTIONAL_COMPONENTS_COUNT; i++)
    {
        const Component *component = &CONFIG_OPTIONAL_COMPONENTS[i];
        if (fetch_component(component, version, output_directory) != 0)
        {
            LOG_WARNING("Optional component skipped: %s", component->repo_name);
        }
    }

    LOG_INFO("All required components fetched successfully");

    return 0;
}
