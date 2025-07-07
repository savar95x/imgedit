# Imgedit
A command line image editor.  

## Overview
I wanted to write good cpp code, and liked image processing, so I made a command line image editor.  
## Features
Written using the png++ library.  
- Implemented concurrency using std::threads, a 64% performance boost compared to sequential execution  
- Loading into memory first to safeguard from file reading conflict problems when running the app recursively
- Signigicantly faster than ImageMagick  

### Drawbacks
- only supports 3 channel (rgb) PNGs for now  

## Installation
```bash
git clone https://github.com/savar95x/imgedit
cd imgedit
./compile
```

## Usage
Considering you're in the root of this repo
```bash
bin/imgedit OPTION[=ARGs] INPUT [OUTPUT]
```
Example usage
```bash
bin/imgedit --colormask=0.8,0.5,0,8 imgs/in.png imgs/out.png
```
<div align="center">

| imgs/in.png | imgs/out.png |
|----------|--------|
| <img width="95%" src="imgs/in.png" /> | <img width="95%" src="imgs/out.png" /> |

</div>

### A complete demo (too long, need to truncate)
![demo](imgs/demo.gif)  


# Experience
I had first written this app using the stb_image header file, and even tho it supported five image file types, it was in practice not very fast. So I rewrote it from scratch using the png++ library, which is honestly significantly faster.  
I also divided the functions in a coupled way so that there's one monolithic function to launch worker (thread) functions correspoding to different flags, to make the codebase smaller and easier to read.  
Also, I am loading a copy of the input file into memory and then passing it to png::image, rather than reading directly. This is to make sure there are no reading conflict between multiple simultanious instances of the application (otherwise the image did not open completely with image(input) in most of the iterations while benchmarking)  

Overall this was a fun learning experience, and as much as I want to make a real time interactive image editor for linux, I think this kind of cli application is more efficient and useful in practice. Maybe in future I'd implement a frontend in React, but for now I am sticking to just the command line.  

## Todo
- [ ] implement a function to parse rgb(a)
- [ ] add rotate, steganography, gamma, hue
- [ ] error handling (less arguements, invalid file input, out of range arguments)
- [ ] make a better front end (or make streamlit work)
- [ ] rewrite using libpng eventually?
- [ ] hdr? also how does google image update the image real time (ans: applied on low res previews, gpu accelerated, start processing with what is visible on screen, output directly to screen (ram) instead of writing to disk)
- [ ] eventually add support for other common image types (jp(e)g, gif)
