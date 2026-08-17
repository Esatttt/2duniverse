# 2duniverse
2D physics core in your terminal

<img width="800" height="471" alt="cikti" src="https://github.com/user-attachments/assets/12c85d42-c3ca-4311-9baa-e4ad619a98f6" />


Calculating elastic collisions with impulse theory.
Calculating friction's effect on speed with impulse theory.
Changeable RATE system. Adjust it for more precise results.

## Compiling:
Linux: gcc main.c -o 2duniverse -lncurses

MacOS: gcc main.c -o 2duniverse -lncurses -lm

Windows is not supported.

## Controls
Close it with ctrl+c.

With manual initialization, you can try yourself with custom values. 
With random initialization you can create yourself a physics experiment quickly.
Have fun.

## Docker
You can easily try my project without needing Linux or installing the ncurses library with Docker.

To compile Docker Image: docker build -t 2duniverse.

To start container from the Image: docker run -it 2duniverse



