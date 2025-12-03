# pour lancer ce fichier:
# Il faut être dans le dossier parent à src
# python -m src.fractales.courbe_du_dragon_new
from src.application import Application
import src.cpp_backend as backend

def courbe_du_dragon(profondeur : int, taille : int = 180):
    t = backend.get_turtle()
    av=taille/(profondeur+1)
    t.forward(av)
    for dir in calcul_liste_direction(profondeur):
        if dir == 90:
            t.right(90)
        else:
            t.left(90)
        t.forward(av)


def calcul_liste_direction(n):
    if n == 1:
        return [90]
    else:
        lst_dir = calcul_liste_direction(n-1)
        lst_dir_inv = lst_dir[::-1]
        lst_dir.append(90)
        for dir in lst_dir_inv:
            lst_dir.append(-dir)
        return lst_dir

if __name__ == "__main__":
    r = Application()
    t = backend.get_turtle()

    t.up()
    t.goto(0, 0)
    t.down()

    courbe_du_dragon(20)

    r.run()
    # print(calcul_liste_direction(1))
    # print(calcul_liste_direction(2))
    # print(calcul_liste_direction(3))
    # print(calcul_liste_direction(4))