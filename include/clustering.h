#pragma once

#include <svdlib.h>

typedef struct
{
	int i;
	double v;
} IntDoublePair;

typedef struct
{
	double averageInternalDistance;
	double contentValue;
	int frameCount;
	int* frames;
} Cluster;

int compare_normes(const void* a, const void* b);
void clustering(DMat Vt, double* S);
double col_norme(DMat Vt, int i);
double col_distance(DMat Vt, double* S, int i, int j);
double min_distance_to_cluster(DMat Vt, double* S, int i, Cluster cluster);
void add_frame_to_cluster(DMat Vt, int i, Cluster* cluster, IntDoublePair* normes, double minDistance);
