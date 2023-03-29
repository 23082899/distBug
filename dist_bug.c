#include "eyebot.h"
#include "math.h"
#include <stdio.h>

#define GOAL_X 3000
#define GOAL_Y 3000
#define STEP 1000

double getRelativeGoalLocationAng(int x, int y){
    double rot = atan2(y, x) * 180/M_PI;
    return rot;
}

double getRelativeGoalLocationDis(int x, int y){
    double distance = sqrt(x*x+y*y);
    return distance;
}

bool checkAtPoint(double x, double y, int goalX, int goalY){
    float nx = goalX-x;
    float ny = goalY-y;
    if ((nx >=-50 && nx <= 50)&&(ny >=-50 && ny <= 50)){
        return true;
    } else{
        return false;
    }
}

bool dist_bug(x, y){
    while (true){
       

        int currentXPosition, currentYPosition, currentAngle;
        VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);

        if (checkAtPoint(currentXPosition, currentYPosition, x, y)){
            return true;
        }
        VWSetSpeed(100, 0);

        bool obstacleDetected = false;
        if (PSDGet(2)<100){
            obstacleDetected = true;
        }

        if (obstacleDetected){
            VWSetSpeed(0, 0);

            int hitX, hitY, hitAngle;
            VWGetPosition(&hitX, &hitY, &hitAngle);

            double hitPoint[2] = {currentXPosition, currentYPosition};

            double minimumDistance = INFINITY;
            // bool leftHitPoint = False;

            VWTurn(180, 60);
            VWWait();

            while (true){

                // boundary following
                // TODO: follow the boundary adjusting direction each time loop executes
                // remember to make this non blocking

                if (checkAtPoint(currentXPosition, currentYPosition, hitPoint[0], hitPoint[1])){
                    return false;
                }

                double distance = getRelativeGoalLocationDis(x, y);
                double angle = getRelativeGoalLocationAng(x, y);

                if (distance < minimumDistance){
                    minimumDistance = distance;
                }

                int angleToGoal = angle - currentAngle;

                int lidar_values[360];
                LIDARGet(lidar_values);

                double freeSpaceToGoal = lidar_values[angleToGoal];

                if ((distance = freeSpaceToGoal) <= minimumDistance - STEP){
                    break;
                }




            }

        }
    }
}

int main() {
    SIMSetRobot(0, 500, 500, 0, 0);
    LCDMenu("Play", "", "", "Break");
    while (1){
        int key = KEYRead();
        if (key == KEY1){
            printf("dist_bug(%i, %i);", GOAL_X, GOAL_Y);
            dist_bug(GOAL_X, GOAL_Y);
            break;
        } 


        
        if (key == KEY4){
            break;
        }

    }
    return EXIT_SUCCESS;
}