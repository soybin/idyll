# _idyll_

### _an idyllic region in your computer's heap for fractals to coexist in complete peace and harmony_
_idyll_ is a fractal engine, designed for the purpose of rendering beautiful, stunning and completely unique images of iterative fractals. if that sounds complicated, [**_this is a nice visual summary_**](https://youtu.be/cFykbtJmg4A).

## features
* path-tracing used to compute global illumination
* raymarching used for fractal distance estimation
* cpu-based
* multi-threaded
* seed generation/saving/loading system
* configuration file

## system dependencies
none. _idyll_ doesn't rely on any system-level libraries. the whole program fits within a 100KB binary file and, thanks to the [TinyPngOut](https://www.nayuki.io/page/tiny-png-output) library by Nayuki, _idyll_ doesn't use the huge, yet common, [libpng library](http://www.libpng.org/pub/png/libpng.html).

## installation
you can either download the binary/exe version or compile the source code it yourself
### [download](https://soybin.itch.io/idyll)
### compile
just clone the repo and make
```
git clone https://github.com/soybin/idyll
cd idyll
make
./idyll
```

## usage
1. run idyll.
1. a configuration file named 'config.txt' will be created in the same directory.
1. the first time you run the program, the 'config .txt' file will have standard values.
1. you can change the values at the 'config.txt' file to fit your needs by changing the resolution, rendering quality and some other values detailed in the 'config.txt' file.
1. enjoy your fresh and unique fractals!

## seeds
seeds serve the purpose of storing the data of each generated fractal using idyll. for example, if you just rendered a fractal that you really like and you would like to render the very same fractal at a higher quality—or different resolution—you can easily accomplish this task:

when the program finishes running, you'll find a '.txt' file named 'seed' + "number of the rendered fractal" + '.txt'. for example, if you just rendered a fractal and the output file for it was 'render23.png', then the seed file for that fractal will be 'seed23.txt'. then, **just run the program from your preferred terminal and pass the directory of the desired seed file as the first argument**.

### unix
```
./idyll seed0.txt
```
### windows
```
idyll.exe seed0.txt
```
