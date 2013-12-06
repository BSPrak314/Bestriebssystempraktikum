
struct list{
	struct list *prev;
	struct list *next;
};

struct list initList( void )
{
	struct list newList;
	newList.prev = 0;
	newList.next = 0;
	return newList;
};

void list_init( struct list *list )
{
	list->prev = 0;
	list->next = 0;
}

void removeElement( struct list *list, struct list *element )
{
	if( !element->prev && !element->next ){
		list_init(list);
		return
	}
	list *tmp = 0;
	if( element->next ){
		tmp = element->next;
		tmp->prev = element->prev;
	}
	if( element->prev )
		element->prev->next = tmp;
	return;
}

void addHead( struct list *list, struct list *element )
{
	if( !list->prev ){
		list->prev = element;
		list->next = element;
		list_init(element);
		return;
	}
	list->prev->prev = element;
	element->next = list->prev;
	list->prev = element;
}

void addTail( struct list *list, struct list *element )
{
	if( !list->prev ){
		list->prev = element;
		list->next = element;
		list_init(element);
		return;
	}
	list->next->next = element;
	element->prev = list->next;
	list->next = element;
}