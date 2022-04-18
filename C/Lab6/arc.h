void set_prog_name(const char *name);
void create_archive(const char *arcname, int count, char **files);
void list_files(const char *arcname);
void extract_files(const char *arcname);
void append_files(const char *arcname, int count, char **files);