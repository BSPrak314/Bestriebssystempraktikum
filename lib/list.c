
#include <printf.h>
#include <list.h>

struct list list_newListStruct( void )
{
		struct list newList;
		newList.prev = 0;
		newList.next = 0;
		newList.data = 0;		
		return newList;
};

void list_init( struct list *list )
{
		list->prev = 0;
		list->next = 0;
		list->data = 0;
}

int list_isEmpty(struct list *list)
{
		return list->next == 0;
}

int list_hasElements(struct list *list)
{
		return list->next != 0;
}

void list_addHead( struct list *list, struct list *element )
{
		if( list_isEmpty(list) ){
				list->prev = element;
				list->next = element;
				list_init(element);
				return;
		}
		list->next->prev = element;
		element->next = list->next;
		element->prev = 0;
		list->next = element;
}

void list_addTail( struct list *list, struct list *element )
{
		if( list_isEmpty(list) ){
				list->prev = element;
				list->next = element;
				list_init(element);
				return;
		}
		list->prev->next = element;
		element->prev = list->prev;
		element->next = 0;
		list->prev = element;
}

void * list_removeHead(struct list *list)
{
		void *out = 0;
		if(!list->next){
				if(list->prev)
						printf("Error in list struct: list had no head, but a tail\n");
				return out;
		}
		out = list->next->data;
		if(list->next == list->prev){
				list_init(list);
				return out;
		}

		list->next = list->next->next;
		list->next->prev = 0;
		return out;		
}

void * list_removeTail(struct list *list)
{
		void *out = 0;
		if(!list->prev){
				if(list->next)
						printf("Error in list struct: list had no tail, but a head\n");
				return out;
		}
		out = list->prev->data;
		if(list->next == list->prev){
				list_init(list);
				return out;
		}

		list->prev = list->prev->prev;
		list->prev->next = 0;
		return out;
}

void list_removeElement( struct list *list, struct list *element )
{
		struct list *tmp = 0;
		if( !element->prev && !element->next ){
				list_init(list);
		}
		if( element->next ){
				tmp = element->next;
				tmp->prev = element->prev;
		}
		if( element->prev )
				element->prev->next = tmp;
		return;
}

void list_testListStruct( void )
{
        char numbers[] = "123456789";
        struct list * elements[9];
        int i = 0;
        printf("creating a 9 list structs pointers as elements\n");
        for(i = 0;i<9;i++){
	        	list_init(elements[i]);
	        	elements[i]->data = &numbers[i];
	        	if(!list_isEmpty(elements[i]) )
	        			printf("Error: new created list is not empty\n");
	        	if(list_hasElements(elements[i]))
	        			printf("Error: new created list has Elements\n");
        }
		printf("creating a new list structs as base\n");
		struct list ancorList = list_newListStruct();
		printf("testing list_addHead and list_addTail\n");
		for(i = 0;i<8;i++){
				if( i < 4 )
						list_addHead(&ancorList,elements[i]);
				if( i > 3 )
						list_addTail(&ancorList,elements[i]);
		}
		i = 1;
		struct list *listElement = ancorList.next;
		printf("now testing iterating over the list, starting at the head\n");
		while( listElement != 0){
				printf("%c",*(char *)(listElement->data));
				listElement = listElement->next;
		}
		printf("\nshould look like: 43215678\n");
		printf("now testing list_removeHead and list_removeTail\n");
		list_removeHead(&ancorList);
		list_removeTail(&ancorList);

		listElement = ancorList.next;
		while( listElement != 0){
				printf("%c",*(char *)(listElement->data));
				listElement = listElement->next;
		}
		printf("\nshould look like: 321567\n");
		
		printf("now testing list_removeElement by removing the 3rd Element\n");
		
		listElement = ancorList.next->next->next;
		list_removeElement(&ancorList, listElement);

		printf("\nshould look like: 32567\n");
		printf("now more removing\n");
		list_removeHead(&ancorList);
		list_removeTail(&ancorList);
		listElement = ancorList.next->next->next;
		list_removeElement(&ancorList, listElement);

		listElement = ancorList.next;
		while( listElement != 0){
				printf("%c",*(char *)(listElement->data));
				listElement = listElement->next;
		}
		printf("\nshould look like: 216\n");

		list_addHead(&ancorList, elements[8]);
		list_addHead(&ancorList, elements[7]);
		
		listElement = ancorList.next;
		while( listElement != 0){
				printf("%c",*(char *)(listElement->data));
				listElement = listElement->next;
		}
		printf("\nshould look like: 92167\n");
		i = 0;
		listElement = ancorList.next;
		while( list_hasElements(&ancorList) ){
				list_removeElement(&ancorList, listElement);
				listElement = ancorList.next;
				i++;
		}
		printf("this should been 5 iterations, i = %x, list_isEmpty should be 1 now = %x\n",i,list_isEmpty(&ancorList));

		printf("END OF TESTING list struct\n");
}

void list_testEmbeddedListStruct( void )
{
		printf("END OF TESTING embedded list struct\n");
}