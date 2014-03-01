#include "common.h"
#include "videoutils.h"

bool get_video_fps(const char* filename, int* fps)
{
	char command[128] = {'\0'};
	sprintf(command, "ffmpeg -i %s 2>&1 | sed -n \"s/.*, \\(.*\\) fp.*/\\1/p\"", filename);
	
	FILE* fp = popen(command, "r");
	
	if (!fp)
	{
		printf("Echec de l'exécution de la commande de récupération du framerate !\n");
		return false;
	}
	
	char fpsStr[4] = {'\0'};
	
	if (!fgets(fpsStr, sizeof(fpsStr)-1, fp))
	{
		printf("Echec de la récupération de la sortie de la commande de récupération du framerate !\n");
		return false;
	}
	
	pclose(fp);
	
	char* endPtr;
	*fps = strtol(fpsStr, &endPtr, 10);
	
	return true;
}

void extract_images(const char* filename, int interval, int fps, const char* folder)
{
	char command[128] = {'\0'};
	sprintf(command, "ffmpeg -i %s -vf \"select=not(mod(n\\,%d)),setpts=N/(%d*TB)\" %s/output%%d.png", filename, interval, fps, folder);
	
	system(command);
	
	printf("plop\n");
}
