#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include "list_struct.h"
#include "list_debug.h"

void        ListCtor                (List *lst, int32_t size);

void        ListDtor                (List *lst);

int32_t     ListInsertBefore        (List *lst, Conv val, int32_t anch);
int32_t     ListInsertAfter         (List *lst, Conv val, int32_t anch);
void        ListErase               (List *lst, int32_t anch);

int32_t     ListPushBack            (List *lst, Conv val);
int32_t     ListPushFront           (List *lst, Conv val);

void        ListPopBack             (List *lst);
void        ListPopFront            (List *lst);

Conv        ListGetValue            (List *lst, int32_t anch);
int32_t     ListGetSize             (List *lst);
int32_t     ListGetCapacity         (List *lst);

int32_t     ListGetHead             (List *lst);
int32_t     ListGetTail             (List *lst);
int32_t     ListGetFree             (List *lst);
     
int32_t     ListGetAnch             (List *lst, int32_t ind);
int32_t     ListGetNext             (List *lst, int32_t anch);
int32_t     ListGetPrev             (List *lst, int32_t anch);

void        ListRealloc             (List *lst, int32_t new_cap, bool linear);
void        ListLinearize           (List *lst);

bool        ListIsEmptyNode         (List *lst, int anch);

#endif  // LIST_H
 
