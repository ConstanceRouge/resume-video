#pragma once

#include <svdlib.h>

bool extract_features_matrix(DMat* matrix, const char* imagefolder, bool* transposed);
void construct_vector(double* vector, unsigned char* imageData, int width, int height);
void construct_histogram(double* vector, unsigned char* imageData, int width, int height, int blocX, int blocY);
