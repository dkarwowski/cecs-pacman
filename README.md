# cecs-pacman

Pacman implemented in c with an entity component system

Mostly a test in seeing whether an entity-component system can work well in C.
Feels a bit like a hack job for some parts, but works fairly well. Parsing
config files would probably end up being the hardest part as the factories are
hard coded at the moment.

## build

To build, you just need to make sure you have the `SDL2` `SDL2_ttf` and
`SDl2_image` libraries installed. After that, you can just run `make all`
from the base directory and the proper files/folders should be auto-generated.
Simply run the `proto` executable from the `./bin` directory and the window
should pop up.
