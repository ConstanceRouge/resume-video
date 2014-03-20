#pragma once

bool get_video_fps(const char* filename, int* fps);
bool extract_images(const char* filename, int interval, int fps, const char* imagefolder);
