# Asciimation
An ASCII animation interpreter for the terminal.  

![asciimation example](https://raw.githubusercontent.com/octobanana/asciimation/master/assets/octo.gif)

Asciimation is like the flipbooks where one draws a single frame per page, and flips through it to create an animation.  
The interpreter reads a file filled with ascii animation frames. Each frame is seperated by a newline with a special delimiter on it. The default delimiter is the word `END`. It then outputs each frame with a configurable time delay to stdout, clearing the screen between each frame. It defaults to playing the animation once, but can be set to loop.  

## Build
Environment:  
* tested on linux
* c++ 14 compiler
* cmake

Libraries:  
* my [parg](https://github.com/octobanana/parg) library, for parsing cli args, included as `./src/parg.hh`

The following shell commands will build the project:  
```bash
git clone <repo_name>
cd <repo_name>
./build.sh -r
```
To build the debug version, run the build script without the -r flag.  

## Install
The following shell commands will install the project:  
```bash
./install.sh -r
```
