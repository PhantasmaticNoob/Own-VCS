int object_write(ObjectType type, const void *data, size_t len, ObjectID *id_out) {
    const char *type_str = NULL;
    char header[64];
    void *full_obj = NULL;
    size_t header_len, full_len;
    char final_path[512], shard_dir[512], temp_path[512];
    int fd = -1, dirfd = -1;
    ssize_t written_total = 0;

    if (!data || !id_out) return -1;

    switch (type) {
        case OBJ_BLOB:   type_str = "blob";   break;
        case OBJ_TREE:   type_str = "tree";   break;
        case OBJ_COMMIT: type_str = "commit"; break;
        default: return -1;
    }

    // Build header: "<type> <size>\0"
    int n = snprintf(header, sizeof(header), "%s %zu", type_str, len);
    if (n < 0 || (size_t)n >= sizeof(header) - 1) return -1;
    header_len = (size_t)n + 1;  // include '\0'

    // Build full object = header + data
    full_len = header_len + len;
    full_obj = malloc(full_len);
    if (!full_obj) return -1;

    memcpy(full_obj, header, header_len);
    memcpy((char *)full_obj + header_len, data, len);

    // Compute hash of full object
    compute_hash(full_obj, full_len, id_out);

    // Deduplication
    if (object_exists(id_out)) {
        free(full_obj);
        return 0;
    }

    // Final object path
    object_path(id_out, final_path, sizeof(final_path));

    // Extract shard directory from final path
    strncpy(shard_dir, final_path, sizeof(shard_dir) - 1);
    shard_dir[sizeof(shard_dir) - 1] = '\0';
    char *last_slash = strrchr(shard_dir, '/');
    if (!last_slash) {
        free(full_obj);
        return -1;
    }
    *last_slash = '\0';

    // Create shard directory if needed
    if (mkdir(shard_dir, 0755) != 0 && access(shard_dir, F_OK) != 0) {
        free(full_obj);
        return -1;
    }

    // Temp file in same shard directory
    if (snprintf(temp_path, sizeof(temp_path), "%s/.tmpXXXXXX", shard_dir) >= (int)sizeof(temp_path)) {
        free(full_obj);
        return -1;
    }

    fd = mkstemp(temp_path);
    if (fd < 0) {
        free(full_obj);
        return -1;
    }

    // Write full object
    while ((size_t)written_total < full_len) {
        ssize_t w = write(fd, (char *)full_obj + written_total, full_len - (size_t)written_total);
        if (w <= 0) {
            close(fd);
            unlink(temp_path);
            free(full_obj);
            return -1;
        }
        written_total += w;
    }

    // Flush file contents
    if (fsync(fd) != 0) {
        close(fd);
        unlink(temp_path);
        free(full_obj);
        return -1;
    }

    if (close(fd) != 0) {
        unlink(temp_path);
        free(full_obj);
        return -1;
    }
    fd = -1;

    // Atomic rename into place
    if (rename(temp_path, final_path) != 0) {
        unlink(temp_path);
        free(full_obj);
        return -1;
    }

    // fsync shard directory to persist rename
    dirfd = open(shard_dir, O_RDONLY | O_DIRECTORY);
    if (dirfd >= 0) {
        fsync(dirfd);
        close(dirfd);
    }

    free(full_obj);
    return 0;
}
