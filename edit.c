#include <stdio.h>
#include <string.h>
#include "edit.h"
#include "types.h"
#include "tags.h"

// Read Inputs from commanline
Status read_argvs_toedit(char *argv[], TagEditor *tageditor)
{
	// edit option
	strcpy(tageditor->edit_option, argv[2]);

	// mp3 file name
	strcpy(tageditor->mp3_filename, argv[3]);

	return t_success;
}


// Find edit option
T_Edit_Option find_edit_option(TagEditor *tageditor)
{
	int index;

	char *edit_ops[7] = {TITLE_OP, ARTIST_OP, ALBUM_OP, TRACK_OP, YEAR_OP, COMPOSER_OP, ART_OP};

	// find tag and return tag_positon
	for (index = 0; index < 7; index++)
	{
		if (strncmp(tageditor->edit_option, edit_ops[index], strlen(edit_ops[index])) == 0)
		{
			break;
		}
	}

	return (index + 1);
}


// Open file
Status open_file_editor(TagEditor *tageditor)
{
	// open mp3 file in w+ mode
	tageditor->file_ptr = fopen(tageditor->mp3_filename, "r+");

	// Error handling
	if (NULL == tageditor->file_ptr)
	{
		printf("Error..File could not be opened\n");

		return t_failure;
	}

	return t_success;
}


// Get tag to edit 
Status get_tag_to_edit(TagEditor *tageditor)
{
	// depends on user option
	switch(tageditor->tag_op)
	{
		// title
		case e_title:

			printf("Edit title\n");

			strcpy(tageditor->edit_tag, TITLE_TAG);
			
			break;

		// artist
		case e_artist:

			printf("Edit artist\n");

			strcpy(tageditor->edit_tag, ARTIST_TAG);

			break;

		// album
		case e_album:

			printf("Edit album\n");
			
			strcpy(tageditor->edit_tag, ALBUM_TAG);

			break;

		// track
		case e_track:
			
			printf("Edit track\n");

			strcpy(tageditor->edit_tag, TRACK_TAG);

			break;

		// year
		case e_year:

			printf("Edit year\n");

			strcpy(tageditor->edit_tag, YEAR_TAG);

			break;

		// composer
		case e_composer:

			printf("Edit composer\n");

			strcpy(tageditor->edit_tag, COMPOSER_TAG);

			break;

		// art
		case e_art:

			printf("Edit art\n");

			strcpy(tageditor->edit_tag, ART_TAG);

			break;
	}

}

// calculate size
int cal_size(FILE *fp)
{
	int index;

	int size = 0;
	
	unsigned char ch = '\0';

	// read 4 bytes and return size
	for (index = 0; index < sizeof (int); index++)
	{
		fread(&ch, sizeof (char), 1, fp);

		size |= ch;

		if (sizeof (int) - 1 != index)

			size <<= (sizeof(char) * 8);
	}

	return size;
}


// Find tag content size
Status find_tag_size(TagEditor *tageditor)
{
	char ch;

	int flag = 0, index;

	char buff[5];

	// find tag to edit
	while (feof(tageditor->file_ptr) == 0)
	{
		fread(&ch, sizeof (char), 1, tageditor->file_ptr);
	
		if (tageditor->edit_tag[0] == ch)
		{
			for (index = 0; index < 4; index++)
			{
				buff[index] = ch;

				if (index != 3)		

					fread(&ch, sizeof (char), 1, tageditor->file_ptr);
			}

			buff[index] = '\0';

			if (strncmp(tageditor->edit_tag, buff, 4) == 0)

				flag = 1;

			else

				flag = 0;
		}

		// then find the size of tag content
		if (flag)
		{
			tageditor->tag_size = cal_size(tageditor->file_ptr);
			
			break;
		}
	}

	return t_success;
}


// Get art file size
Status get_art_file_size(FILE *file_ptr)
{
	char ch, s1 = 0xff, s2 = 0xd8, e1 = 0xd9;

	int s_flag = 1, flag = 0, count = 0;

	// get the 1st position of art file and count the byte
	while (feof(file_ptr) == 0)
	{
		fread(&ch, sizeof (char), 1, file_ptr);

		if (ch == s1)
		{
			fread(&ch, sizeof (char), 1, file_ptr);
	
			if (ch == s2)
			{
				count = 2;
				
				while(1)
				{
					fread(&ch, sizeof (char), 1, file_ptr);
				
					count++;

					if (flag && (ch == e1))

						break;

					if (ch == s1)
					{
						flag = 1;
					}

					else
					{
						flag = 0;
					}
				}
			}
		}

		// place the FPI to initial byte
		fseek(file_ptr, -count, SEEK_CUR);

		if (flag)

			break;
	}

	// return no. of byte
	return count;
}

// Edit art file
Status edit_art_file(TagEditor *tageditor)
{
	char ch, ch1, s1 = 0xff, s2 = 0xd8, e1 = 0xd9;

	int s_flag = 1, flag = 0;

	// get art size of mp3 file
	tageditor->art_size = get_art_file_size(tageditor->file_ptr);
	
	// get art file size
	tageditor->art_file_size = get_art_file_size(tageditor->art_file_ptr);

	// initial position of art file 
	if (tageditor->art_size >= tageditor->art_file_size)
	{
		fread(&ch, sizeof (char), 1, tageditor->file_ptr);
		fread(&ch1, sizeof (char), 1, tageditor->art_file_ptr);

		if (ch == s1)
		{
			fread(&ch, sizeof (char), 1, tageditor->file_ptr);
			fread(&ch1, sizeof (char), 1, tageditor->art_file_ptr);

			if (ch == s2)
			{
				// read upto end of art file
				while (1)
				{
					fread(&ch, sizeof (char), 1, tageditor->art_file_ptr);

					fwrite(&ch, sizeof (char), 1, tageditor->file_ptr);					

					if (flag && (ch == e1))

						break;

					if (ch == s1)
					{
						flag = 1;
					}

					else
					{
						flag = 0;
					}

				}			
			}
		}

	}

	// art file size is large then print error
	else
	{
		printf("Error...Art file size is too long\n");

		return t_failure;
	}

	return t_success;
}


// Editting tag
Status editting_tag(TagEditor *tageditor)
{
	char temp[5];

	char buff[50];

	int size, index;

	char nul = '\0';

	fread(temp, sizeof (char), 2, tageditor->file_ptr);

	// option is not art
	if (tageditor->tag_op != e_art)
	{
		getchar();

		// read new content of corresponding tag
		printf("Enter the new content :  ");
		scanf("%[^\n]", buff);

		size = strlen(buff);

		// is true the write new content
		if ((size + 1) == tageditor->tag_size)
		{
			fwrite(&nul, sizeof(char), 1, tageditor->file_ptr);	

			fwrite(buff, sizeof(char), size, tageditor->file_ptr);	
		}

		// is true the write new content and write remaing byte null
		else if (size < tageditor->tag_size)
		{
			fwrite(&nul, sizeof(char), 1, tageditor->file_ptr);	

			fwrite(buff, sizeof(char), size, tageditor->file_ptr);	

			for (index = 0; index < (tageditor->tag_size - size - 1); index++)
			{
				fwrite(&nul, sizeof(char), 1, tageditor->file_ptr);	
			}
		}

		// Error
		else
		{
			printf("Error!..Size is too long\n");

			return t_failure;
		}

	}

	// edit option is art 
	else if (tageditor->tag_op == e_art)
	{
		getchar();

		// read new art file 
		printf("Enter the new art file name : ");
		scanf("%s", tageditor->art_name_ptr);

		// open new art file
		tageditor->art_file_ptr = fopen(tageditor->art_name_ptr, "r");
	
		// copy new art file to mp3 file
		edit_art_file(tageditor);
		
		fclose(tageditor->art_file_ptr);
	}

	return t_success;
}


// MAIN FUNCTION FOR TAG EDITTOR
Status tag_editor(TagEditor *tageditor)
{
	// check for valid option 
	if (tageditor->tag_op > 8)
	{
		printf("Invalid edit option\n");

		return t_failure;
	}

	// open mp3 file
	if (open_file_editor(tageditor) != t_success)
	{
		return t_failure;
	}

	// get tag to edit
	get_tag_to_edit(tageditor);

	// find tag size
	find_tag_size(tageditor);

	// editting the tag
	editting_tag(tageditor);

	// close mp3 and art file
	fclose(tageditor->file_ptr);

	return t_success;
}
