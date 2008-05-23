/*------------------------------------------------------------------------------
	Filename: example3.c
	
	Description: Loads example3-input.gbx, duplicates it and offsets it to the
		right by the width of the layer, changed the rendered color of the
		second image, then exports a PNG rendering of the overlaid images.

	Instructions: Make sure you are in the example-code directory, and compile
		this program with the following command:

	gcc -Wall -g `pkg-config --cflags gtk+-2.0 glib-2.0 libgerbv` `pkg-config \
--libs gtk+-2.0 glib-2.0 libgerbv` example3.c -o example3

	Run with the following command:
	
	./example3

------------------------------------------------------------------------------*/

#include "gerbv.h"

int
main(int argc, char *argv[]) {
	/* create a top level libgerbv structure */
	gerbv_project_t *mainProject = gerbv_create_project();

	/* parse a Gerber file and store it in the gerbv_project_t struct, and
	   then immediately parse a second copy */
	gerbv_open_layer_from_filename (mainProject, "example3-input.gbx");
	gerbv_open_layer_from_filename (mainProject, "example3-input.gbx");

	/* make sure we parsed the files */
	if ((mainProject->file[0] == NULL) || (mainProject->file[1] == NULL))
		g_error ("There was an error parsing the files.");
	
	/* translate the second image (file[1]) up and right by 0.02 inches */
	mainProject->file[1]->transform.translateY = 0.02;
	mainProject->file[1]->transform.translateX = 0.02;
	
	/* change the color of the first image (file[0]) to green */
	GdkColor greenishColor = {0, 10000, 65000, 10000};
	mainProject->file[0]->color = greenishColor;
	
	/* export a rendered PNG image of the project, using the autoscale version
	   to automatically center the image */
	gerbv_export_png_file_from_project_autoscaled (mainProject, 640, 480,
					"example3-output.png");
	
	/* destroy the project, which will in turn destroy all child images */
	gerbv_destroy_project (mainProject);
	return 0;
}