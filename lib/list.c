
#include <list.h>
#include <printf.h>

void list_initList( struct list * newList )
{
	newList->prev = newList;
	newList->next = newList;
};

struct list list_newListStruct( void )
{
	struct list newList;
	newList.prev = &newList;
	newList.next = &newList;
	return newList;
};

void list_clean( struct list *list )
{
	list->prev = list;
	list->next = list;
}

unsigned int list_getSize(struct list *list)
{
	unsigned int out = 0;
	struct list * tmp = list->next;
	while( tmp != list){
		tmp = tmp->next;
		out++;
	}
	return out;
}

int list_isEmpty(struct list *list)
{
	return list->next == list && list->prev == list;
}

int list_hasElements(struct list *list)
{
	return list->next != list && list->prev != list;
}

void list_addHead( struct list *list, struct list *element )
{
	list_clean(element);
	if( list_isEmpty(list) ){
		list->prev = element;
		list->next = element;
		return;
	}
	list->next->prev = element;
	element->next = list->next;
	element->prev = element;
	list->next = element;
}

void list_addTail( struct list *list, struct list *element )
{
	list_clean(element);
	if( list_isEmpty(list) ){
	
		list->prev = element;
		list->next = element;
		return;
	}
	
	list->prev->next = element;	
	element->prev = list->prev;
	element->next = element;
	list->prev = element;
}

void list_removeHead(struct list *list)
{
	if(!list->next){
		return;
	}
	if(list->next == list->prev){
		list_clean(list->prev);
		list_clean(list);
		return;
	}

	list->next = list->next->next;
	list->next->prev = list->next;
	return;		
}

void list_removeTail(struct list *list)
{
	if(!list->prev){
		return;
	}
	if(list->next == list->prev){
		list_clean(list->prev);
		list_clean(list);
		return;
	}

	list->prev = list->prev->prev;
	list->prev->next = list->prev;
	return;
}

struct list * list_popHead(struct list *list)
{
	if(!list)
		return 0;
	if( list->next == list){
		return 0;
	}
	struct list * out = list->next;
	if(list->next == list->prev){
		list->next = list;
		list->prev = list;
		list_clean(out);
		return out;
	}
	list->next = list->next->next;
	list->next->prev = list->next;
	list_clean(out);
	return out;		
}

struct list * list_popTail(struct list *list)
{
	if(!list->prev){
		return 0;
	}
	struct list * out = list->prev;
	if(list->next == list->prev){
		list->next = list;
		list->prev = list;
		list_clean(out);
		return out;
	}

	list->prev = list->prev->prev;
	list->prev->next = list->prev;
	list_clean(out);
	return out;
}

struct list * list_getHead(struct list *list)
{
	return list->next;	
}

struct list * list_getTail(struct list *list)
{
	return list->prev;
}

void list_removeElement( struct list *list, struct list *element )
{
	if(list->next == element){
		list->next = element->next;
		list->next->prev = 0;
		list_clean(element);
		return;
	}
	if(list->prev == element){
		list->prev = element->prev;
		list->prev->next = 0;
		list_clean(element);
		return;
	}

	element->next->prev = element->prev;
	element->prev->next = element->next;

	return;
}

/*
void list_testEmbeddedListStruct( void )
{
	struct list emptyBase = list_newListStruct();
	struct list fullBase = list_newListStruct();
	struct list * listEmpty = &emptyBase;
	struct list * listFull = &fullBase;
	
	print("Setting up 2 Lists, which uses the intList struct, stored in array\n\n");
	struct intList elementList[9];
	print("Initializing the arrayElements\n");
	int i = 0;
	for(i = 0;i<9;i++){
		list_clean(&(elementList[i].eList));
		elementList[i].nr = (i+1);
	}
	print("Adding the inherritet Lists to listEmpty \n");
	for(i = 8;i>-1;i--){
		list_addHead(listEmpty, &elementList[i].eList );
	}
	print("Iterationg over listEmpty, performing a cast in each iteration\n\n");
	struct list * tmp = listEmpty->next;
	struct intList tmp2;
	struct intList * tmp3;
	while( tmp != 0){
		tmp2 = *((struct intList *)tmp);
		print("%x, ",tmp2.nr);
		tmp = tmp2.eList.next;
	}
	print("\n>1, 2, 3, 4, 5, 6, 7, 8, 9, < is the correct answer\n");

	print("Iterationg over listEmpty, in the other direction, performing a cast in each iteration\n\n");
	tmp = listEmpty->prev;
	while( tmp != 0){
		tmp2 = *((struct intList *)tmp);
		print("%x, ",tmp2.nr);
		tmp = tmp2.eList.prev;
	}

	print("\n>9, 8, 7, 6, 5, 4, 3, 2, 1, < is the correct answer\n");

	print("Iterationg over listEmpty, using getter function, while clearing the list\n\n");
	tmp = list_getHead(listEmpty);
	while( tmp != 0){
		tmp2 = *((struct intList *)tmp);
		print("%x, ",tmp2.nr);
		list_removeHead(listEmpty);
		tmp = list_getHead(listEmpty);
	}
	print("\n>1, 2, 3, 4, 5, 6, 7, 8, 9, < is the correct answer\nlistEmpty should be empty now : %x\nhas more Elements ? %x\n",list_isEmpty(listEmpty),list_hasElements(listEmpty));

	print("Now filling listFull with addTail\n");
	for(i = 0;i<9;i++){
		list_addTail(listFull, &elementList[i].eList );
	}
	print("\n");
	print("Iterationg over listFull, using getter function,\nnot clearing the list, means starting with listFull, then using nextElement as Argument\nalso alter containing elements, but this will display later\n");
	tmp = list_getTail(listFull);
	while( tmp != 0){
		tmp3 = (struct intList *)tmp;
		print("old %x, ",tmp3->nr);
		tmp3->nr = (tmp3->nr * 10);
		tmp2 = *((struct intList *)tmp);
		print("new %x\n",tmp2.nr);
		tmp = list_getTail(tmp);
	}
	print("\n9, 90\n 8, 80\n7, 70\n6, 60\n5, 50\n4, 40\n3, 30\n2, 20\n1, 10\n< is the correct answer\n");
	print("\n");
	print("Now moving the first 5 Elements from listFull to listEmpty, altering them before moving, then removing from listFull\n");
	tmp = list_getHead(listFull);
	for(i = 0;i<5;i++){
		tmp3 = (struct intList *)tmp;
		tmp3->nr = tmp3->nr / 2; 
		list_removeHead(listFull);
		list_addTail(listEmpty, tmp);
		tmp = list_getHead(listFull);
	}

	tmp = list_getHead(listFull);
	while( tmp != 0){
		tmp3 = (struct intList *)tmp;
		print("%x,",tmp3->nr);
		tmp = list_getHead(tmp);
	}
	print("\n>60, 70, 80, 90, < is the correct answer\n");

	tmp = list_getHead(listEmpty);
	while( tmp != 0){
		tmp3 = (struct intList *)tmp;
		print("%x,",tmp3->nr);
		tmp = list_getHead(tmp);
	}
	print("\n>5, 10, 15, 20, 25, < is the correct answer\n");

	print("Moving them back to listFull, while clearing listEmpty\n");

	while(list_hasElements(listEmpty)){
		tmp = list_getTail(listEmpty);
		tmp3 = (struct intList *)tmp;
		tmp3->nr = tmp3->nr * 4;
		list_removeElement(listEmpty, tmp);
		list_addTail(listFull,tmp);
	}

	tmp = list_getHead(listFull);
	while( tmp != 0){
		tmp3 = (struct intList *)tmp;
		print("%x,",tmp3->nr);
		tmp = list_getHead(tmp);
	}
	print("\n>60, 70, 80, 90, 100, 80, 60, 40, 20, < is the correct answer\n");

	print("clearing listFull\n");	
	i = 0;
	while( ! list_isEmpty(listFull) ){
		list_removeHead(listFull);
		i++;
	}

	print("\nshould be 9 iterations : %x\nDONE\nEND OF TESTING embedded list struct\n",i);
}
*/