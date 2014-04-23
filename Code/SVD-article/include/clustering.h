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

typedef struct
{
	int frameCount;
	int* frames;
} Shot;

int compare_normes(const void* a, const void* b);
int compare_ints(const void* a, const void* b);
Cluster* clustering(DMat Vt, double* S, int* numClusters);
double col_norme(DMat Vt, double* S, int i);
double col_distance(DMat Vt, double* S, int i, int j);
double min_distance_to_cluster(DMat Vt, double* S, int i, Cluster cluster);
double max_distance_to_cluster(DMat Vt, double* S, int i, Cluster cluster);
void add_frame_to_cluster(DMat Vt, int i, Cluster* cluster, IntDoublePair* normes, double minDistance);
Shot find_longest_shot(Cluster cluster);
Shot* find_shots(Cluster cluster, int* shotCount);
double col_distance_col(DMat Vt, double* S, int i, double* col);
int find_best_frame(Cluster cluster, DMat Vt, double* S);
