//#include "TXLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void mem_swap( void *x_address, void *y_address, size_t element_size );
void quick_sort( void *arr, size_t len, size_t element_size,  int (*Comparator)( const void *a, const void *b ));
size_t sort( void *arr, size_t len, size_t element_size, int (*Comparator)( const void *a, const void *b ));
void printarrstr( void *arr, size_t len );
int my_strcmp_up( const void *str1_p, const void *str2_p );
int my_strcmp_down( const void *str1_p, const void *str2_p );
char* my_fgets( char *str, size_t n, FILE *stream );
size_t my_strlen( const char *str );

const size_t FILE_SIZE = 300000;

int main( void )
{
    FILE *file = fopen("onegin_j.txt_Ascii.txt", "r");
    //FILE *file = fopen("test.txt", "r");
    if (file == NULL)
    {
        printf("ERROR: cant open file");
        return 1;
    }
    char text[FILE_SIZE] = {};
    size_t symbcnt = fread(text, 1, sizeof(text) - 1, file);
    text[symbcnt - 1] = '\0';
    printf("sizeof text %zd, symbcnt %zd", sizeof(text), symbcnt);
    fclose(file);

    if (!freopen("text.txt", "w", stdout))
    {
        printf("ERROR: cant open file");
        return 1;
    }

    //printf("reopened file\n");
    //putchar('a');
    size_t len = 1;
    for (size_t i = 0; i < symbcnt; i++)
    {
        if (text[i] == '\n') len++;
        if (text[i] == '\0') break;
    }
    printf("len: %zd\n", len);

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

    quick_sort(arr, len, sizeof(arr[0]), my_strcmp_down);
    printarrstr(arr, len);
    qsort(arr, len, sizeof(arr[0]), my_strcmp_down);
    printf("len %d arr[0] %s\n", len, *arr);
    printarrstr(arr, len);
    printarrstr(arrreserv,  len);
    //fclose(stdout);

    free(arr);
    free(arrreserv);

    return 0;
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
    size_t i = - 1;
    for (size_t j = 0; j < len - 1; j++)
    {
        //if (x > (size_t)(*((char*)arr + j * element_size)))
        if (Comparator((char*)arr + element_size * (len - 1), (char*)arr + j * element_size) > 0)
        {
            //printf(" %lg > %lg\n",  (double)(*((char*)arr + (len - 1) * element_size)), (double)(*((char*)arr + j * element_size)));
            i = i + 1;
            mem_swap((char*)arr + i * element_size, (char*)arr + j * element_size, element_size);
            //printarrdouble(arr, len);
        }
    }

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
    char *str1 = *(char**)str1_p, *str2 = *(char**)str2_p;

    if (str1 == NULL || str2 == NULL)
    {
        puts("ERROR: invalid string in function my_strcmp");
        return EOF;
    }

    while (*str1 != '\0' && *str2 != '\0')
    {
        //printf("debug\n");
        while (!isalpha(*str1) && *str1 != '\0')
        {
            str1++;
        }

        while (!isalpha(*str2) && *str2 != '\0')
        {
            str2++;
        }
        if (tolower((int)(*str1)) != tolower((int)(*str2)))
            return tolower((int)(*str1)) - tolower((int)(*str2));
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

int my_strcmp_down( const void *str1_p, const void *str2_p )
{
    char *str1 = *(char**)str1_p, *str2 = *(char**)str2_p;

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
