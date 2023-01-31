#include <stdio.h>
#include "slist.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#define DEBUG
typedef enum {CHAR = 0, STRING} dbllist_data_type;

/** Initialize a double linked list
	\param list - the list to initialize */
void dbllist_init(dbllist_t *list){
    dbllist_head(list) = NULL;
    dbllist_tail(list) = NULL;
    dbllist_size(list) = 0;
}

/** Destroy and de-allocate the memory hold by a list
	\param list - a pointer to an existing list
	\param dealloc flag that indicates whether stored data should also be de-allocated */
void dbllist_destroy(dbllist_t * list,dbllist_destroy_t dealloc){
    while (dbllist_head(list) != NULL){
        dbllist_remove(list,dbllist_head(list), dealloc);
    }
    dbllist_head(list) = NULL;
    dbllist_tail(list) = NULL;
}


/** Append data to list (add as last node of the list)
	\param list - a pointer to a list
	\param data - the data to place in the list
	\return 0 on success, or -1 on failure */
int dbllist_append(dbllist_t *list,void *data){
    dbllist_node_t* new = (dbllist_node_t*) malloc(sizeof(dbllist_node_t));
    if (new == NULL){
        return -1;
    }
    dbllist_data(new) = data;
    dbllist_next(new) = NULL;
    dbllist_prev(new) = dbllist_tail(list);
    if (dbllist_tail(list) == NULL){
        dbllist_head(list) = new;
        dbllist_size(list) = 0;
    }
    else{
        if (list->tail == NULL){
            printf("tail 1\n");
        }
        if (list->tail->next == NULL){

        }
        dbllist_next(dbllist_tail(list)) = new;
    }
    dbllist_tail(list) = new;
    dbllist_size(list)++;
    return 0;
}

/** Prepend data to list (add as first node of the list)
	\param list - a pointer to list
	\param data - the data to place in the list
	\return 0 on success, or -1 on failure
*/
int dbllist_prepend(dbllist_t *list,void *data){
    dbllist_node_t* new = (dbllist_node_t*) malloc(sizeof(dbllist_node_t));
    if (new == NULL){
        return -1;
    }
    dbllist_data(new) = data;
    dbllist_prev(new) = NULL;
    dbllist_next(new) = dbllist_head(list);
    if (dbllist_head(list) == NULL){
        dbllist_tail(list) = new;\
        dbllist_size(list) = 0;
    }
    else{
        dbllist_prev(dbllist_head(list)) = new;
    }
    dbllist_head(list) = new;
    dbllist_size(list)++;
    return 0;
}
/** \brief Remove the specific node from the list.
	\param to a pointer to the list
	\param pointer to the node that should be removed.
	\param dealloc flag that indicates whether to de-allocated the data in the node
	\return 0 on success, or -1 on failure
*/

int dbllist_remove(dbllist_t *list, dbllist_node_t* node,dbllist_destroy_t d){
    dbllist_node_t *it;
    for (it = dbllist_head(list); it != node; it = dbllist_next(it));
    if (it == NULL){
        // error: item not found
        return -1;
    }
    if (dbllist_size(list) <= 1){
        dbllist_head(list) = NULL;
        dbllist_tail(list) = NULL;
    }
    else{
        if (dbllist_prev(it) == NULL){
            dbllist_head(list) = dbllist_next(it);
            dbllist_prev(dbllist_next(it)) = NULL;
        }
        else{
            dbllist_next(dbllist_prev(it)) = dbllist_next(it);
        }
        if (dbllist_next(it) == NULL){
            dbllist_tail(list) = dbllist_prev(it);
            dbllist_next(dbllist_prev(it)) = NULL;
        }
        else{
            dbllist_prev(dbllist_next(it)) = dbllist_prev(it);
        }
    }

    dbllist_prev(it) = NULL;
    dbllist_next(it) = NULL;
    if (d == DBLLIST_FREE_DATA){
        free(dbllist_data(it));
    }
    dbllist_size(list)--;
    free(it);
    return 0;
}

/** \brief prints the list
    \param list - a pointer to the list
    \param type - the data type
 */

void dbllist_print(dbllist_t *list, dbllist_data_type type){
    printf("list size: %d\nlist items: ",dbllist_size(list));

    for (dbllist_node_t* it = dbllist_head(list); it != NULL; it = dbllist_next(it)){
        if (type == CHAR){
            printf("%c", *(char*)dbllist_data(it));
        }
        else if(type == STRING){
            printf("%s", (char*)dbllist_data(it));
        }
        printf("->");
    }
    printf("NULL\n");
}
