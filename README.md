# Asciimation
An ASCII animation interpreter for the terminal.  

![asciimation plane example](https://raw.githubusercontent.com/octobanana/asciimation/master/assets/asciimation.gif)

Asciimation is like the flipbooks where one draws a single frame per page, and flips through it to create an animation.  

![asciimation simple example](https://raw.githubusercontent.com/octobanana/asciimation/master/assets/octo.gif)

The above asciimation is created from the following file contents:  

    x:10        # minimum terminal width
    y:1         # minimum terminal height
    BEGIN       # keyword to seperate headers from frames
    octobanana  # the first frame
    END         # keyword to seperate frames
    OCTOBANANA  # the second frame

The interpreter reads from a plain text file. The start of the file contains the headers. The 'x' header is the minimum width of the terminal needed to properly display the animation, and 'y' is the height. The headers are optional, but the `BEGIN` line must be present. `BEGIN` tells the interpreter where to begin parsing the frames. Each frame is seperated by a newline with a special delimiter on it. The default delimiter is the word `END`.  

While the above example uses a single line per frame, a frame is interpreted as anything inbetween the seperators.  

See the examples folder for some ideas!  

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

## Future Features
* layering multiple frames as one
* keyword for coloring frames
* keyword for modulating the delay between frames
