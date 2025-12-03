# pour lancer ce fichier:
# Il faut être dans le dossier parent à src
# python -m src.fractales.courbe_carre_de_sierpinski_new
from src.application import Application
import src.cpp_backend as backend

def courbe_carre_sierpinski(profondeur : int, taille : int = 1800):
    t = backend.get_turtle()
    t.forward(taille/(profondeur+1))
    t.left(90)
    courbe_carre_sierpinski_récur(profondeur,taille/(profondeur+1))
    t.forward(taille/(profondeur+1))
    t.left(90)
    t.forward(taille/(profondeur+1))
    t.left(90)
    courbe_carre_sierpinski_récur(profondeur,taille/(profondeur+1))
    t.forward(taille/(profondeur+1))


def courbe_carre_sierpinski_récur(profondeur : int, taille):
    if profondeur == 0:
        t.forward(taille)
    else:
        courbe_carre_sierpinski_récur(profondeur-1,taille)
        t.forward(taille)
        t.right(90)
        t.forward(taille)
        t.left(90)
        t.forward(taille)
        t.right(90)
        courbe_carre_sierpinski_récur(profondeur-1,taille)
        t.forward(taille)
        t.left(90)
        t.forward(taille)
        t.left(90)
        courbe_carre_sierpinski_récur(profondeur-1,taille)
        t.forward(taille)
        t.right(90)
        t.forward(taille)
        t.left(90)
        t.forward(taille)
        t.right(90)
        courbe_carre_sierpinski_récur(profondeur-1,taille)



if __name__ == "__main__":
    r = Application()
    t = backend.get_turtle()

    t.up()
    t.goto(0, 0)
    t.down()

    courbe_carre_sierpinski(7)

    r.run()