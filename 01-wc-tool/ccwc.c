#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <locale.h>
#include <wchar.h>

#define use(x) (void)(x)

struct word_count
{
    FILE *file;
    long int bytes;
    long int lines;
    long int words;
    long int characters;
    char options[16];
};

long int get_file_size(FILE *f)
{
    fseek(f, 0L, SEEK_END);
    long int res = ftell(f);
    return res;
}

long int count_lines(FILE *f)
{
    long int lines = 0;
    char c;
    for (char c = fgetc(f); c != EOF; c = fgetc(f))
    {
        if (c == '\n')
        {
            lines++;
        }
    }
    return lines;
}

long int count_words(FILE *f)
{
    long int words = 0;
    bool is_word = false;

    char c = fgetc(f);
    while (c != EOF)
    {
        if (isspace(c))
        {
            is_word = false;
        }
        else if (!is_word)
        {
            is_word = true;
            words++;
        }
        c = fgetc(f);
    }
    return words;
}

long int count_chars(FILE *f)
{
    long int count = 0;
    while (fgetwc(f) != WEOF)
    {
        count++;
    }
    return count;
}

int main(int argc, char *argv[])
{
    int opt;
    struct word_count wc;

    while ((opt = getopt(argc, argv, "c:l:w:m:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            wc.file = fopen(optarg, "r, ccs=UTF-8");
            if (wc.file == NULL)
            {
                printf("File not found\n");
                exit(EXIT_FAILURE);
            }
            wc.bytes = get_file_size(wc.file);
            printf("Number of bytes: %ld\n", wc.bytes);
            break;
        case 'l':
            wc.file = fopen(optarg, "r, ccs=UTF-8");
            if (wc.file == NULL)
            {
                printf("File not found\n");
                exit(EXIT_FAILURE);
            }
            use(optarg);

            wc.lines = count_lines(wc.file);
            printf("Number of lines: %ld\n", wc.lines);
            break;
        case 'w':
            wc.file = fopen(optarg, "r, ccs=UTF-8");
            if (wc.file == NULL)
            {
                printf("File not found\n");
                exit(EXIT_FAILURE);
            }
            wc.words = count_words(wc.file);
            printf("Number of words: %ld\n", wc.words);
            break;
        case 'm':
            setlocale(LC_ALL, "en_US.UTF-8");
            wc.file = fopen(optarg, "r, ccs=UTF-8");
            if (wc.file == NULL)
            {
                printf("File not found\n");
                exit(EXIT_FAILURE);
            }
            if (fwide(wc.file, 1) < 0)
            {
                printf("Error setting file orientation\n");
                exit(EXIT_FAILURE);
            }
            wc.characters = count_chars(wc.file);
            printf("Number of characters: %ld\n", wc.characters);
            break;
        default:
            printf("Usage: ccwc -c <filename>\n");
            exit(EXIT_FAILURE);
        }
    }

    if (wc.file)
        fclose(wc.file);

    return EXIT_SUCCESS;
}
