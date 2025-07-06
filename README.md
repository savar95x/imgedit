# Imgedit
A command line image editor.  

## Overview
I wanted to write good cpp code, so I thought to make one project completely from scratch. Pixel manipulations and image processing always fascinated me, so I thought to myself, why not make an Image Editor.  
I wanted to start small, so I only made a cli application

## features
(Re)written using the png++ library (earlier i used the stb_image header which was noticeably slower).  
- implemented concurrency using std::threads, a 64% performance boost compared to sequential execution  
- uses the efficient png++ library built on the official libpng library  
- signigicantly faster than ImageMagick  

### drawbacks
- only supports 3 channel (rgb) PNGs for now  

## installation
```bash
git clone https://github.com/savar95x/imgedit
cd imgedit
./compile
```

## usage
considering you're in the root of this repo
```bash
bin/imgedit OPTION=ARGs INPUT OUTPUT
```
Example usage
```bash
bin/imgedit --grayscale=0.9 imgs/in.png imgs/output.png
```

![demo](imgs/demo.gif)  

### note
It is intended to be used as a command line application, but there is a live demo on [streamlit](https://imgedit-savar.streamlit.app). Though I discourage testing it out because of compile time differences of the imgedit binary.  

## todo
- [ ] implement a function to parse rgb(a)
- [ ] add rotate, steganography, gamma, hue
- [ ] error handling (less arguements, invalid file input, out of range arguments)
- [ ] make a better front end (or make streamlit work)
- [ ] strtok_r?
- [ ] thread safety (lifetime of masks)
- [ ] rewrite using libpng eventually?
- [ ] hdr? also how does google image update the image real time (ans: applied on low res previews, gpu accelerated, start processing with what is visible on screen, output directly to screen (ram) instead of writing to disk)
- [ ] eventually add support for other common image types (jp(e)g, gif)
