#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <locale.h>
#include <wchar.h>

#define use(x) (void)(x)

struct FileStat
{
    char filename[128];
    char options[8];
    FILE *file;
    long int bytes;
    long int lines;
    long int words;
    long int characters;
};

long int get_file_size(FILE *f)
{
    fseek(f, 0L, SEEK_SET);
    fseek(f, 0L, SEEK_END);
    long int res = ftell(f);
    fseek(f, 0L, SEEK_SET);
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
    fseek(f, 0L, SEEK_SET);
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
    fseek(f, 0L, SEEK_SET);
    return words;
}

long int count_chars(FILE *f)
{
    long int count = 0;
    while (fgetwc(f) != WEOF)
    {
        count++;
    }
    fseek(f, 0L, SEEK_SET);
    return count;
}

void init_file(struct FileStat *fs)
{
    fs->file = fopen(fs->filename, "r, ccs=UTF-8"); // open utf-8 file in read mode
    if (fs->file == NULL)
    {
        printf("File not found: %s\n", fs->filename);
        exit(EXIT_FAILURE);
    }

    if (fwide(fs->file, 1) < 0) // set file orientation to wide char to support multibyte characters
    {
        printf("Error setting file orientation\n");
        exit(EXIT_FAILURE);
    }
}

void get_input(int argc, char *argv[], struct FileStat *fs)
{
    int opt;
    if (argc < 2)
    {
        printf("Usage: %s [-c file] [-l file] [-w file] [-m file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
    {
        strncpy(fs->filename, argv[1], strlen(argv[1]));
        strcpy(fs->options, "clwm");
        printf("bytes\tlines\twords\tcharacter\n");
    }
    else
    {
        int idx = 0;
        strcpy(fs->options, "");
        while ((opt = getopt(argc, argv, "c:l:w:m:")) != -1)
        {
            if (fs->filename[0] == '\0' && optarg)
                strncpy(fs->filename, optarg, strlen(optarg));

            switch (opt)
            {
            case 'c':
                fs->options[idx] = 'c';
                printf("bytes\t");
                break;
            case 'l':
                fs->options[idx] = 'l';
                printf("lines\t");
                break;
            case 'w':
                fs->options[idx] = 'w';
                printf("words\t");
                break;
            case 'm':
                fs->options[idx] = 'm';
                printf("character\t");
                break;
            default:
                printf("Usage: %s [-c file] [-l file] [-w file] [-m file]\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            idx++;
        }
        printf("\n");
    }

    if (fs->filename[0] == '\0')
    {
        printf("Usage: %s [-c file] [-l file] [-w file] [-m file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}
void execute(struct FileStat *fs)
{
    for (int i = 0; i < strlen(fs->options); i++)
    {
        switch (fs->options[i])
        {
        case 'c':
            fs->bytes = get_file_size(fs->file);
            printf("%ld\t", fs->bytes);
            break;
        case 'l':
            fs->lines = count_lines(fs->file);
            printf("%ld\t", fs->lines);
            break;
        case 'w':
            fs->words = count_words(fs->file);
            printf("%ld\t", fs->words);
            break;
        case 'm':
            fs->characters = count_chars(fs->file);
            printf("%ld\t", fs->characters);
            break;
        }
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    int opt;
    struct FileStat fs = {"", "", NULL, 0, 0, 0, 0};

    setlocale(LC_ALL, "en_US.UTF-8"); // set locale to UTF-8
    get_input(argc, argv, &fs);

    init_file(&fs);
    execute(&fs);
    if (fs.file)
        fclose(fs.file);

    return EXIT_SUCCESS;
}
