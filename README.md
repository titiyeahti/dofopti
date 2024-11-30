# dofopti
Mathematical optimisation applied to the french game dofus

## Required libraries to make

The project is using 3 c libraries outside of the standard library.
- `glpk.h` is the mathematical solver used to find the best stuff;
- `sqlite3.h` provides an api to the sqlite sgdb;
- `json-c.h` is used to generate a json at some point.

In order to install these libs on ubuntu or debian you can run
the following commands :
```
sudo apt install libglpk-dev
sudo apt install libsqlite3-dev
sudo apt install libjson-c-dev
```

Once these libs installed simply run `make` inside the `src` folder.

## How to use the solver 

To input data to the solver you have to write an input file `example.input` and then run
```
./dofopti.out path/to/example.input
```

You may find some input examples inside the `inputfiles` folder.

## Input file description

### List of sections

An input file is divided in various sections with a precse syntax for each.
You can precise a section by writing `#SECTION_NAME` in your file.
Here is the list of valid section names :
- `LEVEL`, simple one, just print the level of your character.
- `MIN_CRIT`, used to specify if you actually want to take critical strike into account in the computation. Here you specify the minimum critical strike chance given by your stuff as an integer. The solver will assume that you have THIS amount of crit chance in your stuff when doing computation.
- `TARGETED_SLOTS`, the number of items you desire per slots. Often 6 for dofuses, 2 for ring and one for other slots. The section syntax is `slot_name uint` where the valid slots names are :
    +`amulet`, default 1;
    +`hat`, default 1;
    +`ring`, default 1;
    +`weapon`, default 1;
    +`shield`, default 1;
    +`belt`, default 1;
    +`back`, default 1;
    +`boots`, default 1;
    +`dofus`, default 6;
    +`prysmaradite`, default 1;
    +`pet`, default 1.
- `BASE_STATS` the additionnal stats of your character, mainly "forgemagie". Syntax is the following ; `stat_name int`.
- `DMG_LINES`, this section is here to help you optimize a damage round. Here you ave to give information over each damage line you want to take into account (ex: 4 lines for zoth warrior axe). Here is the syntax for a damage line : `elt_name line_crit_rate min_nocri max_nocri min_cri max_cri`. These are all integral values except for elt wich must be one of `air, eau, feu, terre, neutre`. You have to give each of the six required values even if your spell does not crit, in this case `crit_rate`must be `0`.
- `OBJECIVE`, used to give simple objectives coeffs to the solver. Better not to use if already used dmg lines. Syntax : `stat_name int`.
- `CONSTRAINTS`, the constraints your stuff must satisfy (e.g ap >= 11). Syntax : `stat_name (>=|<=|=) value`.
- `LOCK_ITEMS`, in this section you may force the solver to set certain items in or out from your stuff. Syntax : `(+|-)item_name_in_english_with_no_cap`.

### Stat names

### Example file 

```
#LEVEL
200
#MIN_CRIT
50
#TARGETED_SLOTS
amulet 1
hat 1
ring 2
weapon 1
shield 1
belt 1
back 1
boots 1
dofus 6
prysmaradite 1
pet 1
#BASE_STATS
cha 100
str 100
int 100
agi 100
sa 100
vit 100
pa 1
pm 1
#DMG_LINES
%decimation entaille x2
terre 15 29 32 35 38
terre 25 47 51 56 61
terre 25 47 51 56 61
#CONSTRAINTS
pa >= 12
pm >= 5
#LOCK_ITEMS
```

TODO
