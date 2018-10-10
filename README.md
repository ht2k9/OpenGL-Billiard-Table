# OpenGL Billiard Table

The game is developed in OpenGL, Visual Studio, using C.

To use it, you must have OpenGL glut libraries, it is well documented, easy to understand code.

Animation, physics, lightning and movements are all done with OpenGL and C.

### Note:
_The final version is lost, this code is little wet, has couple of TODOs and the collision is slightly off._

## The Actual Project Assignment _By proffesor A_

### Drawing

You must build a 3D virtual world of vector graphics in an openGL that contains a floor on which a table is placed.
The table stands on four legs and under the feet there is another floor.
- [x] The table has four-sided borders, as shown in a green painting.
- [x] On the table there are 9 balls of different colors.
- [x] The table has three holes as shown in the painting in a different color. You can use a round surface.
- [x] There are red barriers on the table.

![untitled](https://i.imgur.com/YAZMtA1.jpg)

### The Game

- [x] The balls will be numbered from 9-1, you must choose keyboard buttons that allow you to select a ball for the game.
- [x] Selecting the ball will color it in white. Selecting another ball will return the original to its previous color.
- [x] With a keyboard you can move automatically after a click (buttons of your choosing), right, left, forward and backward.
- [ ] Select a button that also makes circular motion to the selected ball, choose a direction for circular motion.
- [x] A ball that hits the red hole leaves the game and drops 10 points. A bullet that hits the black hole adds 10 points and returns the ball to the beginning of the table back.
- [x] The bullets can not pass through the red roadblocks, and hitting a checkpoint takes 2 points.
- [x] Make sure there is a collision between the balls so that it moves accordingly (hit ball and hit ball).
- [ ] The game ends after accumulating 11 points (PS: I don't remember).


### Additional Settings

- [x] Set lighting on the table from above.
- [x] Text your score up. And an end notification if the game ends.
- [x] Make sure that the buttons are used to turn the table sideways and up and down.
- [x] The radius of the balls and the table width of your choice.

### Bonus
- [x] _For those who add additional options such as adding speed to balls or slowing down and more_
