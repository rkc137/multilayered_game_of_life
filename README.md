# Multilayered game of life
- [What?](#what-is-this)
- [How it works?](#how-it-works)
## what is this?
This is a version of [Conway's game of life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) that works not with one, but multiple layers of grids to determine cell state and give you ability to change the rule that the universe will depend on.

Some highliths: [demo video](https://youtu.be/gR2llpU1iEQ)

## how it works?
There is one grid that represents the present time, and one or more layers that store previous states of the grid to calculate the new one.
List of rules contains in `rules.hpp` where 2 arrays of rules, Extraverts - dont depend on their past, and Itroverts - depend on their past.
![scheme](screenshots\example.png)
you can change some of configuration values in runtime:
![conf](screenshots\configurations.png)