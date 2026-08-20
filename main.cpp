#include<iostream>
#include<vector>
#include<fstream>
#include<math.h>

using namespace std;

struct Vector
{   
    double x = 0;
    double y = 0;

};


class Body{
    public: 
        double mass = 0;
        double inertia = 0;
        double dense = 0;
        Vector position = {0,0};
                Vector velocity = {0,0};
        Vector acceleration = {0,0};
        Vector force = {0,0};

        Vector initialVelocity = {0,0};


    Body(
        double m, double i, double d,
        Vector p, Vector v,
        Vector a, Vector f, Vector iv
    ) {
        mass = m;
        inertia = i;
        dense = d;
        position = p;
        velocity = v;
        acceleration = a;
        force = f;
        initialVelocity = iv;
    }
};

const Vector gravity = {0,-9.8}; 

void motion(Body& Ball, double& time, double& StartingHeight, double& acceleration, ofstream& outFile);
void makeCSV(Body Ball, double time, ofstream& outFile);

int main(){
    Body Ball(
        0.5, 5, 1,
        {0,0}, {0,0},
        {0,0},{0,0}, {0,0}
    );

    ofstream outFile("simulation.csv");
    outFile << "time,positionX,positionY,velocityX,velocityY\n";


    double time = 0;
    double StartingHeight = 100;
    double acceleration = 20;
    motion(Ball, time, StartingHeight, acceleration, outFile);

    outFile.close();

    return 0;
}

void motion(Body& Ball, double& time, double& Startingheight, double& acceleration, ofstream& outFile){
    double increment = 0.01;

    Ball.position.y = Startingheight;
    Ball.acceleration.x = acceleration;


    Ball.velocity.x = Ball.acceleration.x;

    for(double i = 0; i < 100; i = i + increment){

        double DragForceX = 0;
        double DragForceY = 0;

        //Uses the euler integration method to constantly apply negative acceleration (gravity) on the ball
        //Funnily enough since its being applied all the time it also handles the bouncing of the ball...
        //Since gravity is always pulling down. Neat!
        Ball.velocity.y += gravity.y * increment;
        Ball.position.y += Ball.velocity.y * increment;
        Ball.position.x += Ball.velocity.x * increment;

        //Calculates DragForce for vertical AND Horizontal motion
        DragForceX += -Ball.velocity.x*abs(Ball.velocity.x)*0.0093;
        DragForceY += -Ball.velocity.y*abs(Ball.velocity.y)*0.0093;

        //Applies a negative force equal to the dragforce again using the euler integration method
        Ball.velocity.y += (DragForceY/Ball.mass)*increment;
        Ball.velocity.x += (DragForceX/Ball.mass)*increment;

        if(Ball.position.y <= 0){
            Ball.position.y = 0;
            Ball.velocity.y = Ball.velocity.y*-0.85;
        }

        makeCSV(Ball, time, outFile);

        //Displays height and vertical velocity of ball (acceleration due to gravity)
        std::cout << "height: " << Ball.position.y << "\n";        
        std::cout << "Y velocity: " << Ball.velocity.y << "\n\n";

        //Displays horizontal position and velocity of ball (given initial nudge aka acceleration)
        std::cout << "X velocity: " << Ball.velocity.x << "\n";
        std::cout << "Position: " << Ball.position.x << "\n\n";
        time = time + increment;   
    }
}

void makeCSV(Body Ball, double time, ofstream& outFile){
    outFile << time << "," << Ball.position.x << "," << Ball.position.y 
        << "," << Ball.velocity.x << "," << Ball.velocity.y << '\n';
}