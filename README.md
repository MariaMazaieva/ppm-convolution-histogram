# 🖼️ PPM Image Processor in C

A lightweight C program that processes PPM (P6) image files by applying a convolution-based sharpening filter and generating a brightness histogram. Ideal for learning low-level image manipulation and pixel-level operations.

---

## ✨ Features

- ✅ Reads PPM (P6) image files
- 🔍 Applies a convolution filter to sharpen the image
- 📊 Calculates brightness histogram (5-level grayscale bands)
- 💾 Saves filtered image (`output.ppm`)
- 📝 Writes histogram data to `output.txt`

---

## 🛠️ How It Works

1. **Reads** a binary `.ppm` file (must be in P6 format)
2. **Validates** image format and dimensions
3. **Allocates** memory for image data
4. **Applies** convolution filter to enhance image sharpness
5. **Generates** brightness histogram based on Y channel luminance
6. **Writes**:
   - Enhanced image to `output.ppm`
   - Histogram counts to `output.txt`

---

## 📂 Project Structure

| File                | Description                                      |
|---------------------|--------------------------------------------------|
| `main.c`            | Main program logic                               |
| `convolution()`     | Applies the sharpening filter                    |
| `histogram()`       | Analyzes pixel brightness and builds histogram   |
| `saveImage_ppm()`   | Outputs processed image in PPM format            |
| `is_scan_ok()`      | Validates image format and reads header          |
| `buffer_alloc()`    | Dynamically allocates memory for image data      |
| `buf_not_eql_size()`| Handles partial image reads                      |

---

## 🚀 How to Run

### 🧰 Prerequisites
- GCC or any C compiler
- A valid PPM (P6) image

### 🧪 Compile the program:

```bash
gcc -o ppm_processor main.c -lm
