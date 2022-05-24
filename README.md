# QtCalculator

## Build on Ubuntu
<em> Most likely works the same for other Debian-like distros </em>

1. Install all of the dependencies using ```sudo apt install qttools5-dev qtmultimedia5-dev libqt5svg5-dev 
libboost-dev libssl-dev libboost-system-dev libboost-filesystem-dev cmake g++```

### Compiling manually

1. Go into the project directory
2. Create a build folder and go into it (```mkdir build && cd build```)
3. Compile using CMake ``` cmake .. && make ```
