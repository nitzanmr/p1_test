#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "pattern_matching.h"
#include "slist.h"


typedef struct pm_output {
    unsigned char* string;
    unsigned int pos;
}pm_output;

/* destroys the state pointed by parameter "state" and all its transition states */
void pm_destroy_state(pm_state_t* state);

/* Adds a state to db in depth stated by parameter "depth"*/
pm_state_t* pm_add_state(pm_t * db, _size_t depth);

/* Initializes the fsm parameters (the fsm itself should be allocated).  Returns 0 on success, -1 on failure.
*  this function should init zero state
*/
int pm_init(pm_t * db){
    db->newstate = 0;
    db->zerostate = pm_add_state(db, 0);
    db->zerostate->fail = NULL;
    return 0;
}

/* Adds a new string to the fsm, given that the string is of length n.
   Returns 0 on success, -1 on failure.*/
int pm_addstring(pm_t * db, unsigned char * string, _size_t n){
    pm_state_t* curr = db->zerostate, *next;
    int status;
    for (int i = 0; i < n && string[i] != '\0'; i++){
        next = pm_goto_get(curr, string[i]);
        if (next == NULL){
            // there is no transition from this state using the current char
            next = pm_add_state(db, i+1); // create new state
            status = pm_goto_set(curr, string[i], next); // add new edge to transition
            if (status == -1){
                return -1;
            }
        }
        curr = next; // goto next state
    }
    dbllist_append(curr->output, (void*)string); // add the output string to the last node
    return 0;
}

/* Finalizes construction by setting up the failrue _transitions, as
   well as the goto _transitions of the zerostate.
   Returns 0 on success, -1 on failure.*/
int pm_makeFSM(pm_t *db){
    unsigned char a, b;
    dbllist_t queue;
    dbllist_init(&queue);
    pm_state_t* curr_state_suffix;
    pm_state_t* curr_state_prefix;
    pm_state_t* better_prefix;
    for (dbllist_node_t* it = dbllist_head(db->zerostate->_transitions); it != NULL; it = dbllist_next(it)){ // add all depth 1 states to queue
        dbllist_append(&queue, ((pm_labeled_edge_t*)dbllist_data(it))->state);
    }

    while (dbllist_size((&queue)) > 0){
        curr_state_suffix = (pm_state_t*)dbllist_head((&queue))->data;
        dbllist_remove(&queue, dbllist_head((&queue)), DBLLIST_LEAVE_DATA);
        // extract state from queue
        curr_state_prefix = curr_state_suffix->fail; // set initial fail state
        for (dbllist_node_t *suffix = dbllist_head(curr_state_suffix->_transitions); suffix != NULL; suffix = dbllist_next(suffix)){ // run for all transitions of extracted state
            better_prefix = pm_goto_get(curr_state_prefix, ((pm_labeled_edge_t*)(suffix->data))->label); // try to extend the fail for extracted state
            while (better_prefix == NULL && curr_state_prefix->id != 0){ // couldn't improve fail, trying the fail of fail...
                curr_state_prefix = curr_state_prefix->fail;
                better_prefix = pm_goto_get(curr_state_prefix, ((pm_labeled_edge_t*)(suffix->data))->label);
            }
            if (better_prefix == NULL){ // the loop broke because it reached state 0(which has no fail)
                better_prefix = curr_state_prefix;
            }
            ((pm_labeled_edge_t*)suffix->data)->state->fail = better_prefix; // register the found fail
            // updating output linked list:
            if (dbllist_tail(((pm_labeled_edge_t*)suffix->data)->state->output) == NULL){ // current state has no output, but may need to print the output of the fail state
                dbllist_head(((pm_labeled_edge_t*)suffix->data)->state->output) = dbllist_head(better_prefix->output);
                /* making the head of output the head of fail output without increasing the size of list because the output doesn't really belong to that list
                 * this allows me to run over all the outputs that need to be printed at this state without double freeing and without wasting memory
                 */
            }
            else {
                dbllist_tail(((pm_labeled_edge_t*)suffix->data)->state->output)->next = dbllist_head(better_prefix->output);
                /*
                 * same as the if branch only in the case the state doesn't have an empty list we connect the last node to the head of the fail output list
                 */
            }

            printf("Setting f(%d) = %d\n", ((pm_labeled_edge_t*)suffix->data)->state->id, ((pm_labeled_edge_t*)suffix->data)->state->fail->id);
            dbllist_append(&queue, ((pm_labeled_edge_t*)dbllist_data(suffix))->state); // adding the curr state to list to scan its transitions late by BF
        }
    }

    return 0;
}


/* Set a transition arrow from this from_state, via a symbol, to a
   to_state. will be used in the pm_addstring and pm_makeFSM functions.
   Returns 0 on success, -1 on failure.*/
int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state){
    pm_labeled_edge_t *edge = malloc(sizeof(pm_labeled_edge_t));
    if (edge == NULL){
        // error: memory not allocated
        return -1;
    }
    edge->label = symbol;
    edge->state = to_state;
    dbllist_append(from_state->_transitions, edge);
    printf("Allocating state %d\n%d ->%c-> %d\n", from_state->id, from_state->id, symbol, to_state->id);
    return 0;
}

/* Returns the transition state.  If no such state exists, returns NULL.
   will be used in pm_addstring, pm_makeFSM, pm_fsm_search, pm_destroy functions. */
pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol){
    for (dbllist_node_t *it = dbllist_head(state->_transitions); it != NULL; it = dbllist_next(it)) {
        if (((pm_labeled_edge_t *) dbllist_data(it))->label == symbol) {
            return ((pm_labeled_edge_t*)dbllist_data(it))->state;
        }
    }
    return NULL;
}



/* Search for matches in a string of size n in the FSM.
   if there are no matches return empty list */
dbllist_t* pm_fsm_search(pm_state_t *db, unsigned char * string, _size_t n){
    pm_state_t *curr = db;
    pm_state_t *next;
    pm_match_t *match;
    dbllist_t *output = (dbllist_t*)malloc(sizeof(dbllist_t));
    dbllist_init(output);
    for (int i = 0; i < n && string[i] != '\0'; i++){
        next = pm_goto_get(curr, string[i]);
        if (next == NULL && curr->fail != NULL){
            curr = curr->fail;
            i--;
        }
        else {
            curr = next;
            for (dbllist_node_t *it = dbllist_head(curr->output); it != NULL; it = dbllist_next(it)){ // add all state output's to output list as new pm_match structs
                match = (pm_match_t*)malloc(sizeof(pm_match_t));
                match->pattern = (char*)dbllist_data(it);
                match->end_pos = i;
                match->start_pos = i - (int)strlen((char*)dbllist_data(it)) + 1;
                match->fstate = curr;
//                printf("from %d to %d appears: %s\n", match->start_pos, match->end_pos, match->pattern);
                dbllist_append(output, (void*)match);
            }
        }
    }
    return output;
}



/* Destroys the fsm, deallocating memory. */
void pm_destroy(pm_t *db){
    pm_destroy_state(db->zerostate); // calling a recursive destroy state with zero state
}

/* destroys given state and all "child" states with recursion by DF(depth first)*/
void pm_destroy_state(pm_state_t* state){
    pm_state_t *goto_state;
    while (dbllist_size(state->_transitions) > 0){ // there are still transition states to destroy
        // extract edge node from transitions list:
        goto_state = ((pm_labeled_edge_t*)dbllist_data(dbllist_head(state->_transitions)))->state;
        dbllist_remove(state->_transitions, state->_transitions->head, DBLLIST_FREE_DATA);

        pm_destroy_state(goto_state); // destroy state
    }
    dbllist_destroy(state->_transitions, DBLLIST_FREE_DATA);
    free(state->_transitions);
    dbllist_destroy(state->output, DBLLIST_LEAVE_DATA);
    free(state->output);
    state->fail = NULL;
    free(state);
}

/* Adds a state to db in depth stated by parameter "depth"*/
pm_state_t* pm_add_state(pm_t * db, _size_t depth){
    pm_state_t *new = (pm_state_t*) malloc(sizeof(pm_state_t));
    new->_transitions = (dbllist_t*)malloc(sizeof(dbllist_t));
    dbllist_init(new->_transitions);
    new->fail = db->zerostate;
    new->output = (dbllist_t*)malloc(sizeof(dbllist_t));
    dbllist_init(new->output);
    new->depth = depth;
    new->id = db->newstate;
    db->newstate++;
    return new;
}

