#include <stdio.h>
#include <string.h>
#include "read.h"
#include "edit.h"
#include "types.h"


// GET OPTION FUNTION
T_Option get_option(char **argv)
{
	// read
	if (strncmp(*argv, "-r", 2) == 0)

		return t_read;

	// edit
	else if (strncmp(*argv, "-e", 2) == 0)

		return t_edit;
	
	// failure
	else 

		return tfailure;

}


// READ INPUTS FROM STDIN
void read_inputs_stdin(TagReader *tagreader, TagEditor *tageditor, int option)
{
	// read
	if (1 == option)
	{
		// read file name
		printf("Enter the file name : ");
		scanf("%s", tagreader->mp3_filename);

		printf("MP3_File Name :\t%s\n", tagreader->mp3_filename);
	}

	// edit
	else if (2 == option)
	{
		// read file name
		printf("Enter the file name : ");
		scanf("%s", tageditor->mp3_filename);
		
		// read edit option
		printf("OPTIONS:\n\t1.Title\n\t2.Artist\n\t3.Album\n\t4.Track\n\t5.Year\n\t6.Composer\n\t7.Art\n\n");

		printf("Enter the edit option");
		scanf("%d", &tageditor->tag_op);
	}

}


// MAIN FUNCTION
int main(int argc, char *argv[])
{
	int option;
	char decision;

	TagReader tagreader;
	TagEditor tageditor;

	do
	{
		if (argc == 1)
		{
			// read option
			printf("OPTIONS:\n\t1.Read\n\t2.Edit\n\n");

			printf("Enter the option :");
			scanf("%d", &option);

			// read input function from stdin
			read_inputs_stdin(&tagreader, &tageditor, option);
		}

		else
		{
			// Error handling
			if (argc != 3 && argc != 4)
			{
				printf("Tagreader : ./a.out <-r> <mp3filename>\n");
				printf("Tageditor : ./a.out <-e> <editor option> <mp3filename>\n");

				return t_failure;
			}
		
			else

				// call get option function for command line 
				option = get_option(&argv[1]);	
		
		}

		switch (option)
		{	
			// read
			case t_read:
			
				// command line input read for reading
				if (argc != 1)

					read_argvs_toread(argv, &tagreader);

				// tag reader function
				tag_reader(&tagreader);

				break;

			case t_edit:

				// command line input read for edittiong
				if (argc != 1)
				{			
					read_argvs_toedit(argv, &tageditor);

					tageditor.tag_op = find_edit_option(&tageditor);
				}
				
				// tag editor
				tag_editor(&tageditor);

				break;

			// Error 
			default:

				printf("Invalid option\n");

		}

		// To continue...
		if (argc == 1)
		{
			printf("Do you want to continue...");
			scanf("\n%c", &decision);
		}

		else

			return 0;

	}while (decision == 'y' || decision == 'Y');

	return 0;
}
