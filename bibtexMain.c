#include <bibtexManager.h>

/*To do : FIRSTNAME AND NAME ARE INVERTED !!! FIRSTNAME = PRENOM, NAME = NOM*/

int main(int argc,char* argv[])
{
	int choice=-1;
	char* response = (char *) malloc(500*sizeof(char));
	char* path = (char *) malloc(500*sizeof(char));
	char* entryKey = (char *) malloc(500*sizeof(char));
	Library library = NULL;
	AuthorsPublications authorsPublications = NULL;
	DatePublications datePublications = NULL;
	
	system("clear");
	printf("\t####################################\n");
	printf("\t#                                  #\n");
	printf("\t#       bibtexManager V 1.3	   #\n");
	printf("\t#                                  #\n");
	printf("\t# Valentin LE MORVAN & Luc CADORET #\n");
	printf("\t#                                  #\n");
	printf("\t####################################\n");
	printf("\nWelcome to bibtexManager. What do you want to do ?\n\n");
	do
	{
		printf("\t1) Create a library from a bibtex file\n\t2) Print the builded library\n\t3) Remove an entry\n");
		printf("\t4) Sort the Library by authors and each author publication by date\n\t5) Print the created list in 4)\n");
		printf("\t6) Sort the Library by date et each date by authors\n\t7) Print the created list in 6)\n");
		printf("\t8) Export the list created in 6) into a file\n");
		printf("\t9) Exit the program\n");
	
		printf("Choice : ");
		scanf("%s",response);
		printf("\n");
		
		if(strlen(response)>1)
		{
			printf("You didn't choose one of the choices, please try again\n");
		}
		else
		{
			choice = atoi(response);

			if(choice == 1)
			{
				printf("Enter the path of the bibtex file : ");
				scanf("%s",path);
				library = parseBibtexFile(path);
			
			}
			else if(choice == 2)
			{
				printLibrary(library);
			
			}
			else if(choice == 3)
			{
				printf("What is the entry key of the entry you want to remove ? ");
				scanf("%s",entryKey);
				library = removeEntry(library,entryKey);
				
			}
			else if(choice == 4)
			{
				authorsPublications = SortLibraryAuthorDate(library);
			}
			else if(choice == 5)
			{
				printAuthorsPublications(authorsPublications);
				
			}
			else if(choice == 6)
			{
				datePublications = SortLibraryDateAuthor(library);
			}
			else if(choice == 7)
			{
				printDatePublications(datePublications);
			}
			else if(choice == 8)
			{
				printf("In which file do you want to export datePublications ? ");
				scanf("%s",path);
				printf("\n");
				exportDatePublications(datePublications,path);
			}
			else
			{
				if(choice != 9)
				{
					printf("You didn't choose one of the choices, please try again\n");
				}
			}
		}
		
	}while(choice != 9);




	return 0;
}
