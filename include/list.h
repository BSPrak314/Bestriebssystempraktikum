#ifndef _list_h_
#define _list_h_

struct list{
		struct list *prev;
		struct list *next;
		void *data;
};

struct intlist{
		struct list embeddedList;
		int nr;
};

struct list list_newListStruct( void );
void list_init( struct list *list );
int list_isEmpty(struct list *list);
int list_hasElements(struct list *list);
void list_addHead( struct list *list, struct list *element );
void list_addTail( struct list *list, struct list *element );
void * list_removeHead(struct list *list);
void * list_removeTail(struct list *list);
void list_removeElement( struct list *list, struct list *element );
void list_testListStruct( void );
void list_testEmbeddedListStruct( void );

#endif
