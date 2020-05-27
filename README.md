# _idyll_

### _an idyllic region in your computer's heap for fractals to coexist in complete peace and harmony_
_idyll_ is a fractal engine, designed for the purpose of rendering beautiful, stunning and completely unique images of iterative fractals. if that sounds complicated, [**_this is a nice visual summary_**](https://www.youtube.com/watch?v=TIDYJsa3Z1A).

## why?
finding wallpapers that I like ends being a waste of time every now and then, so I decided to _invest_ countless hours of my time developing a tool that solves this recurrent problem of mine in the coolest way possible.

## how?
since I first decided that I would do this, I've gone deep down the rabbit hole of rendering techniques, distance estimators, and all of that good stuff related to fractals and graphics.
If you're interested in any of this or wondering how does _idyll_ work, please consider taking a look at [**_this nice explanation_**](apple.com).

## installation
you can either download the binary/exe version or compile it yourself
### download
[download the binary here](soybin.itch.io/idyll)
### compile
just clone the package and make the makefile
```
git clone https://github.com/soybin/idyll
cd idyll
make
./idyll
```

## dependencies
Absolutely none.
_idyll_ is self-contained, written in c++, and, thanks to the [TinyPngOut](https://www.nayuki.io/page/tiny-png-output) library by Nayuki, _idyll_ doesn't rely on the huge, yet common, [libpng library](http://www.libpng.org/pub/png/libpng.html).
