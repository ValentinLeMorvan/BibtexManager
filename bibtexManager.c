#include <bibtexManager.h>

/* Library parseBibtexFile(char* path)
 * Function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This function creates a library with the bibtex file the user specified by the user*/

Library parseBibtexFile(char* path)
{
	FILE* fp;
	char actualCaracter;
	char* line = (char*) malloc(2000*sizeof(char));
	Library library=NULL;
	Entry tempEntry;
	tempEntry.authorList=NULL;
	tempEntry.listEntryFields=NULL;
	
	initializeEntry(&tempEntry);

	fp = fopen(path,"r");
	if(fp==NULL)
	{
		printf("FAILED TO OPEN THE FILE\n");
	}
	else /*if the file is successfully opened*/
	{
		
		actualCaracter = fgetc(fp);
		while(actualCaracter!=EOF)
		{
			tempEntry.authorList=NULL; /* we have to initialize them again if we want to create a new list*/
			tempEntry.listEntryFields=NULL;
			
			if(actualCaracter == '@')
			{
				fscanf(fp,"%s",line);
				strcpy(tempEntry.type,strtok(line,"{")); /* we can directly copy what is BEFORE the '{' with strtok. And it is the type of the file*/
				strcpy(tempEntry.key,strtok(NULL,",")); /* here we copy what is before the ',' and after the '{', this is the key of the entry*/
				actualCaracter=fgetc(fp); /*to go after the comma and go to the CR*/
				fscanf(fp,"%s",line);
				do
				{	
					if (strcmp("author",line)==0)
					{
						tempEntry.authorList = buildAuthorList(tempEntry.authorList,line,fp);
					}
					else
					{
						tempEntry.listEntryFields = buildEntryFieldList(tempEntry.listEntryFields,line,fp);
						fseek(fp,-1,SEEK_CUR); /*we go back to the previous character, because we are too far and we have to read the line, into the function buildEntryFieldList*/
					}
					
					fscanf(fp,"%s",line);
					
				}while(strcmp(line,"}")!=0); /* we stop is there is only '}' on the line*/
				
				library = insertEntry(&tempEntry,library);
			
			}
			actualCaracter = fgetc(fp); /* we read the character until we reach a '@' (begin of an entry) or the end of the file*/
			
			
		}
		printf("\nThe file was parsed\n");

		fclose(fp);
	}

	freeEntry(&tempEntry);
	
	return library;
}

/* ListEntryFields buildEntryFieldList(ListEntryFields listEntryFields,char* line,FILE* fp)
 * Function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This function completes an EntryFieldList by parsing a line of the line.*/

ListEntryFields buildEntryFieldList(ListEntryFields listEntryFields,char* line,FILE* fp)
{
	int i=0; /*an iteration variable*/
	int verif=0;
	EntryField tempEntryField;
	char actualCaracter;
    char* buffer = (char *) malloc(2000*sizeof(char));
    tempEntryField.fieldname = (char *) malloc(2000*sizeof(char));
    tempEntryField.value = (char *) malloc(2000*sizeof(char));
	
    
    strcpy(tempEntryField.fieldname,strtok(line,"="));

    do
	{
		actualCaracter=fgetc(fp);
    }while(actualCaracter!='{');
    
    do
    {
		actualCaracter=fgetc(fp);
		buffer[i]=actualCaracter; /*We copy the contents of the entryfield caracter by caracter*/
		if(actualCaracter=='\n')
		{
			actualCaracter=fgetc(fp);
			if(actualCaracter=='\t' || actualCaracter=='}') /*we check that there is a '\t' after the '\n', meaning that it's the end of the entryfild, of simply the end of the entry*/
			{
				verif=1;
			}
			
			if(verif!=1)
			{
				fseek(fp,-1,SEEK_CUR); /* everytime we check the character after, we have to go back in the file, because we went too far*/
			}
		}
		
		
		i++;
	}while(verif != 1); /*until the caracter is the CR*/
	
	buffer[i-3]='\0'; /*because the 3 last characters of buffer are } , and \n*/
	

	strcpy(tempEntryField.value,buffer);

    listEntryFields = insertTailListEntrFi(&tempEntryField,listEntryFields);
	
	free(tempEntryField.fieldname);
    free(tempEntryField.value);
	free(buffer);
	
	return listEntryFields;
}

/*ListEntryFields insertTailListEntrFi(EntryField *tempEntryField,ListEntryFields listEntryFields)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions inserts the value and the name of a given entryField into a list of entry fields*/

ListEntryFields insertTailListEntrFi(EntryField *tempEntryField,ListEntryFields listEntryFields)
{
	EntryFieldElem* newel = (EntryFieldElem *) malloc(sizeof(EntryFieldElem));
	EntryFieldElem* p;
	newel->entryField.fieldname = (char *) malloc((strlen(tempEntryField->fieldname)+1)*sizeof(char));
    newel->entryField.value = (char *) malloc((strlen(tempEntryField->value)+1)*sizeof(char));
    strcpy(newel->entryField.fieldname,tempEntryField->fieldname);
    strcpy(newel->entryField.value,tempEntryField->value);
	newel->next = NULL;
	newel->prev = NULL;
	if(listEntryFields==NULL)
	{
		listEntryFields=newel;
	}
	else
	{	
		p=listEntryFields;
		while(p->next != NULL)
		{
			p=p->next;
		}
		p->next = newel;
		newel->prev = p;
	}
	return listEntryFields;
}

/*void printEntryFieldList(ListEntryFields listEntryFields)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions prints a whole given list of entryFields*/

void printEntryFieldList(ListEntryFields listEntryFields)
{
	ListEntryFields p;	
	
	if(listEntryFields == NULL)
	{
		printf("Empty EntryFieldList\n");
	}
	else
	{
		p=listEntryFields;
		while(p!=NULL)
		{
			printf("%s = %s\n",p->entryField.fieldname,p->entryField.value);
			p=p->next;
		}
	}
}

/*AuthorList buildAuthorList(AuthorList authorList,char* line,FILE* fp)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions parses a line of a file containing a list of authors and builds a usable list of authors*/

AuthorList buildAuthorList(AuthorList authorList,char* line,FILE* fp)
{
	int i=0;
	Author tempAuthor;
	char actualCaracter;
	int verif;
	char* buffer = (char *) malloc(2000*sizeof(char));
    tempAuthor.name=(char *) malloc(2000*sizeof(char));
    tempAuthor.firstname=(char *) malloc(2000*sizeof(char));
    
    do
	{
		actualCaracter=fgetc(fp);
    }while(actualCaracter!='{'); /*moving to the start of the enumeration of the authors*/

	do
	{
		i=0;
		do
		{
			actualCaracter = fgetc(fp);
			buffer[i] = actualCaracter;
			i++;
			
		}while(actualCaracter!=',');
		
		buffer[i-1] = '\0';
		strcpy(tempAuthor.name,buffer);
		
		i = 0;
		actualCaracter = fgetc(fp); /*to skip the next space*/
		verif = 0;
		do
		{
			actualCaracter = fgetc(fp);
			buffer[i] = actualCaracter;
			i++;
			if(actualCaracter == ' ')
			{
				actualCaracter = fgetc(fp);
				if(actualCaracter == 'a') 
				{
					verif = 1; /*if the character after the space is a minor a, it's the end of the firstname of the author*/
				}
				fseek(fp,-1,SEEK_CUR);
			}
			
		}while(verif == 0 && actualCaracter!=','); /*if it's the end of the entry, then actualCaracter will meet a coma*/
		/*if this is a ',', we have to go back of 1 caracter, because it's the end of the line*/
		if(actualCaracter==',')
		{
			buffer[i-2]='\0';
			
		}
		else
		{
			buffer[i-1] = '\0';
		}
		strcpy(tempAuthor.firstname,buffer);

		authorList = insertTailAuthorList(&tempAuthor,authorList);
		
		do
		{
			actualCaracter = fgetc(fp);
		}while(actualCaracter!=' ' && actualCaracter!='\n');
		
	}while(actualCaracter!='\n');

	free(tempAuthor.name);
    free(tempAuthor.firstname);
	free(buffer);
	
	return authorList;
}

/*AuthorList insertTailAuthorList(Author* tempAuthor,AuthorList authorList)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions inserts a given author into a list of authors*/

AuthorList insertTailAuthorList(Author* tempAuthor,AuthorList authorList)
{
	AuthorListElem *newel = (AuthorListElem *) malloc(sizeof(AuthorListElem));
	AuthorListElem* p;
	newel->author.firstname = (char *) malloc((strlen(tempAuthor->firstname)+1)*sizeof(char));
    newel->author.name = (char *) malloc((strlen(tempAuthor->name)+1)*sizeof(char));
    strcpy(newel->author.firstname,tempAuthor->firstname);
    strcpy(newel->author.name,tempAuthor->name);
	newel->next = NULL;
	newel->prev = NULL;

	if(authorList==NULL)
	{
		authorList=newel;
	}
	else
	{	

		p = authorList;
		while(p->next != NULL)
		{
			p=p->next;
		}
		p->next = newel;
		newel->prev = p;
	}

	return authorList;
}
	
/*Library insertTailLib(Library library,Entry *tempEntry)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions inserts a given entry at the tail of a library*/

Library insertTailLib(Library library,Entry *tempEntry)
{
	LibraryElem* newel = (LibraryElem *) malloc(sizeof(LibraryElem));
	Library p = NULL;
	newel->entry.key = (char *) malloc((strlen(tempEntry->key)+1)*sizeof(char));
	newel->entry.type = (char *) malloc((strlen(tempEntry->type)+1)*sizeof(char));
	strcpy(newel->entry.key,tempEntry->key);
	strcpy(newel->entry.type,tempEntry->type);
	newel->entry.authorList = tempEntry->authorList;
	newel->entry.listEntryFields = tempEntry->listEntryFields;
	newel->next = NULL;
	newel->prev = NULL;
	if(library==NULL)
	{
		library=newel;
	}
	else
	{	
		p = library;
		while(p->next != NULL)
		{
			p=p->next;
		}
		p->next = newel;
		newel->prev = p;
	}
	
	return library;
}

/*void printAuthorList(AuthorList authorList)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions prints a given author list, with the name and the firstname of each author*/

void printAuthorList(AuthorList authorList)
{
	AuthorList p;	
	
	if(authorList == NULL)
	{
		printf("Empty AuthorList\n");
	}
	else
	{
		printf("Authors : ");
		p=authorList;
		
		while(p!=NULL)
		{
			printf("%s %s, ",p->author.firstname,p->author.name);
			p=p->next;
		}
		printf("\n");
	}

}

/*void printEntry(Entry *tempEntry)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions prints the key, the entryfieldlist and the author list of a given entry*/

void printEntry(Entry *tempEntry)
{
	printf("Type =%s\nKey = %s\n",tempEntry->type,tempEntry->key);
	printEntryFieldList(tempEntry->listEntryFields);
	printAuthorList(tempEntry->authorList);
	printf("\n\n");
}

/*void printLibrary(Library library)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions prints avery entry of a given library*/
 
void printLibrary(Library library)
{
	Library p;	
	int i = 1;
	
	if(library == NULL)
	{
		printf("Empty Library\n");
	}
	else
	{
		p=library;
		
		while(p!=NULL)
		{
			printf("   >Entry num %d\n",i);
			printEntry(&(p->entry));
			p=p->next;
			i++;
		}
	}
}

void initializeEntry(Entry* tempEntry)
{
	tempEntry->type = (char *) malloc(100*sizeof(char));
	tempEntry->key = (char *) malloc(200*sizeof(char));
}
void freeEntry(Entry* tempEntry)
{
	free(tempEntry->type);
	free(tempEntry->key);
}

/*Library insertEntry(Entry* newEntry,Library library)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions inserts a given entry into a library, verifying that the entry doesn't exist yet*/

Library insertEntry(Entry* newEntry,Library library)
{
	int isNewEntryDifferent; /*1 = Yes, it's different, we can include it /// 0 = No, it's not different, we can't include 2 similar entries)*/
	
	isNewEntryDifferent = compareEntries(newEntry,library);
	
	if(isNewEntryDifferent == 1)  /*If there is no similar entry, we can ask for the entryfields of the new entry to build it*/
	{
		library = insertTailLib(library,newEntry);
		
	}
	else if (isNewEntryDifferent == 0)
	{
		printf("This entry already exists. (Key : ");
		printf("%s)\n",newEntry->key);
		
	}
	else
	{
		printf("Failure inserting the entry\n");
	}
	
	return library;
}

/*int compareEntries(Entry* newEntry,Library library)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions compare a given entry to every entries into a given library and returns 0 if the entry already exists into the lib (elsa returns 1)*/

int compareEntries(Entry* newEntry,Library library)
{
	
	/*entries are considered as duplicates if and only if they have the same title
and the same list of authors*/
	Library p=NULL;
	int areTitleDifferent; /*if titles are differents, then areTitleDifferent = 1, else areTitleDifferent = 0*/
	int areAuthorsDifferent;
	
	if(library == NULL)
	{
		return 1;
	}
	else
	{
		p = library;
		while(p!=NULL)
		{
			areTitleDifferent = isTitleDifferent(p->entry.listEntryFields,newEntry->listEntryFields);
			areAuthorsDifferent = isAuthorListDifferent(p->entry.authorList,newEntry->authorList);
			if(areAuthorsDifferent==0 && areTitleDifferent==0)
			{
				return 0; /*when ONE of the Entry is the same as the newEntry, end the loop*/
			}
			p=p->next;
			
		}
		
		return 1;
	}
}
/*int isTitleDifferent(ListEntryFields listEntryFields,ListEntryFields newListEntryFields)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This function compares the title into two different ListEntryFields. Returns 1 if they are different, 0 if same*/
int isTitleDifferent(ListEntryFields listEntryFields,ListEntryFields newListEntryFields)
{
	ListEntryFields p=listEntryFields;
	ListEntryFields np=newListEntryFields;
	int foundTitle = 0;
	
	while(foundTitle!=1)
	{
		if(strcmp(p->entryField.fieldname,"title")==0)
		{
			foundTitle = 1;
		}
		if(foundTitle !=1)
		{
			p=p->next;
		}
	}
	
	foundTitle = 0;
	
	while(foundTitle!=1)
	{
		if(strcmp(np->entryField.fieldname,"title")==0)
		{
			foundTitle = 1;
		}
		if(foundTitle !=1)
		{
			np=np->next;
		}
	}
	
	
	if(strcmp(np->entryField.value,p->entryField.value)==0)
	{
		return 0; /*the titles are the same*/
	}
	else
	{
		return 1;
	}
}

/*int isAuthorListDifferent(AuthorList authorList,AuthorList newAuthorList)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions compares two authorlists*/

int isAuthorListDifferent(AuthorList authorList,AuthorList newAuthorList)
{
	AuthorList p = authorList;
	AuthorList np = newAuthorList;
	int i=0; /*an iteration variable*/
	int j=0;
	
	/* 2 authorlists are the same if and only if : they have the same number of authors
	 * they have the same authors IN THE SAME ORDER*/
	
	while(p!=NULL)
	{
		i++;
		p=p->next;
	}
	while(np!=NULL)
	{
		j++;
		np = np->next;
	}
	if(j!=i)
	{
		return 1; /*the author lists are different because they don't have the same number of authors*/
	}
	else
	{
		p = authorList;
		np = newAuthorList;

			
		while(p!=NULL && np!=NULL)
		{
			if(strcmp(p->author.firstname,np->author.firstname)!=0 || strcmp(p->author.name,np->author.name)!=0)
			{
				return 1; /*If the found ONE author that is different from another, the authorLists are different*/
			}
			p=p->next;
			np=np->next;
			
		}
		return 0; /*if we went through the two authorLists without founding different authors, then the authorLists are the sames*/
	}
}

/*
 * Library removeEntry(Library library,char* entryKey)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions removes the entry corresponding to a given entrykey*/

Library removeEntry(Library library,char* entryKey)
{
	Library p = NULL;
	
	if(library == NULL)
	{
		printf("This library is empty\n");
	}
	else
	{
		p = findEntryToRemove(library,entryKey);
		if(p == NULL)
		{
			printf("This entry isn't into the library. It was not found.\n");
		}
		else
		{
			if(p->next!=NULL)
			{
				p->next->prev = p->prev;
			}
			if(p->prev!=NULL)
			{
				p->prev->next = p->next;
			}
			if(p->prev == NULL)
			{
				library = p->next; 
			}
			/*if p->prev = NULL, then it mean that p is equal to library.
			 * we have to change the pointeur library into p->next
			 * or we will free library and it will be equal to 0*/
			free(p);
			p=NULL;

			printf("Entry successfuly removed !!\n");
		}
		
	}

	return library;
	
}

/*
 *Library findEntryToRemove(Library library,char* entryKey)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions returns an entry corresponding to an entry key*/

Library findEntryToRemove(Library library,char* entryKey)
{
	Library p=library;
	
	printf("entryKey : %s\n",entryKey);
	
	while(p!=NULL)
	{
		if(strcmp(entryKey,p->entry.key)==0)
		{
			return p; /* if we found the entry that match with the key, we return the pointer on this library element.*/
		}
		p = p->next;
	}
	return NULL; /*if we go through the whole library without founding the entry, we return NULL*/

}

/*
 * Valentin LEMORVAN
-SearchEntry search a given field in a list of Entryfields and return its value
-l : Given list of EntryFields
-s : Name of the field to search
-returns the value of the field or NULL.
*/

char* SearchField (ListEntryFields l, char* s)
{
	EntryFieldElem* e=NULL;
	char* fieldValue=NULL;
	e=l;

	while(strcmp(e->entryField.fieldname,s)!=0 && e->next != NULL)
	{
		e = e->next;
	}
	if(strcmp(e->entryField.fieldname,s)==0)
	{
		fieldValue=(char*) malloc((strlen(e->entryField.value)+1)*sizeof(char));
		strcpy(fieldValue,e->entryField.value);
	}
	else
	{	
		fieldValue = (char *) malloc(2*sizeof(char));
		strcpy(fieldValue,"0");
	}
	return fieldValue;
}

/*
 * Valentin LEMORVAN
-InsertTail inserts a given AuthorPubElem in a given AuthorsPublications list.
-l: AuthorsPublications in which insert the element
-e: Element to insert in l
-The function returns a AuthorsPublications corresponding to l with e in its tail.
*/


AuthorsPublications InsertTailAP(AuthorsPublications l, AuthorPubElem* e)
{
	AuthorsPublications newel = (AuthorPubElem*) malloc(sizeof(AuthorPubElem));
	AuthorsPublications p=NULL;
	newel->author.firstname=(char *) malloc((strlen(e->author.firstname)+1)*sizeof(char));
	newel->author.name=(char *) malloc((strlen(e->author.name)+1)*sizeof(char));
	strcpy(newel->author.firstname,e->author.firstname);
	strcpy(newel->author.name,e->author.name);
	newel->publications=e->publications;
	newel->next=NULL;
	newel->prev=NULL;
	if(l==NULL)
	{
		l=newel;	
	}
	else
	{
		p=l;
		while(p->next!=NULL)
		{
			p=p->next;
		}
		p->next=newel;
		newel->prev=p;

	}

	return l;
}

/*
 * Valentin LEMORVAN
-Quicksort sorts a given AuthorsPublications using the Quicksort algorithm
-L: AuthorsPublications to sort
-Returns the AuthorsPublications L sorted by authors.
*/
AuthorsPublications QuickSortAP(AuthorsPublications L)
{
	AuthorsPublications less=NULL;
	AuthorsPublications greater=NULL;
	AuthorPubElem* p=L;
	AuthorsPublications pivot=NULL;
	AuthorsPublications l=NULL;

	if(p==NULL)
	{
		l=NULL;
	}
	else
	{
		pivot=InsertTailAP(pivot,L);
		p=p->next;
		while(p!=NULL)
		{
			if(strcmp(p->author.name,pivot->author.name)<=0)
			{
				less=InsertTailAP(less,p);
			}
			else
			{
				greater=InsertTailAP(greater,p);

			}
			p=p->next;
		}
	l=Concatenate(QuickSortAP(less),pivot,QuickSortAP(greater));
	}

	return l;
}

/*
 * Valentin LEMORVAN
-Concatenate transforms 3 AuthorsPublications into a single AuthorsPublications
-l1:AuthorsPublication to concatenate in head of the final AuthorsPublication
-l2:AuthorsPublication to concatenate in the middle of the final AuthorsPublication
-l3:AuthorsPublication to concatenate in the tail of the final AuthorsPublication
-This function returns a single list containing the 3 given lists
*/
AuthorsPublications Concatenate(AuthorsPublications l1, AuthorsPublications l2, AuthorsPublications l3)
{
	AuthorsPublications p=l1;
	AuthorsPublications l=NULL;
	while(p!=NULL)
	{
		l=InsertTailAP(l,p);
		p=p->next;
	}

	p=l2;
	while(p!=NULL)
	{
		l=InsertTailAP(l,p);
		p=p->next;
	}

	p=l3;
	while(p!=NULL)
	{
		l=InsertTailAP(l,p);
		p=p->next;
	}

	return l;
}

/* 
 * Valentin LEMORVAN
-isAuthor verifies if an author is already present in an AuthorsPublications
-a: Author to search
-l: List in which the function search a
-return 0 if the author is not in the list and 1 if the author is in the list
*/

int isAuthor(AuthorsPublications l, Author* a) /*infinite loop ??*/
{
	int v=0;
	AuthorsPublications p=l;
	while(p!=NULL && v==0)
	{
		if(strcmp(p->author.name,a->name)==0 && strcmp(p->author.firstname,a->firstname)==0)
		{
			v=1;	
		}
		p=p->next;
	}
	return v;
}


/*
 * Valentin LEMORVAN
-SortLibraryAuthorDate transforms a given library into a AuthorsPublications sorted by authors last name and by date.
-l: library to sort
-returns a sorted library
*/
AuthorsPublications SortLibraryAuthorDate(Library l)
{
	AuthorsPublications AP=NULL;
	Library p=l;
	AuthorList a=NULL;
	AuthorsPublications ref=NULL;

	while(p!=NULL)
	{
		a=p->entry.authorList;
		while(a!=NULL)
		{
			if(isAuthor(AP,&(a->author))==0)
			{
				ref=getAuthorReferences(l,&(a->author));
				AP=InsertTailAP(AP,ref);
			}
			a=a->next;
		}
		p=p->next;
	}
	free(a);
	a=NULL;
	free(ref);
	ref=NULL;
	if(AP!=NULL)
	{
		AP=QuickSortAP(AP);
		printf("The AuthorsPublication was successfuly created\n");
	}
	
	return AP;
}

/*
 * Valentin LEMORVAN
-InsertElemAP inserts a given entry in the list of publications of a given AuthorsPubElem, inserting it sorted by year
-l: pointer on the AuthorsPubElem
-e: Entry to insert in the Library of l
-returns a pointer on the AuthorPubElem.
*/
AuthorsPublications InsertElemAP(AuthorPubElem* l, Entry* e)
{
	long y,ny;
	Library p;
	LibraryElem* newel=(LibraryElem*) malloc(sizeof(LibraryElem));
	newel->entry.key = (char *) malloc((strlen(e->key)+1)*sizeof(char));
	newel->entry.type = (char *) malloc((strlen(e->type)+1)*sizeof(char));
	
	strcpy(newel->entry.key,e->key);
	strcpy(newel->entry.type,e->type);
	newel->entry.authorList=e->authorList;
	newel->entry.listEntryFields=e->listEntryFields;
	
	newel->next=NULL;
	newel->prev=NULL;
	ny=atol(SearchField(e->listEntryFields,"year"));
	
	if(l!=NULL)
	{
		if(l->publications==NULL)
		{
			l->publications=newel;
		}
		else
		{
			p=l->publications;
			y=atol(SearchField(p->entry.listEntryFields,"year"));
			while(p->next!=NULL && ny<y)
			{
				p=p->next;
				y=atol(SearchField(p->entry.listEntryFields,"year"));
			}
			if(p->next==NULL && ny<y)
			{
				p->next=newel;
				newel->prev=p;
			}
			else
			{
				newel->prev=p->prev;
				p->prev=newel;
				if(newel->prev!=NULL)
				{
					newel->prev->next=newel;
				}
				newel->next=p;
			}
		}
	}
	return l;
}

/*
 * Valentin LEMORVAN
-getAuthorReferences gets the list of publications of a given author from a given library
-L: library in which looking for the publications
-a: author to seek in the library
-the function returns an AuthorsPublications of one single element, with its publications sorted by year.
*/
AuthorsPublications getAuthorReferences (Library L,Author* a)
{
	AuthorPubElem* AP=(AuthorPubElem*) malloc(sizeof(AuthorPubElem));
	Library tempL=L;
	AuthorList tempA=NULL;
	AP->author.firstname = (char *) malloc((strlen(a->firstname)+1)*sizeof(char));
	AP->author.name = (char *) malloc((strlen(a->name)+1)*sizeof(char));
	strcpy(AP->author.firstname,a->firstname);
	strcpy(AP->author.name,a->name);
	AP->publications=NULL;
	
	while(tempL!=NULL)
	{
		tempA=tempL->entry.authorList;
		while(tempA!=NULL)
		{
			if(strcmp(tempA->author.name,a->name)==0 && strcmp(tempA->author.firstname,a->firstname)==0)
			{
				AP=InsertElemAP(AP,&(tempL->entry));
			}
			tempA=tempA->next;
		}
		tempL=tempL->next;
	}
	return AP;
}

/*void printAuthorsPublications(AuthorsPublications l)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions prints the whole list of authors publications (Entries sorted by dates for every Author sorted by names)*/

void printAuthorsPublications(AuthorsPublications l)
{
	AuthorsPublications p = l;
	Library publicationsOfOneAuthor=NULL;
	
	if(p==NULL)
	{
		printf("Empty list.\n");
	}
	else
	{
		while(p!=NULL)
		{
			printf("\tAuthor : %s %s\n\n",p->author.firstname,p->author.name);
			publicationsOfOneAuthor=p->publications;
			while(publicationsOfOneAuthor != NULL)
			{
				printEntry(&(publicationsOfOneAuthor->entry));
				publicationsOfOneAuthor=publicationsOfOneAuthor->next;
			}
			p=p->next;
		}
	}
	
}

/*
 * Written by Valentin LE MORVAN
-InsertTailDP inserts a given DatePubElem in a given DatePublications list.
-l: DatePublications in which insert the element
-e: pointer on the element to insert in l
-The function returns a DatePublications corresponding to l with e in its tail.
*/


DatePublications InsertTailDP(DatePublications l, DatePubElem* e)
{
	DatePublications newel = (DatePubElem*) malloc(sizeof(DatePubElem));
	DatePublications DP=l;
	DatePublications p=NULL;
	newel->year=e->year;
	newel->publications=e->publications;
	newel->next=NULL;
	newel->prev=NULL;
	if(DP==NULL)
	{
		DP=newel;
		
	}
	else
	{
		p=DP;
		while(p->next!=NULL)
		{
			p=p->next;
		}
		p->next=newel;
		newel->prev=p;

	}
	
	return DP;
}


/*
 * Written by Valentin LE MORVAN
-ConcatenateDP transforms 3 DatePublications into a single DatePublications
-l1:DatePublication to concatenate in head of the final DatePublication
-l2:DatePublication to concatenate in the middle of the final DatePublication
-l3:DatePublication to concatenate in the tail of the final DatePublication
-This function returns a single list containing the 3 given lists
*/
DatePublications ConcatenateDP(DatePublications l1, DatePublications l2, DatePublications l3)
{
	DatePublications p=l1;
	DatePublications l=(DatePublications) malloc(sizeof(DatePubElem));
	l=NULL;
	while(p!=NULL)
	{
		l=InsertTailDP(l,p);
		p=p->next;
	}

	p=l2;
	while(p!=NULL)
	{
		l=InsertTailDP(l,p);
		p=p->next;
	}

	p=l3;
	while(p!=NULL)
	{
		l=InsertTailDP(l,p);
		p=p->next;
	}

	return l;
}


/*
 * Written by Valentin LE MORVAN
-QuicksortDP sorts a given DatePublications using the Quicksort algorithm
-L: DatePublications to sort
-Returns the DatePublications L sorted by year.
*/
DatePublications QuickSortDP(DatePublications L)
{
	DatePublications less=NULL;
	DatePublications greater=NULL;
	DatePublications p=L;
	DatePublications pivot=NULL;
	DatePublications l=NULL;
	
	if(p==NULL)
	{
		l=NULL;
	}
	else
	{
		pivot=InsertTailDP(pivot,p);
		p=p->next;
		while(p!=NULL)
		{
			if(pivot->year <= p->year)
			{
				less=InsertTailDP(less,p);
			}
			else
			{
				greater=InsertTailDP(greater,p);
				
			}
			p=p->next;
		}
	l=ConcatenateDP(QuickSortDP(less),pivot,QuickSortDP(greater));
	}

	return l;
}


/*Written by Valentin LE MORVAN
-isYear verifies if a year is already present in a DatePublications
-y: year to search
-l: List in which the function search y
-return 0 if the year is not in the list and 1 if the date is already in the list
*/

int isYear(DatePublications l, long y)
{
	int v=0;
	DatePublications p=l;
	while(p!=NULL && v==0)
	{
		if(p->year == y)
		{
			v=1;
		}
		p=p->next;
	}
	return v;
}


/*
 * Written by Valentin LE MORVAN
-SortLibraryDateAuthor transforms a given library into a DatePublications sorted by year and by author.
-l: library to sort
-returns a sorted DatePublications
*/
DatePublications SortLibraryDateAuthor (Library l)
{
	DatePublications DP=NULL;
	Library p=l;
	DatePublications ref=NULL;
	long y;

	while(p!=NULL)
	{
		y=atol(SearchField(p->entry.listEntryFields, "year"));	
		if(isYear(DP,y)==0)
		{
			ref=getYearReferences(l,y);
			DP=InsertTailDP(DP,ref);
		}
		p=p->next;
	
	}

    if(DP!=NULL)
    {
        DP=QuickSortDP(DP);
        printf("DatePublications was succesfully created\n");
    }
    
    
	return DP;
}

/*
 * Written by Valentin LE MORVAN
-getYearReferences gets the list of publications published in a given year from a given library
-L: library in which looking for the publications
-year: year to seek in the library
-the function returns a DatePublications of one single element, with its publications sorted by author.
*/

DatePublications getYearReferences (Library L, long year)
{
	DatePublications DP=(DatePubElem *) malloc(sizeof(DatePubElem));
	LibraryElem* p=L;
	DP->publications = NULL;
	DP->next = NULL;
	DP->prev = NULL;
	DP->year=year;
	while(p!=NULL)
	{
		if(atol(SearchField(p->entry.listEntryFields,"year"))==year)
		{
			DP=InsertElemDP(DP,&(p->entry));
		}
		p=p->next;
	}
	return DP;
}

/*
 * Written by Valentin LE MORVAN
-InsertElemDP inserts a given entry in the list of publications of a given DatePubElem, inserting it sorted by first authors'name
-l: pointer on the DatePubElem
-e: Entry to insert in the Library of l
-returns a pointer on the DatePubElem.
*/
DatePublications InsertElemDP (DatePublications l, Entry* e)
{
	Library p;
	LibraryElem* newel=(LibraryElem*) malloc(sizeof(LibraryElem));
	Author a,na;
	newel->entry.key =(char *) malloc((strlen(e->key)+1)*sizeof(char));
	newel->entry.type =(char *) malloc((strlen(e->type)+1)*sizeof(char));
	newel->entry.authorList=e->authorList;
	newel->entry.listEntryFields=e->listEntryFields;
	strcpy(newel->entry.key,e->key);
	strcpy(newel->entry.type,e->type);
	newel->next=NULL;
	newel->prev=NULL;
	na.firstname=e->authorList->author.firstname;
	na.name=e->authorList->author.name;
	if(l!=NULL)
	{
		if(l->publications==NULL)
		{	
			l->publications=newel;
		}
		else
		{
			p=l->publications;
			a.name=p->entry.authorList->author.name;
			a.firstname=p->entry.authorList->author.firstname;
			while(p->next!=NULL && strcmp(a.name,na.name)<0) /*if a.name is BEFORE na.name in the alphabet*/
			{
				p=p->next;
				a.name=p->entry.authorList->author.name;
				a.firstname=p->entry.authorList->author.firstname;
			}
			if(p->next==NULL && strcmp(a.name,na.name)<0)
			{
				p->next=newel;
				newel->prev=p;
			}
			else
			{
				newel->prev=p->prev;
				p->prev=newel;
				if(newel->prev!=NULL)
				{
					newel->prev->next=newel;
				}
				newel->next=p;
			}
		}
	}

	return l;
}

/*void printDatePublications(DatePublications datePublications)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions prints a given DatePublication*/

void printDatePublications(DatePublications datePublications)
{
	DatePublications p=NULL;
	Library l=NULL;
	if (datePublications == NULL)
	{
		printf("Empty DatePublications List\n");
	}
	else
	{
		p=datePublications;
		while(p!=NULL)
		{
			printf("\tYear : %ld\n\n",p->year);
			l = p->publications;
			while(l!=NULL)
			{
				printEntry(&(l->entry));
				l=l->next;
			}
			p=p->next;
		}
	}
}

/*void exportDatePublications(DatePublications datePublications,char* filename)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions exports a given DatePublication*/

void exportDatePublications(DatePublications datePublications,char* filename)
{
	FILE* fp;
	DatePublications p=NULL;
	Library l=NULL;
	
	fp=fopen(filename,"a"); /*we want to use the add because the file is maybe not empty*/
	
	if(fp==NULL)
	{
		printf("Failed to open/create the file.\n");
	}
	else
	{
		p=datePublications;
		while(p!=NULL)
		{
			fprintf(fp,"\t%ld\n\n",p->year);
			l = p->publications;
			while(l!=NULL)
			{
				fprintEntryTitle(l->entry.listEntryFields,fp);
				fprintAuthorList(l->entry.authorList,fp);
				fprintEntryFields(l->entry.listEntryFields,fp,p->year);
				l=l->next;
			}
			fprintf(fp,"\n\n\n");
			p=p->next;
		}
		
		fclose(fp);
		printf("The file %s was filled with the DatePublication list.\n",filename);
	}
}

/*void fprintEntryTitle(ListEntryFields listEntryFields,FILE* fp)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions prints the title of an entry*/

void fprintEntryTitle(ListEntryFields listEntryFields,FILE* fp)
{
	ListEntryFields p=NULL;
	int found=0;
	
	if(listEntryFields == NULL)
	{
		fprintf(fp,"This entry has no title\n");
	}
	else
	{
		p=listEntryFields;
		while(p!=NULL && found == 0)
		{
			if(strcmp(p->entryField.fieldname,"title")==0)
			{
				fprintf(fp,"%s\n",p->entryField.value);
				found = 1;
			}
			p=p->next;
		}
		if(found==0)
		{
			fprintf(fp,"This entry has no title\n");
		}
	}
	
}

/*void fprintAuthorList(AuthorList authorList,FILE* fp)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions prints a given authorList*/
 
void fprintAuthorList(AuthorList authorList,FILE* fp)
{
	AuthorList p=NULL;
	
	if(authorList == NULL)
	{
		fprintf(fp,"Couldn't find the authors of this entry\n");
	}
	else
	{
		p=authorList;
		while(p->next!=NULL)
		{
			fprintf(fp,"%s %s, ",p->author.firstname,p->author.name);
			p=p->next;
		}
		fprintf(fp,"%s %s\n",p->author.firstname,p->author.name);
	}
	
}

/*void fprintEntryFields(ListEntryFields listEntryFields,FILE* fp,int year)
 * function made by Luc CADORET
 * Contact : luc.cadoret@utbm.fr
 * This functions prints a given ListEntryFields*/
 
void fprintEntryFields(ListEntryFields listEntryFields,FILE* fp,int year)
{
	ListEntryFields p=NULL;
	
	if(listEntryFields==NULL)
	{
		fprintf(fp,"Couldn't find the list of EntryFields\n");
	}
	else
	{
		p=listEntryFields;
		while(p!=NULL)
		{
			if(strcmp(p->entryField.fieldname,"title")!=0 && strcmp(p->entryField.fieldname,"year")!=0)
			{
				fprintf(fp,"%s : %s, ",p->entryField.fieldname,p->entryField.value);
			}
			p=p->next;
		}
		fprintf(fp,"%d.\n\n",year);
	}
}
