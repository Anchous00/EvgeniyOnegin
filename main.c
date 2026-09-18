//#include "TXLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

void mem_swap( void *x_address, void *y_address, size_t element_size );
void quick_sort( void *arr, size_t len, size_t element_size,  int (*Comparator)( const void *a, const void *b ));
size_t sort( void *arr, size_t len, size_t element_size, int (*Comparator)( const void *a, const void *b ));
void printarrstr( void *arr, size_t len );
int my_strcmp_up( const void *str1_p, const void *str2_p );
int my_strcmp_down( const void *str1_p, const void *str2_p );
char* my_fgets( char *str, size_t n, FILE *stream );
size_t my_strlen( const char *str );
size_t input_text_from_file( const char *file_name, char **buffer );
size_t file_size( const char* filename );

int main( void )
{
    //printf("start\n");
    const char *file_name = "test.txt";
    char *text = 0;
    size_t symbcnt = input_text_from_file(file_name, &text);

    if (!freopen("text.txt", "w", stdout))
    {
        printf("ERROR: cant open file");
        return 1;
    }

    //putchar('a');
    size_t len = 1;
    for (size_t i = 0; i < symbcnt; i++)
    {
        if (text[i] == '\n') len++;
        if (text[i] == '\0') break;
    }
    //printf("len: %zd\n", len);

    char **arr = (char**)calloc(len, sizeof(char*));
    char **arrreserv = (char**)calloc(len, sizeof(char*));

    arr[0] = &text[0];
    arrreserv[0] = &text[0];
    for (size_t i = 0, j = 0; i < symbcnt; i++)
    {
        if (text[i] == '\n' && j < len - 1)
        {
            j++;
            text[i] = '\0';
            //printf("<%s>", &text[i]);
            arr[j] = &text[i + 1];
            arrreserv[j] = &text[i + 1];
        }
    }

    printf("sort by endings\n");
    quick_sort(arr, len, sizeof(arr[0]), my_strcmp_down);
    printarrstr(arr, len);

    printf("sort by starts\n");
    qsort(arr, len, sizeof(arr[0]), my_strcmp_up);
    printarrstr(arr, len);

    //printf("len %zd arr[0] %s\n", len, *arr);
    printf("original\n");
    printarrstr(arrreserv,  len);

    fclose(stdout);
    free(text);
    free(arr);
    free(arrreserv);

    return 0;
}

size_t input_text_from_file( const char *file_name, char **buffer )
{
    FILE *input_file = fopen(file_name, "r");
    
    if (input_file == NULL)
    {
        printf("ERROR: cant open file");
        return 0;
    }
    const size_t FILE_SIZE = file_size(file_name);

    *buffer = (char*)calloc(FILE_SIZE + 1, sizeof(char));
    if (*buffer == NULL)
        return 0;
    //printf("FILE_SIZE %ld\n", FILE_SIZE);
    size_t symbcnt = fread(*buffer, sizeof(char), FILE_SIZE, input_file);
    //printf("succesful read %zd sumbols\n", symbcnt);
    //printf("<%s>, <%c>", *buffer, *buffer[symbcnt - 1]);
    (*buffer)[symbcnt - 1] = '\0';
    //printf("symbcnt %zd", symbcnt);
    fclose(input_file);

    return symbcnt;
}


size_t file_size( const char* filename )
{
    struct stat input_file_stat = {};

    if (stat(filename, &input_file_stat) == -1)
        return 0;

    return (size_t)input_file_stat.st_size;
}
void quick_sort( void *arr, size_t len, size_t element_size, int (*Comparator)( const void *a, const void *b ))
{
    if (len <= 1)
        return;
    size_t x = sort(arr, len, element_size, Comparator);

    quick_sort(arr, x, element_size, Comparator);
    quick_sort((char*)arr + (x + 1) * element_size, len - x - 1, element_size, Comparator);

    return;
}

size_t sort( void *arr, size_t len, size_t element_size, int (*Comparator)( const void *a, const void *b ))
{
    //printarrint(arr, len);
    //size_t x = (size_t)(*((char*)arr + element_size * (len - 1)));
    //printf("first  : %d last : %d x : %d\n", first, last, x);
    size_t i = 0;

    for (size_t j = 0; j < len - 1; j++)
    {
        //if (x > (size_t)(*((char*)arr + j * element_size)))
        if (Comparator((char*)arr + element_size * (len - 1), (char*)arr + j * element_size) > 0)
        {
            //printf(" %lg > %lg\n",  (double)(*((char*)arr + (len - 1) * element_size)), (double)(*((char*)arr + j * element_size)));
            mem_swap((char*)arr + i * element_size, (char*)arr + j * element_size, element_size);
            i = i + 1;
            //printarrdouble(arr, len);
        }
    }
    i = i - 1;
    // printf("swaping i: %d  last:%d\n", i + 1, last);
    mem_swap((char*)arr + (i + 1) * element_size, (char*)arr + (len - 1) * element_size, element_size);
    // printarrint(arr, len);
    // printf("\n");
    return i + 1;
}

void printarrstr( void *arr, size_t len )
{
    for (size_t i = 0; i < len; i++)
        printf("%s\n", *((char**)arr + i));
    printf("\n");

    return;
}

void mem_swap( void *x_address, void *y_address, size_t element_size )
{
    char *x = (char*)x_address, *y = (char*)y_address;
    for (size_t i = 0; i < element_size; i++)
    {
        char t = *(x + i);
        *(x + i) = *(y + i);
        *(y + i) = t;
    }

    return;
}

int my_strcmp_up( const void *str1_p, const void *str2_p )
{
    const char *str1 = *(const char* const*)str1_p, *str2 = *(const char* const*)str2_p;
    size_t i = 0, j = 0;
    if (str1 == NULL || str2 == NULL)
    {
        puts("ERROR: invalid string in function my_strcmp");
        return EOF;
    }

    while (str1[i] != '\0' && str2[j] != '\0')
    {
        //printf("debug\n");
        while (!isalpha(str1[i]) && str1[i] != '\0')
        {
            i++;
        }

        while (!isalpha(str2[j]) && str2[j] != '\0')
        {
            j++;
        }
        if (tolower((int)(str1[i])) != tolower((int)(str2[j])))
            return tolower((int)(str1[i])) - tolower((int)(str2[j]));
        i++;
        j++;
    }

    return str1[i] - str2[j];
}

int my_strcmp_down( const void *str1_p, const void *str2_p )
{
    const char *str1 = *(const char* const*)str1_p, *str2 = *(const char* const*)str2_p;

    if (str1 == NULL || str2 == NULL)
    {
        puts("ERROR: invalid string in function my_strcmp");
        return EOF;
    }

    size_t i = my_strlen(str1);
    size_t j = my_strlen(str2);

    while (i > 0 && j > 0)
    {
        //printf("debug\n");
        while (!isalpha(*(str1 + i)) && i > 0)
        {
            i--;
        }

        while (!isalpha(*(str2 + j)) && j > '\0')
        {
            j--;
        }
        if (tolower((int)(*(str1 + i))) != tolower((int)(*(str2 + j))))
            return tolower((int)(*(str1 + i))) - tolower((int)(*(str2 + j)));
        i--;
        j--;
    }

    return *(str1 + i) - *(str2 + j);
}

char* my_fgets( char *str, size_t n, FILE *stream )
{
    if (str == NULL)
    {
        puts("ERROR: invalid string in function my_fgets");
        return NULL;
    }

    if (stream == NULL)
    {
        puts("ERROR: invalid file stream in function my_fgets");
        return str;
    }

    for (size_t i = 0; i < n; i++)
    {
        str[i] = (char)fgetc(stream);
        if (str[i] == EOF)
        {
            str[i] = '\0';
            return str;
        }
        if (str[i] == '\n')
        {
            str[i] = '\n';
            str[i + 1] = '\0';
            return str;
        }

        if (i == n - 1)
            str[i] = '\0';
    }

    return str;
}

size_t my_strlen( const char *str )
{
    if (str == NULL)
    {
        puts("ERROR: invalid string in function my_strlen");
        return 0;
    }

    size_t len = 0;

    while (str[len] != '\0')
        len++;

    return len;
}
