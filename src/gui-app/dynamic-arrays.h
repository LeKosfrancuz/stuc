#ifndef DA_H_
#define DA_H_

#define DA_INIT_CAP 256

// Append an item to a dynamic array
#define da_append(da, item)                                                          \
    do {                                                                                 \
        if ((da)->count >= (da)->capacity) {                                             \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2;       \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items)); \
            assert((da)->items != NULL && "Buy more RAM lol");                       \
        }                                                                                \
                                                                                         \
        (da)->items[(da)->count++] = (item);                                             \
    } while (0)

#define da_free(da) free((da).items)

// Append several items to a dynamic array
#define da_append_many(da, new_items, new_items_count)                                  \
    do {                                                                                    \
        if ((da)->count + new_items_count > (da)->capacity) {                               \
            if ((da)->capacity == 0) {                                                      \
                (da)->capacity = DA_INIT_CAP;                                               \
            }                                                                               \
            while ((da)->count + new_items_count > (da)->capacity) {                        \
                (da)->capacity *= 2;                                                        \
            }                                                                               \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items));        \
            assert((da)->items != NULL && "Buy more RAM lol");                              \
        }                                                                                   \
        memcpy((da)->items + (da)->count, new_items, new_items_count*sizeof(*(da)->items)); \
        (da)->count += new_items_count;                                                     \
    } while (0)

#define da_insert(da, i, item)                                                           \
    do {                                                                                 \
        if ((da)->count >= (da)->capacity) {                                             \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2;       \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items));     \
            assert((da)->items != NULL && "Buy more RAM lol");                           \
        }                                                                                \
        for (size_t __el_id = (da)->count; __el_id > (size_t)(i); __el_id--) {           \
            memcpy(&(da)->items[__el_id], &(da)->items[__el_id-1], sizeof((da)->items[0]));\
        }                                                                                \
        (da)->items[(i)] = (item);                                                       \
        (da)->count++;                                                                   \
    } while (0)

#define da_remove(da, i)                                                           \
    do {                                                                                 \
        (da)->count--;                                                                   \
        for (size_t __el_id = (size_t)(i); __el_id < (da)->count; __el_id++) {           \
            memcpy(&(da)->items[__el_id], &(da)->items[__el_id+1], sizeof((da)->items[0]));\
        }                                                                                \
    } while (0)
/* Code snippet author:
Copyright 2021 Alexey Kutepov <reximkut@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#endif // DA_H_
