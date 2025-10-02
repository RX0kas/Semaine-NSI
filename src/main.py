from window import Window
from OpenGL.GL import *

from turtleRenderer import *
from returtle import Turtle


fenetre = Window(800,600,"Test fenetre")
# juste le temps que je le mettes a un meilleur endroit
# c'est pour afficher la tortue
turtleRenderer = TurtleRenderer()
# attention c'est pas les memes distances que la vrai turtle ;)
# Ce n'est pas les pixels que tu dois donner mais les pourcentages de l'écran
# car on ne pourras pas prevoir la taille de la fenetre
t = Turtle()

glClearColor(0.5,0.5,0.5,1) # Couleur du fond (temporaire)

def flocon_koch(profondeur : int):
    if profondeur==0:
        t.forward(5)
    else:
        flocon_koch(profondeur-1)
        t.left(45)
        flocon_koch(profondeur-1)
        t.right(90)
        flocon_koch(profondeur-1)
        t.left(45)
        flocon_koch(profondeur-1)


t.up()
t.goto(-50, -50)
t.down()

flocon_koch(2)

while not fenetre.devrait_fermer():
    glClear(GL_COLOR_BUFFER_BIT)

    turtleRenderer.render(t)

    fenetre.swapbuffer()


turtleRenderer.cleanup()
fenetre.supprimer()