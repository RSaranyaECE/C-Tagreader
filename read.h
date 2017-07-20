#ifndef TAG_READ
#define TAG_READ

#include "types.h"

// structure for tagreader

typedef struct _TagReader
{
	char mp3_filename[32];
	FILE *file_ptr;

	int title_size;
	char *title_ptr;
	
	int artist_name_size;
	char *artist_name_ptr;
	
	int album_name_size;
	char *album_name_ptr;
	
	int track_size;
	char *track_ptr;

	int year_size;
	char *year_ptr;

	int composer_name_size;
	char *composer_name_ptr;
	
	int art_size;
	char *art_name_ptr;
	FILE *art_file_ptr;

} TagReader;


// function prototypes

Status read_argvs_toread(char *argv[], TagReader *tagreader);
Status open_file(TagReader *tagreader);
int calc_size(FILE *fp);
Status read_tag(int *size, char **ptr, FILE *file_ptr);
Status choose_tagreader(T_Read_Option r_option, TagReader *tagreader);
T_Read_Option find_tagreader(char *tag_buff);
Status get_tag(TagReader *tagreader);
Status tag_reader(TagReader *tagreader);


#endif

