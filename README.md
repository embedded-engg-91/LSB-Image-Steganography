# 🔐 LSB Image Steganography

A C-based implementation of **Least Significant Bit (LSB) Image Steganography** that securely hides a secret text file inside a BMP 
image while preserving the visual quality of the cover image. The hidden data can later be extracted accurately using the decoding process.

## ✨ Features

- Hide text files inside BMP images
- Extract hidden data from encoded images
- Preserves image quality after encoding
- Supports secret file extensions
- Capacity validation before encoding
- Command-line interface

## 🛠 Technologies Used

- C Programming
- File Handling
- Bit Manipulation
- Bitmap (BMP) Image Format
- GCC

## ⚙️ Build

```bash
gcc *.c -o stego
```

## 🚀 Usage

### To Encode:

```bash
./stego -e beautiful.bmp secret.txt
```

Output:

```
stego.bmp
```

### To Decode:

```bash
./stego -d stego.bmp
```

Output:

```
decoded.txt
```

## 📖 Concepts Used

- Least Significant Bit (LSB) Encoding
- Bitwise Operators
- BMP File Structure
- Error Handling
- Modular Programming


 ## 👨‍💻 Author

**Manoj B**
