from window import Window
from OpenGL.GL import *

from turtleRenderer import *
from returtle import Turtle
from imguiRenderer import ImGuiRenderer


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
    imguiRenderer.newFrame()
    glClear(GL_COLOR_BUFFER_BIT)
    turtleRenderer.render(t)
    
    imguiRenderer.show_debug_window(t)

    imguiRenderer.endFrame()
    fenetre.swapbuffer()


turtleRenderer.cleanup()
fenetre.supprimer()