import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))

from window import Window
from OpenGL.GL import *
from turtleRenderer import *
from returtle import Turtle
from imguiRenderer import ImGuiRenderer
from ligne_koch import ligne_koch

def flocon_koch(profondeur : int, t):
    t.left(180)
    ligne_koch(profondeur,t,140)
    t.right(120)
    ligne_koch(profondeur,t,140)
    t.right(120)
    ligne_koch(profondeur,t,140)




if __name__ == "__main__":
    fenetre = Window(800,600,"Test fenetre")
    turtleRenderer = TurtleRenderer()
    imguiRenderer = ImGuiRenderer(fenetre.getWindow())
    t = Turtle()
    glClearColor(0.5,0.5,0.5,1) # Couleur du fond (temporaire)
    
    
    t.up()
    t.goto(70, -50)
    t.down()

    flocon_koch(7,t)


    while not fenetre.devrait_fermer():
        imguiRenderer.newFrame()
        glClear(GL_COLOR_BUFFER_BIT)
        turtleRenderer.render(t)

        imguiRenderer.endFrame()
        fenetre.swapbuffer()


    turtleRenderer.cleanup()
    fenetre.supprimer()