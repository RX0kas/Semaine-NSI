import time
import warnings
import numpy as np
import math
from typing import Any


class Camera:
    """
    Pour l'instance contient juste les données comme la position et le zoom
    Car la logique est ecrite sur le shader
    """
    __instance = None

    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.zoom = 1.0
        self.target_zoom = 1.0

        # Store window dimensions for coordinate conversion
        self.window_width = 1
        self.window_height = 1

        # Facteurs de vitesse
        self.zoom_speed = 8.0  # vitesse de convergence du zoom
        self.move_speed = 15  # vitesse de déplacement en unités/s
        self.smooth_damping = 0.2  # amortissement général

        self.__view_matrix = None
        self.__last_mouse_pos = None
        self._last_time = time.time()

        self.__compute_view_matrix()

        if Camera.__instance is not None:
            raise PermissionError("Camera ne peux pas avoir plus d'une instance")
        Camera.__instance = self

    def set_window_size(self, width, height):
        """Set window dimensions for coordinate conversion"""
        self.window_width = width
        self.window_height = height

    def screen_to_world(self, mouse_x, mouse_y):
        """Convertit une position écran (pixels) en coordonnées monde."""
        # Normalize mouse coordinates to [-1, 1] range
        nx = (mouse_x / self.window_width) * 2.0 - 1.0
        ny = 1.0 - (mouse_y / self.window_height) * 2.0

        # Apply inverse of view transformation
        world_x = (nx / self.zoom) + self.x
        world_y = (ny / self.zoom) + self.y

        return np.array([world_x, world_y], dtype=np.float32)

    # Mouvement de camera:
    def on_scroll(self, xoffset, yoffset, mouse_pos, window_size):
        """Change la cible de zoom en fonction de la molette"""
        mouse_x, mouse_y = mouse_pos
        w, h = window_size

        # Store window size
        self.set_window_size(w, h)

        # Get mouse position in world coordinates before zoom
        world_before = self.screen_to_world(mouse_x, mouse_y)

        # Calculate zoom factor
        zoom_factor = 1.1 ** yoffset
        self.target_zoom = max(self.target_zoom * zoom_factor, 0.0000005)

        # Store mouse position for smooth zoom adjustment
        self.__last_mouse_pos = (mouse_x, mouse_y, world_before)

    def update(self):
        """Appelée à chaque frame avec interpolation dépendante du temps"""
        now = time.time()
        dt = now - self._last_time
        self._last_time = now

        # Interpolation exponentielle du zoom vers sa cible
        if abs(self.target_zoom - self.zoom) > 1e-5:
            factor = 1.0 - math.exp(-self.zoom_speed * dt)
            old_zoom = self.zoom
            self.zoom += (self.target_zoom - self.zoom) * factor

            # Ajustement du centre de zoom (pour rester centré sur la souris)
            if self.__last_mouse_pos:
                mouse_x, mouse_y, world_before = self.__last_mouse_pos

                # Get mouse position in world coordinates after zoom
                world_after = self.screen_to_world(mouse_x, mouse_y)

                # Adjust camera position to keep the point under mouse fixed
                self.x += world_before[0] - world_after[0]
                self.y += world_before[1] - world_after[1]

        self.__compute_view_matrix()

    def on_drag(self, dx, dy, window_size, dt):
        """Déplacement fluide avec la souris (clic milieu)"""
        w, h = window_size
        # Store window size
        self.set_window_size(w, h)

        # Move camera in world coordinates
        self.x -= (dx / w) * 2.0 / self.zoom * self.move_speed * dt
        self.y += (dy / h) * 2.0 / self.zoom * self.move_speed * dt
        self.__compute_view_matrix()

    def __compute_view_matrix(self):
        """
        Correct view matrix for 2D camera:
        [ zoom   0     -zoom * x ]
        [ 0      zoom  -zoom * y ]
        [ 0      0      1        ]

        This transforms world coordinates to view coordinates
        """
        self.__view_matrix = np.matrix([
            [self.zoom, 0, -self.zoom * self.x],
            [0, self.zoom, -self.zoom * self.y],
            [0, 0, 1]
        ], dtype=np.float32)

    def get_view_matrix(self) -> np.matrix[tuple[int, int], Any]:
        if self.__view_matrix is None:
            warnings.warn("View matrix not computed")
            self.__compute_view_matrix()
        return self.__view_matrix

    @staticmethod
    def instance():
        return Camera.__instance