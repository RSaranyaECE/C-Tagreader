#ifndef TAG_EDIT
#define TAG_EDIT

#include "types.h"

// Structure for tag editor

typedef struct _TagEditor
{
	char mp3_filename[32];
	
	FILE *file_ptr;

	int tag_op;

	char edit_option[10];

	char edit_tag[5];

	int tag_size;

	int art_size;
	
	char art_name_ptr[32];
	FILE *art_file_ptr;
	int art_file_size;


} TagEditor;


// function prototypes
Status read_argvs_toedit(char *argv[], TagEditor *tageditor);
T_Edit_Option find_edit_option(TagEditor *tageditor);
Status tag_editor(TagEditor *tageditor);
Status get_tag_to_edit(TagEditor *tageditor);
Status tag_editor(TagEditor *tageditor);
Status open_file_editor(TagEditor *tageditor);


#endif
