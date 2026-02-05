# Nonogram Solver (C++)

This solver reads a puzzle file from the `puzzles/` folder and prints the solved grid (or the current state if it can’t solve it).

## Pick which puzzle to run (puzzleName.txt)

1. Open `puzzleName.txt`
2. Put the puzzle filename on the **first line**, for example:

   0003.txt

3. Save the file.

On startup, the app reads the first line of `puzzleName.txt` and uses it as the puzzle file name.  
If `puzzleName.txt` is missing/empty, it falls back to `0003.txt`.

The puzzle is then loaded from:

puzzles/<puzzleName>

Example: `puzzles/0003.txt`

### Important note about file paths (read this if it “can’t find” the file)

`puzzleName.txt` is opened using a **relative path**, which means it’s resolved from the **working directory** (the folder you run the exe from), not from `app/app.cpp`.

So:
- If you run the program from the project root, the code should open `"puzzleName.txt"`.
- If your code opens `"../puzzleName.txt"`, then you must run the program from inside the `app/` folder (or it will look one folder above the project and fail).

## Build + run (Windows)

If you have MinGW/MSYS2 `g++` installed:

- Double-click `runSolver.bat`

That script builds everything and runs the solver from the project root.

## Troubleshooting

- **It reads the wrong puzzle:** make sure the first line of `puzzleName.txt` exactly matches a file inside `puzzles/` (including `.txt`).
- **It can’t open puzzleName.txt:** you’re running the exe from a different working directory than you think. Run it from the project root (or adjust the path in code).
- **It can’t read the puzzle file:** confirm `puzzles/<name>` exists and the filename matches exactly.
