#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "tags.h"
#include "read.h"
#include <string.h>


/* Tag Reader Function Definitions */


// Read arguments from commandline
Status read_argvs_toread(char *argv[], TagReader *tagreader)
{
	// read file name
	strcpy(tagreader->mp3_filename,argv[2]);

	printf("MP3_Filename :  %s\n", tagreader->mp3_filename);

	return t_success;
}


// Open file 
Status open_file_reader(TagReader *tagreader)
{
	// Open mp3 file
	tagreader->file_ptr = fopen(tagreader->mp3_filename, "r");

	// Error handling
	if (NULL == tagreader->file_ptr)
	{
		printf("Error..File could not be opened\n");

		return t_failure;
	}

	return t_success;
}


// New art file name
Status new_art_file_name(TagReader *tagreader)
{
	int index = 0, idx;

	char extn[5] = ".jpg";

	// adding extension to artfile
	tagreader->art_name_ptr = calloc(strlen(tagreader->mp3_filename) + 5, sizeof (char));

	strcpy(tagreader->art_name_ptr, tagreader->mp3_filename);

	while(tagreader->art_name_ptr[index] != '\0')
	{
		index++;
	}

	for (idx = 0; idx < 5; idx++, index++)
	{
		tagreader->art_name_ptr[index] = extn[idx];
	}

	return t_success;
}



// Calculate size 
int calc_size(FILE *fp)
{
	int index;

	int size = 0;
	
	unsigned char ch = '\0';

	// read 4 bytes to get size
	for (index = 0; index < sizeof (int); index++)
	{
		fread(&ch, sizeof (char), 1, fp);

		size |= ch;

		if (sizeof (int) - 1 != index)

			size <<= (sizeof(char) * 8);
	}

	// return size
	return size;
}

// Read tag content
Status read_tag(int *size, char **ptr, FILE *file_ptr)
{
	char temp[5];

	int index;

	char ch;

	// get size
	*size = calc_size(file_ptr);

	// allocate memory to heap for tag content
	*ptr = calloc(*size, sizeof (char));

	// read chars upto size
	fread(temp, sizeof (char), 2, file_ptr);

	for (index = 0; index < *size; index++)
	{
		fread(&ch, sizeof (char), 1, file_ptr);
	
		*(*ptr + index) = ch;
	}

	return t_success;
}


Status read_art(TagReader *tagreader)
{
	char ch;

	char temp_start[2] = {0xff, 0xd8};

	char s1 = 0xff, s2 = 0xd8, e1 = 0xff, e2 = 0xd9;

	int index, flag = 0, b_flag = 0;

	// open new art file
	tagreader->art_file_ptr = fopen(tagreader->art_name_ptr, "w");

	// get the starting position of image
	while (feof(tagreader->file_ptr) == 0)
	{
		fread(&ch, sizeof (char), 1, tagreader->file_ptr);
		
		if (ch == s1)
		{
			fread(&ch, sizeof (char), 1, tagreader->file_ptr);

			if (ch == s2)
			{
				fread(&ch, sizeof (char), 1, tagreader->file_ptr);
		
				fwrite(temp_start, sizeof (char), 2, tagreader->art_file_ptr);
				
				// copy bytes from mp3 file to new art file
				while (feof(tagreader->file_ptr) == 0)
				{
					fread(&ch, sizeof (char), 1, tagreader->file_ptr);

					fwrite(&ch, sizeof (char), 1, tagreader->art_file_ptr);

					if (flag && (ch == e2))
					{
						break;
					}

					if (ch == e1)
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

		// break when u find the ending
		if (flag)
		{
			break;
		}
	}
	
	fclose(tagreader->art_file_ptr);
	
	return t_success;
}


// Choose tagreader
Status choose_tagreader(T_Read_Option r_option, TagReader *tagreader)
{
	// depends on r_option read
	switch(r_option)
	{
		// title
		case r_title:

			read_tag(&tagreader->title_size, &tagreader->title_ptr, tagreader->file_ptr);
			
			break;

		// artist
		case r_artist:

			read_tag(&tagreader->artist_name_size, &tagreader->artist_name_ptr, tagreader->file_ptr);

			break;

		// album
		case r_album:

			read_tag(&tagreader->album_name_size, &tagreader->album_name_ptr, tagreader->file_ptr);
			
			break;

		// track
		case r_track:

			read_tag(&tagreader->track_size, &tagreader->track_ptr, tagreader->file_ptr);
			
			break;

		// year
		case r_year:

			read_tag(&tagreader->year_size, &tagreader->year_ptr, tagreader->file_ptr);
			
			break;

		// composer
		case r_composer:

			read_tag(&tagreader->composer_name_size, &tagreader->composer_name_ptr, tagreader->file_ptr);
			
			break;

		// art
		case r_art:
			
			read_art(tagreader);

			break;
	
	}

	return t_success;

}


// Find tag
T_Read_Option find_tagreader(char *tag_buff)
{
	int index;

	char *tag_ptr[7] = {TITLE_TAG, ARTIST_TAG, ALBUM_TAG, TRACK_TAG, YEAR_TAG, COMPOSER_TAG, ART_TAG};

	// return the correspopnding tag position value
	for (index = 0; index < 7; index++)
	{
		if (strncmp(tag_buff, tag_ptr[index], 4) == 0)

			break;
	}

	return index;

}

// Get Tag
Status get_tag(TagReader *tagreader)
{
	char ch, tag_buff[5];

	int index, count = 0;

	T_Read_Option r_option;

	// Get Tags
	while (feof(tagreader->file_ptr) == 0)
	{
		fread(&ch, sizeof (char), 1, tagreader->file_ptr);

		if (ch == 'T' || ch == 'A')
		{
			// Tags to buff
			for (index = 0; index < 4; index++)
			{
				tag_buff[index] = ch;

				if (index != 3)

					fread(&ch, sizeof (char), 1, tagreader->file_ptr);
			}

			tag_buff[index] = '\0';

			// find read option
			r_option = find_tagreader(tag_buff);

			if (r_option < 7)
			{
				// choose tag reader function
				choose_tagreader(r_option, tagreader);

				count++;
			}
		}

		if (count == 7)

			break;
	}

	return t_success;
}


// Print tag
Status print_tag(int *size, char *ptr)
{
	int index;

	char ch;

	// Print only printable
	for (index = 0; index < *size; index++)
	{
		ch = ptr[index];

		if (ch > 31 && ch != 92 && ch != 127)

			printf("%c", ch);
	}

	printf("\n");
}


// Print all tags
Status print_all_tags(TagReader *tagreader)
{
	// Print title
	printf("Song Name :  ");
	if (tagreader->title_ptr != NULL)

		print_tag(&tagreader->title_size, tagreader->title_ptr);

	else

		printf("-- No Info --\n");

	// Print album
	printf("Album Name :  ");
	if (tagreader->album_name_ptr != NULL)

		print_tag(&tagreader->album_name_size, tagreader->album_name_ptr);

	else

		printf("-- No Info --\n");

	// print artist
	printf("Artist Name :  ");
	if (tagreader->artist_name_ptr != NULL)

		print_tag(&tagreader->artist_name_size, tagreader->artist_name_ptr);

	else

		printf("-- No Info --\n");

	// print track
	printf("Track :  ");
	if (tagreader->track_ptr != NULL)

		print_tag(&tagreader->track_size, tagreader->track_ptr);

	else

		printf("-- No Info --\n");

	// print year
	printf("Year :  ");
	if (tagreader->year_ptr != NULL)

		print_tag(&tagreader->year_size, tagreader->year_ptr);

	else

		printf("-- No Info --\n");

	// print composer
	printf("Composer :  ");
	if (tagreader->composer_name_ptr != NULL)

		print_tag(&tagreader->composer_name_size, tagreader->composer_name_ptr);

	else

		printf("-- No Info --\n");

	// extract art to new art file
	printf("Art file:  ");
	if (tagreader->art_file_ptr != NULL)

		printf("%s\n", tagreader->art_name_ptr);

	else

		printf("-- No Info --\n");
}


// MAIN TAG READER FUNCTION
Status tag_reader(TagReader *tagreader)
{
	tagreader->title_ptr = NULL;
	tagreader->artist_name_ptr = NULL;
	tagreader->album_name_ptr = NULL;
	tagreader->track_ptr = NULL;
	tagreader->year_ptr = NULL;
	tagreader->composer_name_ptr = NULL;
	tagreader->art_file_ptr = NULL;
	tagreader->art_name_ptr = NULL;

	// open file
	if (open_file_reader(tagreader) != t_success)
	{
		return t_failure;
	}

	// save new art file name
	new_art_file_name(tagreader);

	// get tags to read
	get_tag(tagreader);

	// print tags
	print_all_tags(tagreader);

	// close mp3 file
	fclose(tagreader->file_ptr);

	// free heap memory
	free(tagreader->title_ptr);
	free(tagreader->album_name_ptr);
	free(tagreader->artist_name_ptr);
	free(tagreader->composer_name_ptr);
	free(tagreader->year_ptr);
	free(tagreader->track_ptr);
	free(tagreader->art_name_ptr);

	// making ptr to NULL
	tagreader->title_ptr = NULL;
	tagreader->album_name_ptr = NULL;
	tagreader->artist_name_ptr = NULL;
	tagreader->composer_name_ptr = NULL;
	tagreader->year_ptr = NULL;
	tagreader->track_ptr = NULL;
	tagreader->art_name_ptr = NULL;
	
	return t_success;
}
