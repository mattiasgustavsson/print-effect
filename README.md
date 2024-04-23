![build](https://github.com/mattiasgustavsson/print-effect/actions/workflows/main.yaml/badge.svg)

# print-effect 
Emulating old comic book print. Very much experimental WIP, but can be used to preprocess a CBZ into a version with an emulated print look.

Code is public domain, use for whatever, at your own risk etc. 

More information here: https://x.com/Mattias_G/status/1741115035944996900


## building

From an msvc command prompt, run:

    cl source\print-effect.c

Note: currently does not build on mac/linux because of missing implementation for function `set_app_dir`, which would be pretty straightforward to add, I just haven't got around to it yet. It is also possible to just make it an empty function and make sure you run the program with current path being where the `data` folder lives.

## examples

(more examples can be found here: https://mattiasgustavsson.com/img/print/)

![print-effect](example.png)

