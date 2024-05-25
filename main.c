#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif
#include "md4c-html.h"

#define SRC_DIR "src"
#define GENERATED_DIR "_generated"

void my_process_output(const MD_CHAR *output, MD_SIZE size, void *userdata)
{
    FILE *output_file = (FILE *)userdata;
    fwrite(output, 1, size, output_file);
}

void convert_markdown_to_html(const char *input_path, const char *output_path)
{
    FILE *input_file = fopen(input_path, "rb");
    if (!input_file)
    {
        perror("Failed to open input file");
        return;
    }

    fseek(input_file, 0, SEEK_END);
    size_t input_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    char *input_buffer = (char *)malloc(input_size + 1);
    if (!input_buffer)
    {
        perror("Failed to allocate memory for input buffer");
        fclose(input_file);
        return;
    }

    size_t read_size = fread(input_buffer, 1, input_size, input_file);
    fclose(input_file);

    if (read_size != input_size)
    {
        perror("Failed to read input file");
        free(input_buffer);
        return;
    }
    input_buffer[input_size] = '\0';

    FILE *output_file = fopen(output_path, "w");
    if (!output_file)
    {
        perror("Failed to open output file");
        free(input_buffer);
        return;
    }

    fprintf(output_file, "<!DOCTYPE html>\n");
    fprintf(output_file, "<html>\n");
    fprintf(output_file, "<head>\n");
    fprintf(output_file, "<meta charset=\"UTF-8\">\n");
    fprintf(output_file, "<title>%s</title>\n", input_path);
    fprintf(output_file, "</head>\n");
    fprintf(output_file, "<body>\n");

    if (md_html(input_buffer, (MD_SIZE)input_size, my_process_output, output_file, MD_FLAG_COLLAPSEWHITESPACE, 0) != 0)
    {
        fprintf(stderr, "Failed to parse markdown file %s\n", input_path);
    }

    fprintf(output_file, "</body>\n");
    fprintf(output_file, "</html>\n");

    fclose(output_file);
    free(input_buffer);
}

int main()
{
#ifdef _WIN32
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(SRC_DIR "\\*.md", &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("FindFirstFile failed (%d)\n", GetLastError());
        return 1;
    }

    _mkdir(GENERATED_DIR);

    do
    {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            char input_path[256];
            snprintf(input_path, sizeof(input_path), "%s\\%s", SRC_DIR, findFileData.cFileName);

            // Set output file name, removing the ".md" extension
            char output_path[256];
            snprintf(output_path, sizeof(output_path), "%s\\%s", GENERATED_DIR, findFileData.cFileName);
            char *dot = strrchr(output_path, '.');
            if (dot && !strcmp(dot, ".md"))
            {
                *dot = '\0';
            }
            strcat(output_path, ".html");

            convert_markdown_to_html(input_path, output_path);
            printf("Converted %s to %s\n", input_path, output_path);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
#else
    struct dirent *entry;
    DIR *dp = opendir(SRC_DIR);

    if (dp == NULL)
    {
        perror("opendir");
        return 1;
    }

    mkdir(GENERATED_DIR, 0755);

    while ((entry = readdir(dp)))
    {
        if (entry->d_type == DT_REG)
        {
            char input_path[256];
            snprintf(input_path, sizeof(input_path), "%s/%s", SRC_DIR, entry->d_name);

            // Set output file name, removing the ".md" extension
            char output_path[256];
            snprintf(output_path, sizeof(output_path), "%s/%s", GENERATED_DIR, entry->d_name);
            char *dot = strrchr(output_path, '.');
            if (dot && !strcmp(dot, ".md"))
            {
                *dot = '\0';
            }
            strcat(output_path, ".html");

            convert_markdown_to_html(input_path, output_path);
            printf("Converted %s to %s\n", input_path, output_path);
        }
    }

    closedir(dp);
#endif
    return 0;
}
