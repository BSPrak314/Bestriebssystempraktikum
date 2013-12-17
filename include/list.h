#ifndef _list_h_
#define _list_h_

struct list{
		struct list *prev;
		struct list *next;
};

void 		list_initList( struct list * );
struct list 	list_newListStruct( void );
void 		list_clean( struct list * );
unsigned int 	list_getSize(struct list *list);
int 		list_isEmpty( struct list * );
int 		list_hasElements( struct list * );
void 		list_addHead( struct list *, struct list * );
void 		list_addTail( struct list *, struct list * );
void 		list_removeHead( struct list * );
void 		list_removeTail( struct list * );
void 		list_removeElement( struct list *, struct list * );
struct list * 	list_popHead( struct list * );
struct list * 	list_popTail( struct list * );
struct list * 	list_getHead( struct list * );
struct list * 	list_getTail( struct list * );
void 		list_testEmbeddedListStruct( void );

#endif
