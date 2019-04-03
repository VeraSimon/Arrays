#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct Array
{
    int capacity;    // How many elements can this array hold?
    int count;       // How many states does the array currently hold?
    char **elements; // The string elements contained in the array (it's an array of string pointers!)
} Array;

/************************************
 *
 *   CREATE, DESTROY, RESIZE FUNCTIONS
 *
 ************************************/

/*****
 * Allocate memory for a new array
 *****/
Array *create_array(int capacity)
{
    printf("~~create_array~~\n");

    // Allocate memory for the Array struct
    Array *newArr = malloc(sizeof(Array));

    // Set initial values for capacity and count
    newArr->capacity = capacity;
    newArr->count = 0;

    // Allocate memory for elements. Tests fail if values not initialized to NULL.
    // Good practice to make sure there's no garbage pointers in the mem block anyway.
    newArr->elements = calloc(capacity, sizeof(char *));

    printf("Create Array->capacity: %i\n", newArr->capacity);
    printf("Create Array->count: %i\n", newArr->count);
    printf("Create Array struct size: %li\n", sizeof(newArr)); // ?_?
    printf("Create Array->capacity size: %li\n", sizeof(newArr->capacity));
    printf("Create Array->count size: %li\n", sizeof(newArr->count));
    printf("Create Array->elements size: %li\n\n", sizeof(newArr->elements));

    return newArr;
}

/*****
 * Free memory for an array and all of its stored elements
 *****/
void destroy_array(Array *arr)
{
    printf("~~destroy_array~~\n\n");

    // Free all elements
    // Assumes strings were malloc'ed, such as with `strdup` used in arr_append
    for (int i = 0; i < arr->count; i++)
    {
        free(arr->elements[i]);
    }

    free(arr->elements);

    // Free Array struct
    free(arr);
}

/*****
 * Create a new elements array with double capacity and copy elements
 * from old to new
 *****/
void resize_array(Array *arr)
{
    printf("~~resize_array~~\n");

    // Create a new element storage with double capacity
    char **newElementArr = calloc(2, sizeof(arr));

    // Copy elements into the new storage
    for (int i = 0; i < arr->count; i++)
    {
        newElementArr[i] = arr->elements[i];
    }

    // Free the old elements array (but NOT the strings they point to)
    free(arr->elements);

    // Update the elements and capacity to new values
    arr->capacity *= 2;
    arr->elements = newElementArr;

    printf("Resize Array->capacity: %i\n", arr->capacity);
    printf("Resize Array->count: %i\n", arr->count);
    printf("Resize Array struct size: %li\n", sizeof(arr));
    for (int i = 0; i < arr->count; i++)
    {
        printf("Resize element %i: %s\n", i, arr->elements[i]);
    }
    printf("\n");
}

/************************************
 *
 *   ARRAY FUNCTIONS
 *
 ************************************/

/*****
 * Return the element in the array at the given index.
 *
 * Throw an error if the index is out of range.
 *****/
char *arr_read(Array *arr, int index)
{
    printf("~~arr_read~~\n");

    // Throw an error if the index is greater or equal to than the current count
    if (index >= arr->count && arr->count > 0)
    {
        fprintf(stderr, "Read at index %i out of range\n", index);
        // NULL / 0 / '\0' is an allowable pointer address ^_^
        return NULL;
    }
    else
    {
        printf("arr_read: %s\n", arr->elements[index]);
        for (int i = 0; i < arr->count; i++)
        {
            printf("arr_read element: %s\n", arr->elements[i]);
        }
        printf("\n");

        // Otherwise, return the element at the given index
        return arr->elements[index];
    }
}

/*****
 * Insert an element to the array at the given index
 *****/
void arr_insert(Array *arr, char *element, int index)
{
    printf("~~arr_insert~~\n");

    // Throw an error if the index is greater than the current count
    if (index >= arr->count && arr->count > 0)
    {
        fprintf(stderr, "Insert at index %i out of range\n", index);
        return;
    }

    // Resize the array if the number of elements is over capacity
    if (arr->count >= arr->capacity)
    {
        printf("~arr_insert resize~\n");
        resize_array(arr);
        printf("~arr_insert finish resize~\n");
    }

    // Move every element after the insert index to the right one position
    for (int i = arr->count; i > index; i--)
    {
        arr->elements[i] = arr->elements[i - 1];
    }

    // Copy the element and add it to the array
    char *elementCopy = strdup(element);
    arr->elements[index] = elementCopy;

    // Increment count by 1
    arr->count++;

    printf("arr_insert inserted: %s\n", arr->elements[index]);
    for (int i = 0; i < arr->count; i++)
    {
        printf("arr_insert element %i: %s\n", i, arr->elements[i]);
    }
    printf("\n");
}

/*****
 * Append an element to the end of the array
 *****/
void arr_append(Array *arr, char *element)
{
    printf("~~arr_append~~\n");

    // Resize the array if the number of elements is over capacity
    // or throw an error if resize isn't implemented yet.
    if (arr->count >= arr->capacity)
    {
        printf("~arr_append resize~\n");

        resize_array(arr);
        // fprintf(stderr, "Can't add element %s. Array is full.\n", element);
        // return;

        printf("~arr_append finish resize~\n");
    }

    // Copy the element and add it to the end of the array
    // strdup uses malloc under the hood, so remember to free it after
    char *elementCopy = strdup(element);
    arr->elements[arr->count] = elementCopy;

    printf("arr_append appended: %s\n", arr->elements[arr->count]);

    // Increment count by 1
    arr->count++;

    for (int i = 0; i < arr->count; i++)
    {
        printf("arr_append element %i: %s\n", i, arr->elements[i]);
    }
    printf("\n");
}

/*****
 * Remove the first occurence of the given element from the array,
 * then shift every element after that occurence to the left one slot.
 *
 * Throw an error if the value is not found.
 *****/
void arr_remove(Array *arr, char *element)
{
    printf("~~arr_remove~~\n");

    // Search for the first occurence of the element and remove it.
    // Don't forget to free its memory!
    int lastElement = arr->count - 1;
    // Saving for after the search loop so we know when to stop shifting elements down
    int index = 0;
    for (; index < arr->count; index++)
    {
        if (*arr->elements[index] == *element)
        {
            free(arr->elements[index]);
            break;
        }

        // In case the passed element isn't in the array...
        if (&arr->elements[index] == &arr->elements[lastElement] && arr->elements[index] != arr->elements[lastElement])
        {
            fprintf(stderr, "Element \"%s\" not in the array!\n", element);
            return;
        }
    }

    // Shift over every element after the removed element to the left one position
    for (int i = index; i < arr->count - 1; i++)
    {
        arr->elements[i] = arr->elements[i + 1];
    }
    // NULL out the original last element so it doesn't look like we have a dupe
    arr->elements[arr->count] = NULL;

    // Decrement count by 1
    arr->count--;

    for (int i = 0; i < arr->count; i++)
    {
        printf("arr_remove element %i: %s\n", i, arr->elements[i]);
    }
    printf("\n");
}

/*****
 * Utility function to print an array.
 *****/
void arr_print(Array *arr)
{
    printf("[");
    for (int i = 0; i < arr->count; i++)
    {
        printf("%s", arr->elements[i]);
        if (i != arr->count - 1)
        {
            printf(",");
        }
    }
    printf("]\n");
}

#ifndef TESTING
int main(void)
{

    Array *arr = create_array(1);

    arr_insert(arr, "STRING1", 0);
    arr_append(arr, "STRING4");
    arr_insert(arr, "STRING2", 0);
    arr_insert(arr, "STRING3", 1);
    arr_print(arr);
    arr_remove(arr, "STRING3");
    arr_print(arr);

    destroy_array(arr);

    return 0;
}
#endif
