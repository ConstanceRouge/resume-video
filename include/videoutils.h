#pragma once

bool get_video_fps(const char* filename, int* fps);
void extract_images(const char* filename, int interval, int fps, const char* folder);
