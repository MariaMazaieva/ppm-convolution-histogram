#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

bool is_scan_ok(FILE *f, char imgformat[3], int *w, int *h, int *max);

unsigned char *buffer_alloc(int width, int height);

void buf_not_eql_size(FILE *f, unsigned char *buffer, size_t count_fread);

void histogram(unsigned char *buffer, int w, int h);

unsigned char *convolution(unsigned char *buffer, int w, int h);

void saveImage_ppm(const char *filename, unsigned char *data, int w, int h);

/*
* main function:
* Loads and validates a PPM image file
* Reads pixel data into memory
* Applies convolution and computes a histogram
* Saves and frees resources before exiting
*/
int main(int argc, char **argv) {
    bool ret = true;
    char const *filename = argc > 1 ? argv[1] : NULL;
    int width = 0, height = 0, max = 0;
    char imgformat[3];

    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Failed to load the file\n");
        // fclose(f);
        return EXIT_FAILURE;
    }
    is_scan_ok(f, imgformat, &width, &height, &max);
    size_t size = 3 * width * height;
    unsigned char *buffer = buffer_alloc(width, height);
    if (!buffer) {
        fclose(f);
        return false;
    }

    size_t count_fread = fread(buffer, 1, size, f);

    if (count_fread != size) {
        buf_not_eql_size(f, buffer, count_fread);
        return false;
    }

    unsigned char *cpy = convolution(buffer, width, height);
    free(buffer);

    histogram(cpy, width, height);
    free(cpy);
    fclose(f);
    return ret;
}

/*
 * Applies a sharpening convolution filter to the image and saves the result to output.ppm.
 * Returns the filtered image.
 */
unsigned char *convolution(unsigned char *buffer, int w, int h) {
    unsigned char *cpy = malloc(3 * w * h);
    if (!cpy) {
        fprintf(stderr, "ERROR: malloc failed");
        return NULL;
    }
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int sum_R = 0, sum_G = 0, sum_B = 0;
            int pxl_id = (y * w + x) * 3;

            //Index offsets
            int pos_left = pxl_id - 3;
            int pos_right = pxl_id + 3;
            int pos_up = pxl_id - (w * 3);
            int pos_down = pxl_id + (w * 3);

            sum_R = buffer[pos_up + 0] * -1 + buffer[pos_left + 0] * -1 +
                    buffer[pxl_id + 0] * 5 + buffer[pos_right + 0] * -1 + buffer[pos_down + 0] * -1;

            sum_G = buffer[pos_up + 1] * -1 + buffer[pos_left + 1] * -1 +
                    buffer[pxl_id + 1] * 5 + buffer[pos_right + 1] * -1 + buffer[pos_down + 1] * -1;

            sum_B = buffer[pos_up + 2] * -1 + buffer[pos_left + 2] * -1 +
                    buffer[pxl_id + 2] * 5 + buffer[pos_right + 2] * -1 + buffer[pos_down + 2] * -1;

            // in range 0–255
            sum_R = (sum_R < 0) ? 0 : (sum_R > 255) ? 255 : sum_R;
            sum_G = (sum_G < 0) ? 0 : (sum_G > 255) ? 255 : sum_G;
            sum_B = (sum_B < 0) ? 0 : (sum_B > 255) ? 255 : sum_B;

            cpy[pxl_id + 0] = (unsigned char) sum_R;
            cpy[pxl_id + 1] = (unsigned char) sum_G;
            cpy[pxl_id + 2] = (unsigned char) sum_B;
        }
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (y == 0 || y == h - 1 || x == 0 || x == w - 1) {
                int idx = (y * w + x) * 3;
                cpy[idx + 0] = buffer[idx + 0];
                cpy[idx + 1] = buffer[idx + 1];
                cpy[idx + 2] = buffer[idx + 2];
            }
        }
    }

    saveImage_ppm("output.ppm", cpy, w, h);
    return cpy;
}

/*
 *Computes a brightness histogram of the image using the Y (luminance) value
 *writes results to output.txt.
 */
void histogram(unsigned char *buffer, int w, int h) {
    int histogram[5] = {0, 0, 0, 0};
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            int ptr = (row * w + col) * 3;
            unsigned char R = buffer[ptr];
            unsigned char G = buffer[ptr + 1];
            unsigned char B = buffer[ptr + 2];
            // int Y = round(0.2126*R + 0.7152*G + 0.0722*B);
            int Y = (int) (0.2126 * R + 0.7152 * G + 0.0722 * B + 0.5);

            if (Y <= 50) histogram[0]++;
            else if (Y <= 101) histogram[1]++;
            else if (Y <= 152) histogram[2]++;
            else if (Y <= 203) histogram[3]++;
            else if (Y <= 255) histogram[4]++;
            else histogram[4]++;
        }
    }
    FILE *out = fopen("output.txt", "w");
    fprintf(out, "%d %d %d %d %d", histogram[0], histogram[1], histogram[2], histogram[3], histogram[4]);
    fclose(out);
}

/*
 * Checks if image file is in the correct PPM P6 format
 * reads its width, height, and max color value
 */
bool is_scan_ok(FILE *f, char imgformat[3], int *w, int *h, int *max) {
    bool ok = true;
    int scan = 0;
    scan = fscanf(f, "%2s %d %d %d\n", imgformat, w, h, max);
    if (strcmp("P6", imgformat) != 0 || scan != 4 || *max != 255) {
        ok = false;
        fclose(f);
    }

    return ok;
}

/*
 * Handles errors when the actual image data read is not equal to the expected size.
 * Frees memory and closes the file.
 */
void buf_not_eql_size(FILE *f, unsigned char *buffer, size_t count_fread) {
    fprintf(stderr, "Failed to read pixel data\n");
    free(buffer);
    fclose(f);
}

/*
 * Allocates memory for the image pixel buffer
 * (size: width × height × 3 channels for RGB)
 */
unsigned char *buffer_alloc(int width, int height) {
    size_t size = 3 * width * height;
    unsigned char *buffer = malloc(size); // malloc(sizeof(size));
    if (!buffer) {
        printf("Failed to allocate the memory for buffer\n");
    }
    return buffer;
}

/*
 * Saves the given image data in PPM P6 format to the specified file.
 */
void saveImage_ppm(const char *filename, unsigned char *data, int w, int h) {
    FILE *out = fopen(filename, "wb");
    if (!out) {
        perror("Failed to open output file");
        return;
    }
    fprintf(out, "P6\n%d %d\n255\n", w, h);
    fwrite(data, 1, 3 * w * h, out);
    fclose(out);
}
