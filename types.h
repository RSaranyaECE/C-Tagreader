#ifndef TYPES
#define TYPES

typedef unsigned int uint;
typedef unsigned char uchar;

typedef enum 
{
	tfailure,
	t_read,
	t_edit

} T_Option;

// editor
typedef enum
{
	e_failure,
	e_title,
	e_artist,
	e_album,
	e_track,
	e_year,
	e_composer,
	e_art

} T_Edit_Option;

// reader
typedef enum
{
	r_title,
	r_artist,
	r_album,
	r_track,
	r_year,
	r_composer,
	r_art

} T_Read_Option;

typedef enum
{
	t_success,
	t_failure

} Status;

#endif
