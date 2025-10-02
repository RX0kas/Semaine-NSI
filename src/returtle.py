import math
import numpy as np

class Turtle:
    """
    Représente une "tortue" pour tracer des chemins, inspirée du langage Logo.

    La tortue a une position (x, y), un angle de direction et un état de stylo (en bas ou en haut).
    Lorsqu'elle se déplace avec le stylo en bas, elle trace un segment de ligne.
    Les coordonnées des segments sont stockées et utilisées par le renderer.
    """

    def __init__(self, x=0, y=0, angle=0):
        """
        Initialise la tortue à une position donnée.

        :param x: position initiale en X
        :param y: position initiale en Y
        :param angle: orientation initiale en degrés (0 = vers la droite)
        """
        self.x = x
        self.y = y
        self.angle = angle  # en degrés
        self.pen_down = True  # par défaut, la tortue trace
        self.vertices = []  # segments déjà terminés (liste de sommets)
        self.current_path = []  # segment en cours de tracé (non encore validé)
        if self.pen_down:
            self.current_path = [self.x, self.y]

    def forward(self, distance):
        """
        Fait avancer la tortue d'une certaine distance dans sa direction actuelle.
        Si le stylo est en bas, ajoute un segment à la trajectoire.

        :param distance: longueur du déplacement (pourcentages)
        """
        distance /= 100

        # Calcul de la nouvelle position en fonction de l'angle
        rad_angle = math.radians(self.angle)
        new_x = self.x + math.cos(rad_angle) * distance
        new_y = self.y + math.sin(rad_angle) * distance

        if self.pen_down:
            # Si on trace : ajouter un segment (de l'ancienne position à la nouvelle)
            self.current_path.extend([self.x, self.y, new_x, new_y])

        # Mise à jour de la position
        self.x = new_x
        self.y = new_y


    def backward(self, distance):
        """
        Fait reculer la tortue (équivalent à avancer avec une distance négative).
        :param distance longueur du déplacement (pourcentages)
        """
        distance /= 100

        self.forward(-distance)

    def right(self, angle):
        """
        Tourne la tortue vers la droite (angle négatif).

        :param angle: angle en degrés
        """
        self.angle -= angle

    def left(self, angle):
        """
        Tourne la tortue vers la gauche (angle positif).

        :param angle: angle en degrés
        """
        self.angle += angle

    def penup(self):
        """
        Relève le stylo : la tortue se déplace sans tracer.
        Le chemin en cours est validé et ajouté aux segments terminés.
        """
        if self.current_path:
            # On ajoute le chemin en cours aux segments validés
            self.vertices.extend(self.current_path)
            self.current_path = []
        self.pen_down = False

    def pendown(self):
        """
            Abaisse le stylo : la tortue recommence à tracer.
            Un nouveau chemin démarre à la position actuelle.
        """
        if not self.pen_down:
            self.current_path = [self.x, self.y]
            self.pen_down = True

    def get_vertices(self):
        """
        Retourne l'ensemble des sommets (anciens + en cours de tracé).
        Utilisé par le renderer pour dessiner le chemin.

        :return: tableau numpy de coordonnées (x, y) en float32
        """
        all_vertices = self.vertices.copy()
        if self.current_path:
            all_vertices.extend(self.current_path)
        return np.array(all_vertices, dtype=np.float32)

    def goto(self,x,y):
        """
        :param x: coordonnée gauche-droite (pourcentage)
        :param y: coordonnée haut-bas (pourcentage)
        """
        x /= 100
        y /= 100

        if self.pen_down:
            # Si on trace : ajouter un segment (de l'ancienne position à la nouvelle)
            self.current_path.extend([self.x, self.y, x, y])

        # Mise à jour de la position
        self.x = x
        self.y = y


    #############################################
    #   Meme fonction mais ecrites differaments #
    #############################################

    def fd(self,distance):
        self.forward(distance)

    def bk(self, distance):
        self.backward(distance)

    def back(self,distance):
        self.backward(distance)

    def rt(self,angle):
        self.right(angle)

    def up(self):
        self.penup()

    def down(self):
        self.pendown()