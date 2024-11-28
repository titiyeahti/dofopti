# dofopti
Mathematical optimisation applied to the french game dofus

# Required libraries to make

The project is using 3 c libraries outside of the standard library.
- `glpk.h` is the mathematical solver used to find the best stuff;
- `sqlite3.h` provides an api to the sqlite sgdb;
- `json-c.h` is used to generate a json at some point.

In order to install these libs on ubuntu or debian you can do run :
```
sudo apt install libglpk-dev
sudo apt install libsqlite3-dev
sudo apt install libjson-c-dev
```
