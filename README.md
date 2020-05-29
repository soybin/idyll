# _idyll_

### _an idyllic region in your computer's heap for fractals to coexist in complete peace and harmony_
_idyll_ is a fractal engine, designed for the purpose of rendering beautiful, stunning and completely unique images of iterative fractals. if that sounds complicated, [**_this is a nice visual summary_**](https://www.youtube.com/watch?v=TIDYJsa3Z1A).

## technical details
* path-tracing used to compute global illumination
* raymarching used for fractal distance estimation
* cpu-based
* multi-threaded
* random seed generation
* seed saving/loading system
* configuration file
* highly portable
* dependent-less
* lightweight

## installation
you can either download the binary/exe version or compile the source code it yourself

### download binary
[download the binary here](soybin.itch.io/idyll)
### compile
just clone the package and make the makefile
```
git clone https://github.com/soybin/idyll
cd idyll
make
./idyll
```

## seeds
seeds serve the purpose of storing the data from each fractal that you generate when using idyll. for example, if you just rendered a fractal that you really like and you would like to render the very same fractal at a higher quality—or different resolution—you can easily accomplish this task:

when the program finishes running, you'll find a '.txt' file named 'seed' + "number of the rendered fractal" + '.txt'. for example, if you just rendered a fractal and the output file for it was 'render23.png', then the seed file for that fractal will be 'seed23.txt'. then, **just run the program from your preferred terminal and pass the directory of the desired seed file as the first argument**.

### unix
```
./idyll seed0.txt
```
### windows
```
idyll.exe seed0.txt
```

## dependencies
Absolutely none.
_idyll_ is self-contained, written in c++, and, thanks to the [TinyPngOut](https://www.nayuki.io/page/tiny-png-output) library by Nayuki, _idyll_ doesn't rely on the huge, yet common, [libpng library](http://www.libpng.org/pub/png/libpng.html).
