# DO NOT deliver 

- .vscode
- build
- docs
- .git
- readme.md
- makefile
- .gitignore



# How to run program

```bash
    cd build
    cmake ..
    make
    ./polyg input.json output.json
```

# Algorithm

- Input: points and edges
- Output: non obtuse triangles

| Step | Algorithm                                            | Result                          |
| ---- | ---------------------------------------------------- | ------------------------------- |
| 1    | add points                                           | just points                     |
| 2    | add edges                                            | graph                           |
| 3    | add Delanau                                          | obtuse and non obtuse triangles |
| 4a   | for each triangle, that is obtuse, try edge flip     | obtuse and non obtuse triangles |
| 4b   | for each triangle, that is obtuse, add steiner point | obtuse and non obtuse triangles |
| 5    | Repeat step 4 until no obtuse exist                  | non obtuse triangles            |

# How to run program

Ερωτήσεις:

```
Δίνεται Α(x1,y1), B(x2,y2), C(x3,y3):

Ζητούμενο: Γράψτε κώδικα σε CGAL που να βρίσκει αν το πολύγωνο είναι αμβλυγώνιο.
```

```
Δίνεται Α(x1,y1), B(x2,y2), C(x3,y3):

Βρες που θα τοποθετηθεί το steiner point (βλέπε e-class)
```
