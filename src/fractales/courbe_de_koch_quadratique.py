# pour lancer ce fichier:
# Il faut être dans le dossier parent à src
# python -m src.fractales.courbe_de_koch_quadratique

from src.application import Application
import src.cpp_backend as backend

def courbe_de_koch_quadratique(profondeur : int, taille : int = 180):
    t = backend.get_turtle()

    if profondeur==0:
        t.forward(taille)

    else:
        courbe_de_koch_quadratique(profondeur-1,taille/3)
        t.left(90)
        courbe_de_koch_quadratique(profondeur-1,taille/3)
        t.right(90)
        courbe_de_koch_quadratique(profondeur-1,taille/3)
        t.right(90)
        courbe_de_koch_quadratique(profondeur-1,taille/3)
        t.left(90)
        courbe_de_koch_quadratique(profondeur-1,taille/3)



if __name__ == "__main__":
    r = Application()
    t = backend.get_turtle()

    t.up()
    t.goto(-90, -90)
    t.down()

    courbe_de_koch_quadratique(0)

    r.run()