#ifndef BIBTEXMANAGER_H_INCLUDED
#define BIBTEXMANAGER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 2000

/*
Library is the main list. It regroups the authors of the publication, the date, the title etc... It's the list of the entries
*/

	/*the entryfields are the content of the entry EXCEPT the authors that are in a different list (AuthorList)*/
	typedef struct {
		char* fieldname;
		char* value;
	}EntryField;

	typedef struct entrfi{
		EntryField entryField;
		struct entrfi* next;
		struct entrfi* prev;
	}EntryFieldElem;

	/*the list of entryfields*/
	typedef EntryFieldElem*  ListEntryFields;

	/*
	This type represents an author
	*/
	typedef struct {
		char* name;
		char* firstname;
	}Author;

	typedef struct elemAuthList{
		Author author;
		struct elemAuthList *next;
		struct elemAuthList *prev;
	}AuthorListElem;

	/*AuthorList = doubly-linked list of authors*/
	typedef AuthorListElem* AuthorList;

	/*Element of the Library list, and Entry is : a list of authors, and a list of entryfields*/
	typedef struct {
		char* type;
		char* key;
		AuthorList authorList;
		ListEntryFields listEntryFields;
	}Entry;

	typedef struct libElem{
		Entry entry;
		struct libElem* next;
		struct libElem* prev;
	}LibraryElem;

	/*Definition of the list*/
	typedef LibraryElem* Library;

	/*Each element of the AuthorsPublications list contain an author and a linked list of his publications */
	typedef struct authorPub{
		Author author;
		Library publications;
		struct authorPub *next;
		struct authorPub *prev;
	}AuthorPubElem;
	typedef AuthorPubElem* AuthorsPublications;


	/*Each element of DatePublications contain an integer representing the year of publication, and a linked list of publications*/
	typedef struct datePub{
		long year;
		Library publications;
		struct datePub *next;
		struct datePub *prev;
	}DatePubElem;
	typedef DatePubElem* DatePublications;

/*major functions*/
Library parseBibtexFile(char* path);
Library insertEntry(Entry* newEntry,Library library);
Library removeEntry(Library library,char* entryKey);
AuthorsPublications SortLibraryAuthorDate (Library l);
AuthorsPublications getAuthorReferences (Library L,Author* a);
DatePublications getYearReferences (Library L, long year);
DatePublications SortLibraryDateAuthor (Library l);
void exportDatePublications(DatePublications datePublications,char* filename);

Library findEntryToRemove(Library library,char* entryKey);

Library insertTailLib(Library library,Entry *tempEntry);
Entry findEntryFields(Entry *tempEntry,FILE* fp);

int isTitleDifferent(ListEntryFields listEntryFields,ListEntryFields newListEntryFields);
int isAuthorListDifferent(AuthorList authorList,AuthorList newAuthorList);
int compareEntries(Entry* newEntry,Library library);
int isAuthor(AuthorsPublications l, Author* a);
int isYear(DatePublications l, long y);

void freeEntry(Entry* tempEntry);
void initializeEntry(Entry* tempEntry);

AuthorList buildAuthorList(AuthorList authorList,char* line,FILE* fp);
AuthorList insertTailAuthorList(Author* tempAuthor,AuthorList authorList);
ListEntryFields buildEntryFieldList(ListEntryFields listEntryFields,char* line,FILE* fp);
ListEntryFields insertTailListEntrFi(EntryField *tempEntryField,ListEntryFields listEntryFields);

void printAuthorList(AuthorList authorList);
void printEntryFieldList(ListEntryFields listEntryFields);
void printEntry(Entry *tempEntry);
void printLibrary(Library library);
void printAuthorsPublications(AuthorsPublications l);
void printDatePublications(DatePublications datePublications);
void fprintEntryTitle(ListEntryFields listEntryFields,FILE* fp);
void fprintEntryFields(ListEntryFields listEntryFields,FILE* fp,int year);
void fprintAuthorList(AuthorList authorList,FILE* fp);

char* SearchField (ListEntryFields l, char* s);

AuthorsPublications InsertTail(AuthorsPublications l, AuthorPubElem* e);
AuthorsPublications Concatenate(AuthorsPublications l1, AuthorsPublications l2, AuthorsPublications l3);
AuthorsPublications QuickSort(AuthorsPublications L);
AuthorsPublications InsertElemAP(AuthorPubElem* l, Entry* e);

DatePublications InsertElemDP (DatePublications l, Entry* e);
DatePublications InsertTailDP(DatePublications l, DatePubElem* e);
DatePublications ConcatenateDP(DatePublications l1, DatePublications l2, DatePublications l3);

#endif /*BIBTEXMANAGER_H_INCLUDED*/
