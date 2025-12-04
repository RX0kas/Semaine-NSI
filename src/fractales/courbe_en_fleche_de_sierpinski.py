# pour lancer ce fichier:
# Il faut être dans le dossier parent à src
# python -m src.fractales.courbe_en_fleche_de_sierpinski_new
from src.application import Application
import src.cpp_backend as backend

def courbe_en_fleche_sierpinski(profondeur : int, taille : int = 180):
    t = backend.get_turtle()
    av=taille*((2/3)**profondeur)
    courbe_en_fleche_sierpinski_récur1(profondeur,av)
    t.forward(av)


def courbe_en_fleche_sierpinski_récur1(profondeur : int, taille):
    t = backend.get_turtle()
    if profondeur == 0:
        pass
    else:
        courbe_en_fleche_sierpinski_récur2(profondeur-1,taille)
        t.forward(taille)
        t.left(60)
        courbe_en_fleche_sierpinski_récur1(profondeur-1,taille)
        t.forward(taille)
        t.left(60)
        courbe_en_fleche_sierpinski_récur2(profondeur-1,taille)


def courbe_en_fleche_sierpinski_récur2(profondeur : int, taille):
    t = backend.get_turtle()
    if profondeur == 0:
        pass
    else:
        courbe_en_fleche_sierpinski_récur1(profondeur-1,taille)
        t.forward(taille)
        t.right(60)
        courbe_en_fleche_sierpinski_récur2(profondeur-1,taille)
        t.forward(taille)
        t.right(60)
        courbe_en_fleche_sierpinski_récur1(profondeur-1,taille)

    



if __name__ == "__main__":
    r = Application()
    t = backend.get_turtle()

    t.up()
    t.goto(0, 0)
    t.down()

    courbe_en_fleche_sierpinski(7)

    r.run()