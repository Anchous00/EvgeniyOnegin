#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include <sys/stat.h>

const char *FILE_NAME = "Onegin.txt";
//const char *FILE_NAME = "test.txt";
//const char *FILE_NAME = "WarAndPeace.txt";

struct line
{
    char *index;
    size_t len;
};

#define FREE(x, len) while (len-- > 0) *((char*)x + len) = '~'; free(x);

size_t input_text_from_file( const char *file_name, char **buffer  );
size_t str_amount          ( const char *text,      size_t symbcnt );
ssize_t file_size           ( const char *filename                  );
void   print_text_in_file  ( char** index,          size_t len     );

void divide_text ( char* text, size_t symbcnt, char **index, size_t len );

void quick_sort( void *arr, size_t len, size_t element_size, int (*Comparator)( const void *a, const void *b ));
size_t sort    ( void *arr, size_t len, size_t element_size, int (*Comparator)( const void *a, const void *b ));

void mem_swap( void *x_address, void *y_address, size_t element_size );

int my_strcmp_up   ( const void *str1_p, const void *str2_p );
int my_strcmp_down ( const void *str1_p, const void *str2_p );
int my_ptrcmp_up   ( const void *x,      const void *y      );

void printarrstr( void *arr, size_t len );


int main( void )
{
    char *text = NULL;
    size_t symbcnt = input_text_from_file(FILE_NAME, &text);

    size_t stringcnt = str_amount(text, symbcnt);

    char **index = (char**)calloc(stringcnt, sizeof(char*));
    if (index == NULL)
    {
        fprintf(stderr, "cant allocate memory\n");
        return 0;
    }
    divide_text(text, symbcnt, index, stringcnt);
    if (!freopen("text.txt", "w", stdout))
    {
        fprintf(stderr, "ERROR: cant open file");
        return 1;
    }

    print_text_in_file(index, stringcnt);
    if (fclose(stdout) == -1)
        fprintf(stderr, "can't close output file");

    FREE(text, symbcnt);
    FREE(index, stringcnt);

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

    const ssize_t FILE_SIZE = file_size(file_name);

    *buffer = (char*)calloc((size_t)FILE_SIZE + 1, sizeof(char));
    if (*buffer == NULL)
        return 0;
    //printf("FILE_SIZE %ld\n", FILE_SIZE);
    size_t symbcnt = fread(*buffer, sizeof(char), (size_t)FILE_SIZE, input_file);
    (*buffer)[symbcnt - 1] = '\0';

    if (fclose(input_file) == -1)
        fprintf(stderr, "can't close input file\n");

    return symbcnt;
}

size_t str_amount( const char* text, size_t symbcnt )
{
    size_t stringcnt = 1;
    for (size_t i = 0; i < symbcnt; i++)
    {
        if (text[i] == '\n') stringcnt++;
        if (text[i] == '\0') break;
    }

    return stringcnt;
}

ssize_t file_size( const char* filename )
{
    struct stat input_file_stat = {};

    if (stat(filename, &input_file_stat) == -1)
        return -1;

    return input_file_stat.st_size;
}

void print_text_in_file( char** index, size_t len )
{
    clock_t t0 = clock();
    qsort(index, len, sizeof(*index), my_strcmp_down);
    clock_t t1 = clock();
    printarrstr(index, len); // snake_case: print_arr_str; lower camelCase: printArrStr; Upper CamelCase: PrintArrStr

    clock_t t2 = clock();
    quick_sort(index, len, sizeof(*index), my_strcmp_up);
    clock_t t3 = clock();
    printarrstr(index, len);

    quick_sort(index, len, sizeof(*index), my_ptrcmp_up);
    printarrstr(index,  len);
    clock_t t4 = clock();

#ifdef TIME
    fprintf(stderr, "dt1 = %lg, dt2 = %lg, t0 = %lg\n",
          (double)(t1 - t0) / CLOCKS_PER_SEC,
          (double)(t3 - t2) / CLOCKS_PER_SEC,
          (double)(t4 - t0) / CLOCKS_PER_SEC);
#endif
}

void divide_text( char* text, size_t symbcnt, char **index, size_t len )
{
    index[0] = &text[0];

    for (size_t i = 0, j = 0; i < symbcnt; i++)
    {
        if (text[i] == '\n' && j < len - 1)
        {
            j++;
            text[i] = '\0';
            index[j] = &text[i + 1];
        }
    }
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
    mem_swap((char*)arr + (i + 1) * element_size,
             (char*)arr + (len - 1) * element_size,
             element_size);
    // printarrint(arr, len);
    // printf("\n");
    return i + 1;
}

void mem_swap(void *x_address, void *y_address, size_t element_size)
{
    char *x = (char *)x_address;
    char *y = (char *)y_address;

    while (element_size >= 8)
    {
        int64_t t;
        memcpy(&t, x, sizeof t);
        memcpy(x, y, sizeof t);
        memcpy(y, &t, sizeof t);

        x += 8;
        y += 8;
        element_size -= 8;
    }

    while (element_size >= 4)
    {
        int32_t t;
        memcpy(&t, x, sizeof t);
        memcpy(x, y, sizeof t);
        memcpy(y, &t, sizeof t);

        x += 4;
        y += 4;
        element_size -= 4;
    }

    while (element_size >= 2)
    {
        int16_t t;
        memcpy(&t, x, sizeof t);
        memcpy(x, y, sizeof t);
        memcpy(y, &t, sizeof t);

        x += 2;
        y += 2;
        element_size -= 2;
    }

    if (element_size)
    {
        char t = *x;
        *x = *y;
        *y = t;
    }
}

int my_strcmp_up( const void *str1_p, const void *str2_p )
{
    const char *str1 = *(const char* const*)str1_p;
    const char *str2 = *(const char* const*)str2_p;
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

    size_t i = strlen(str1);
    size_t j = strlen(str2);

    while (i > 0 && j > 0)
    {
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
        if (i > 0) i--;
        if (j > 0) j--;
    }

    return *(str1 + i) - *(str2 + j);
}

int my_ptrcmp_up( const void *x, const void *y )
{
    long long val = *((const intptr_t*)x) - *((const intptr_t*)y);
    return val > 0 ? 1 : val == 0 ? 0 : -1;
}

void printarrstr( void *arr, size_t len )
{
    for (size_t i = 0; i < len; i++)
        printf("%s\n", *((char**)arr + i));
    printf("\n");

    return;
}
