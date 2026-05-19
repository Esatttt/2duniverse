#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <ncurses.h>
#include <math.h>
#include <time.h>

#define RATE 200.00
#define MAXOBJECTS 10

typedef struct{
    double mass;

    //POSITION
    double x;
    double y;

    //VELOCITY
    double vx;
    double vy;
    
    //DIMENSION
    double xl;
    double yl;
} freeobject;

typedef struct{
    //POSITON
    double x,y;
    //DIMENSION
    double xl,yl;
} staticobject;

typedef struct{
    double friction;
    double g;
} enviroment;

void force(freeobject *object, double xf, double yf){
    double momentx = object->mass * object->vx;
    double momenty = object->mass * object->vy;

    //Calculating velocity change with the momentum change, impulse theory:
    double xpush = (1.0 / RATE) * xf;
    double ypush = (1.0 / RATE) * yf;

    momentx += xpush, momenty += ypush;

    //Updating the last velocity:
    object->vx = momentx / object->mass;
    object->vy = momenty / object->mass;
}

void friction(freeobject *object, enviroment *env){
            double friction = object->mass *  env->g * env->friction;
            double frictionx = 0, frictiony = 0;

            //FOR X AXIS
            if(object->vx > 0){
                frictionx = -friction;
            } else if(object->vx < 0){
                frictionx = friction;
            }else{frictionx = 0;}

            //FOR Y AXIS
            if(object->vy > 0){
                frictiony = -friction;
            } else if(object->vy < 0){
                frictiony = friction;
            }else{frictiony = 0;}

            double oldvx = object->vx, oldvy = object->vy;

            //APPLY
            force(object, frictionx, frictiony);

            if(oldvx * object->vx < 0){object->vx = 0.0;};
            if(oldvy * object->vy < 0){object->vy = 0.0;};
}

void getborders(freeobject object, double *left, double *right, double *up, double *down){
    *left = object.x - object.xl/2;
    *right = object.x + object.xl/2;
    *up = object.y + object.yl/2;
    *down = object.y - object.yl/2;
}

void elastic_collission(freeobject *o1, freeobject *o2){
    double o1left, o1right, o1up, o1down;
    double o2left, o2right, o2up, o2down;
    getborders(*o1, &o1left, &o1right, &o1up, &o1down);
    getborders(*o2, &o2left, &o2right, &o2up, &o2down);

    //Real life had infinite moments
    //We can't simulate it in computers
    //Instead of infinite, this physic core had limited rate of moment
    //So there is an delay in collisions mostly

    //Program will decide the collision should be x axis or y axis in this way
    //Make the collısıon axis which is objects collapsed their axis borders MOST

    double xgap = 0;
    double ygap = 0;
    double xvelo = 0;
    double yvelo = 0;

    if(o1->x < o2->x){ //LEFT IS O1
        xgap = o1right - o2left;
        xvelo = o1->vx - o2->vx;
    } else {           //LEFT IS O2
        xgap = o2right - o1left;
        xvelo = o2->vx - o1->vx;
    }

    if(o1->y < o2->y){ //UP IS O2
        ygap = o1up - o2down;
        yvelo = o2->vy - o1->vy;
    } else {           //UP IS O1
        ygap = o2up - o1down;
        yvelo = o1->vy - o2->vy;
    }

    double totalmass = o1->mass + o2->mass;
    double temp;

    if(xvelo == 0 || yvelo == 0){return;} //SAFETY
    
if(ygap > 0 && xgap > 0){
    if(ygap/yvelo > xgap/xvelo){
        temp = o1->vy;
        o1->vy = ((o1->mass - o2->mass) * o1->vy + 2 * o2->mass * o2->vy)/totalmass;
        o2->vy = ((o2->mass - o1->mass) * o2->vy + 2 * o1->mass * temp)/totalmass;
    } else if(ygap/yvelo < xgap/xvelo){
        temp = o1->vx;
        o1->vx = ((o1->mass - o2->mass) * o1->vx + 2 * o2->mass * o2->vx)/totalmass;
        o2->vx = ((o2->mass - o1->mass) * o2->vx + 2 * o1->mass * temp)/totalmass;
    } else {
        temp = o1->vy;
        o1->vy = ((o1->mass - o2->mass) * o1->vy + 2 * o2->mass * o2->vy)/totalmass;
        o2->vy = ((o2->mass - o1->mass) * o2->vy + 2 * o1->mass * temp)/totalmass;
        temp = o1->vx;
        o1->vx = ((o1->mass - o2->mass) * o1->vx + 2 * o2->mass * o2->vx)/totalmass;
        o2->vx = ((o2->mass - o1->mass) * o2->vx + 2 * o1->mass * temp)/totalmass;
     }
  } return;
}

void speedmv(freeobject *o){
    o->x += o->vx / RATE;
    o->y += o->vy / RATE;
}

void borderbounce(freeobject *o){
    int x =  COLS, y = LINES;
    double up,down,left,right;
    getborders(*o, &left, &right, &up, &down);

    if(right > x){
        o->vx = -o->vx;
        o->x -= (right - x);
    }
    if(left < 0){
        o->vx = -o->vx;
        o->x += (0 - left);
    }
    if(up > y){
        o->vy = -o->vy;
        o->y -= (up - y);
    }
    if(down < 0){
        o->vy = -o->vy;
        o->y += (0 - down);
    }
}

double gettime_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

void drawfreeobject(freeobject obj){
    //This function should be called when initscr is active

    int minx = obj.x - obj.xl/2, miny = obj.y - obj.yl/2, maxx = obj.x + obj.xl/2, maxy = obj.y + obj.yl/2;

    if(miny > LINES || miny < 0 || minx > COLS || maxy < 0){return;}

    for(int x = minx; x < maxx; x ++){
        for(int y = miny; y < maxy; y++){
  
            mvaddch(LINES - y,x,'*');
        }   
    }
    return;
}

void manuali(freeobject* objects, enviroment* env, int* size){
    *size = 11;

    while(*size > MAXOBJECTS){
    printf("\nHow much object do you want to create? (MAX is 10):\n");
    scanf("%d", size);
    }

    initscr();
    int x =  COLS, y = LINES;
    endwin();

    printf("X cordinates -> (0 to %d)\n", x);
    printf("Y corditanes -> (0 to %d)\n", y);

 
    for(int a = 0; a < *size; a ++){
        printf("\nObject %d\n\n", a);

        printf("Mass:\n");
        scanf("%lf", &objects[a].mass);

        printf("X cordinate:\n");
        scanf("%lf", &objects[a].x);

        printf("Y cordinate:\n");
        scanf("%lf", &objects[a].y);

        printf("X dimension length:\n");
        scanf("%lf", &objects[a].xl);

        printf("Y dimesion length:\n");
        scanf("%lf", &objects[a].yl);

        printf("Speed's X compenent:\n");
        scanf("%lf", &objects[a].vx);

        printf("Speed's Y compenent:\n");
        scanf("%lf", &objects[a].vy);
    }

    printf("Enviroment's friction coefficent: \n");
    scanf("%lf", &env->friction);
    printf("Gravity: \n");
    scanf("%lf", &env->g);

    printf("All variables initalized...\n");

}

void randomi(freeobject* objects, enviroment* env, int* size){
    //CHECKING WINDOW SIZES
    initscr();
    int x =  COLS, y = LINES;
    endwin();
    srand(time(NULL));
    env->friction = 0.3;
    env->g = 9.8;

    //RANDOMISING   
    *size = (rand() % (MAXOBJECTS - 1)) + 1;
    for(int i = 0; i < *size; i++){
        objects[i].x = (rand() % x);
        objects[i].y = (rand() % y);
        objects[i].vx = (rand() % 240) - 120;
        objects[i].vy = (rand() % 80) - 40;
        objects[i].xl = (rand() % 5) + 3;
        objects[i].yl = (rand() % 2) + 2;
        objects[i].mass = objects[i].xl*objects[i].yl;
    }
}

int main(){

    printf("Welcome to my 2D simple physics simulation!\n");
    printf("-------------------------------------------\n");
    printf(" _____          _   \n");
    printf("|  ___|        | |  \n");
    printf("| |__ ___  __ _| |_ \n");
    printf("|  __/ __|/ _` | __|\n");
    printf("| |__\\__ \\ (_| | |_ \n");
    printf("\\____/___/\\__,_|\\__|\n\n\n\n");

    int choice = 0;
    int programrunning = 1;
    freeobject objects[MAXOBJECTS];
    enviroment env;
    int size;
    double timelast;

    printf("'1' for manual initalization\n'2' for random initialization\n'3' for quit\n ");
    while(programrunning == 1){
    int choice = 0;
    while(choice != 1 && choice != 2 && choice != 3){
    printf("\nEnter choice: ");
    scanf("%d", &choice);
    }

    if(choice == 1){
    manuali(objects, &env, &size);
    } else if(choice == 2){
        randomi(objects, &env, &size);
    } else if(choice == 3){
        return 0;
    }

    int resume = 1;
    double framelength = (1000.0 / RATE);
    //FRAME
    initscr();
    curs_set(0);

    while(resume == 1){
        clear();
        int a = 0, b = 0;
        timelast = gettime_ms();

        resume = 0;
        //CHECKING every object
        while(a < size){            

            //COLLUSION 
            while(b < size){

                if(a == b){
                    b++; 
                    continue;}

                elastic_collission(&objects[a], &objects[b]);
                b++;
            }

            //FRICTION
            friction(&objects[a], &env);

            //SPEED
            speedmv(&objects[a]);   
            borderbounce(&objects[a]);
            drawfreeobject(objects[a]);

            if(objects[a].vx*objects[a].vx > 1 || objects[a].vy*objects[a].vy > 0){
                resume = 1; //To stopping the loop if the objects reached 0 speed..
            
            }
            a++;
            b = a + 1;
        
        }
        refresh();  
        usleep((framelength - (gettime_ms() - timelast))*1000);
    }   
        endwin();

}
}
