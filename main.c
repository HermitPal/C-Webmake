#include <stdio.h>
#include <string.h>
#include "md4c.h"

void my_text_callback(MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size, void *userdata)
{
    fwrite(text, 1, size, stdout);
}

int main()
{
    const char *markdown = "# Hello, MD4C\nThis is a simple markdown parser.";
    MD_PARSER parser = {0};
    parser.text = my_text_callback;
    md_parse(markdown, strlen(markdown), &parser, NULL);
    return 0;
}
