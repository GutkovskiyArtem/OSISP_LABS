#define _POSIX_C_SOURCE 200809L
#include <stdio.h>

#include <string.h>
#include <dirent.h>

#include <sys/stat.h>
#include <getopt.h>
#include <locale.h>
#include <errno.h>
#include <linux/limits.h>

void print_usage() {
    printf("Использование: dirwalk [dir] [options]\n");
    printf("Опции:\n");
    printf("  -l\tтолько символические ссылки (-type l)\n");
    printf("  -d\tтолько каталоги (-type d)\n");
    printf("  -f\tтолько файлы (-type f)\n");
    printf("  -s\tсортировать выход в соответствии с LC_COLLATE\n");
}

void process_dir(const char* dir_path, int symbolic_link_only, int directory_only, int file_only, int sort_output) {
    DIR* dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Ошибка открытия каталога '%s': %s\n", dir_path, strerror(errno));
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        char entry_path[PATH_MAX];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", dir_path, entry->d_name);

        struct stat entry_stat;
        if (lstat(entry_path, &entry_stat) == -1) {
            fprintf(stderr, "Ошибка получения информации о файле '%s': %s\n", entry_path, strerror(errno));
            continue;
        }

        if ((symbolic_link_only && S_ISLNK(entry_stat.st_mode)) ||
            (directory_only && S_ISDIR(entry_stat.st_mode)) ||
            (file_only && S_ISREG(entry_stat.st_mode))) {
            printf("%s\n", entry_path);
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    setlocale(LC_COLLATE, "");

    int symbolic_link_only = 0;
    int directory_only = 0;
    int file_only = 0;
    int sort_output = 0;

    int opt;
    int options_flag = 0;
    while ((opt = getopt(argc, argv, "ldfs")) != -1) {
        switch (opt) {
            case 'l':
                symbolic_link_only = 1;
                options_flag = 1;
                break;
            case 'd':
                directory_only = 1;
                options_flag = 1;
                break;
            case 'f':
                file_only = 1;
                options_flag = 1;
                break;
            case 's':
                sort_output = 1;
                options_flag = 1;
                break;
            default:
                print_usage();
                return 1;
        }
    }

    const char* dir_path = (optind < argc) ? argv[optind] : ".";

    if (!options_flag) {
        symbolic_link_only = 1;
        directory_only = 1;
        file_only = 1;
    }

    process_dir(dir_path, symbolic_link_only, directory_only, file_only, sort_output);

    return 0;
}