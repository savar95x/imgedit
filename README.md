# imgedit
A command line image editor.  
(Re)written using the png++ library (earlier i used the stb_image header which was noticeably slower).  

## features
- implemented concurrency using std::threads, a 64% performance boost compared to sequential execution  
- uses the efficient png++ library which uses libpng  
- signigicantly faster than ImageMagick  

### drawbacks
- only supports png for now  

## usage
considering you're in the root of this repo
```bash
bin/imgedit OPTION=ARG(S) INPUT OUTPUT
```
Example usage
```bash
bin/imgedit --grayscale=0.9 imgs/in.png imgs/output.png
```

![demo](imgs/demo.gif)  

### note
It is intended to be used as a command line application, but there is a live demo on [streamlit](https://imgedit-savar.streamlit.app). Though I discourage testing it out because of compile time differences of the imgedit binary.  

## todo
- [ ] add crop, rotate, steganography, gamma, sepia, hue etc
- [ ] implement concurrency in all the functions
- [ ] make a better front end (or make streamlit work)
- [ ] error handling (less arguements, invalid file input)
- [ ] strtok_r?
- [ ] thread safety (lifetime of masks)
