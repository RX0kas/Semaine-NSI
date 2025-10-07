import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))

from window import Window
from OpenGL.GL import *
from turtleRenderer import *
from returtle import Turtle
from imguiRenderer import ImGuiRenderer

def ligne_koch(profondeur : int,t, taille : int = 180):
    if profondeur==0:
        t.forward(taille)
    else:
        ligne_koch(profondeur-1,t, taille/3)
        t.left(60)
        ligne_koch(profondeur-1,t, taille/3)
        t.right(120)
        ligne_koch(profondeur-1,t, taille/3)
        t.left(60)
        ligne_koch(profondeur-1,t, taille/3)


if __name__ == "__main__":
    fenetre = Window(800,600,"Test fenetre")
    # juste le temps que je le mettes a un meilleur endroit
    # c'est pour afficher la tortue
    turtleRenderer = TurtleRenderer()
    imguiRenderer = ImGuiRenderer(fenetre.getWindow()) # ImGui est la librarie que je vais utiliser pour debugger les shaders car c'est facile a mettre en place
    # attention c'est pas les memes distances que la vrai turtle ;)
    # Ce n'est pas les pixels que tu dois donner mais les pourcentages de l'écran
    # car on ne pourras pas prevoir la taille de la fenetre
    t = Turtle()
    
    glClearColor(0.5,0.5,0.5,1) # Couleur du fond (temporaire)




    t.up()
    t.goto(-90, -90)
    t.down()

    ligne_koch(5,t)

    while not fenetre.devrait_fermer():
        imguiRenderer.newFrame()
        glClear(GL_COLOR_BUFFER_BIT)
        turtleRenderer.render(t)

        imguiRenderer.endFrame()
        fenetre.swapbuffer()


    turtleRenderer.cleanup()
    fenetre.supprimer()