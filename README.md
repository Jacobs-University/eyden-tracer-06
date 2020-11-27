# Practical Assignment 6
**Dealine**: 3.12.2020

Please put your name here:  
**Name:** .......
## Problem 1
### Target Camera (Points 10 + 10)
So far we have used mostly the perspective cameras, which were defined as cameras' position, direction and field of view (fov). A camera defined like that is also called _free camera_. For many cases (_e.g._ for animation) it is more convinient to operate a _target camera_ instead. A target camera instead of the _direction_ vector has a _target_ point, _i.e._ the point of space where the camera looks at; thus it is defined with two points - position and direction (and same fov). The goal of the first problem is to prepare our camera for animation. Proceed as follows:
1. **Prepare the ```CCameraPerspective``` class for animation**
    * In ```CCameraPerspective``` class implement the following accessors: 
        * ```virtual void setPosition(const Vec3f& pos);```
        * ```Vec3f getPosition(void) const;```
        * ```virtual void setDirection(const Vec3f& dir);```
        * ```Vec3f getDirection(void) const;```
        * ```virtual void setAngle(float angle);```
        * ```float getAngle(void) const;```
   
   These methods will allow us to modify the camera after it was created and added to the scene.
2. **Implement the ```CCameraTarget``` class**
    * In the **CameraTarget.h** file implement the ```CCameraTarget``` class. It should be derived from the ```CCameraPerspective``` class and tt's constructor should have the following form: ```CCameraTarget(Size resolution, const Vec3f& pos, const Vec3f& target, const Vec3f& up, float angle)```, _i.e._ take parameter _target_ instead of parameter _pos_. Implement the above mentioned logic of target amera directly in initialization in constructor.
    * In ```CCameraTarget``` class implement the following accessors: 
        * ```virtual void setPosition(const Vec3f& pos) override;```
        * ```virtual void setTarget(const Vec3f& target);```
        * ```Vec3f getTarget(void) const;```
        
In **main.cpp** file substite your free perspective camera with the target camera and test your implementation.

## Problem 2
### Shearing Transform (Points xx)

## Problem 3
### Camera Animation (Points xx)

## Problem 4
### Geometry Animation (Points xx)


## Submission
Please submit the assignment by making a pull request.
**Important** : Please make sure that
- No _extra files_ are submitted (except those, which were mentioned in the assignment)
- The changes were made _only_ in those files where you were asked to write your code
- The Continiouse Integration system (appVeyor) can build the submitted code
- The rendered images are also submitted in the folder "renders" 
