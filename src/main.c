#include "common.h"
#include "videoutils.h"

#include <unistd.h>

int main(int argc, char *argv[])
{	
	if (argc < 2)
	{
		printf("Usage : resume-video video.avi\n");
		return EXIT_FAILURE;
	}
	
	char* filename = argv[1];
	if (access(filename, F_OK) == -1)
	{
		printf("Le fichier vidéo indiqué en paramètre n'existe pas !\n");
		return EXIT_FAILURE;
	}
	
	int fps;
	if (!get_video_fps(filename, &fps))
	{
		printf("Ne peut récupérer le nombre d'images par seconde de la vidéo !\n");
		return EXIT_FAILURE;
	}
	
	extract_images(filename, 10, fps, "tmp/");
	
	return EXIT_SUCCESS;
}
