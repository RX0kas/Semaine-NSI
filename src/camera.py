import time
import warnings
import numpy as np
import math
from typing import Any
from glfw import get_window_size

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
        
        # TODO: Remove
        self.use_old_cam = True

        # Store window dimensions for coordinate conversion
        from src.window import Window
        self.__glfw_handle = Window.instance().getWindow()

        # Facteurs de vitesse
        self.zoom_speed = 8.0  # vitesse de convergence du zoom
        self.move_speed = 15  # vitesse de déplacement en unités/s
        self.smooth_damping = 0.2  # amortissement général

        self._last_mouse_pos = None
        self._last_time = time.time()

        if Camera.__instance is not None:
            raise PermissionError("Camera ne peux pas avoir plus d'une instance")
        Camera.__instance = self

    def screen_to_world(self, mouse_x, mouse_y)->np.ndarray:
        """Convertit une position écran (pixels) en coordonnées monde."""
        # Normalize mouse coordinates to [-1, 1] range
        size = get_window_size(self.__glfw_handle)
        nx = (mouse_x / size[0]) * 2.0 - 1.0
        ny = 1.0 - (mouse_y / size[1]) * 2.0

        # Apply inverse of view transformation
        world_x = (nx / self.zoom) + self.x
        world_y = (ny / self.zoom) + self.y

        return np.array([world_x, world_y], dtype=np.float32)

    # Mouvement de camera:
    def on_scroll(self, xoffset, yoffset, mouse_pos, window_size):
        """Change la cible de zoom en fonction de la molette"""
        mouse_x, mouse_y = mouse_pos
        w, h = window_size

        # Get mouse position in world coordinates before zoom
        world_before = self.screen_to_world(mouse_x, mouse_y)

        # Calculate zoom factor
        zoom_factor = 1.1 ** yoffset
        self.target_zoom = max(self.target_zoom * zoom_factor, 0.0000005)

        # Store mouse position for smooth zoom adjustment
        self._last_mouse_pos = (mouse_x, mouse_y, world_before)

    def update(self):
        """Appelée à chaque frame avec interpolation dépendante du temps"""
        now = time.time()
        dt = now - self._last_time
        self._last_time = now

        # Interpolation exponentielle du zoom vers sa cible
        if abs(self.target_zoom - self.zoom) > 1e-5: # Si la difference entre le zoom reel et le zoom voulu
            if self.use_old_cam:
                factor = 1.0 - math.exp(-self.zoom_speed * dt)
                old_zoom = self.zoom
                self.zoom += (self.target_zoom - self.zoom) * factor

                # Ajustement du centre de zoom (pour rester centré sur la souris)
                if self._last_mouse_pos:
                    mouse_x, mouse_y, world_before = self._last_mouse_pos

                    # Get mouse position in world coordinates after zoom
                    world_after = self.screen_to_world(mouse_x, mouse_y)

                    # Adjust camera position to keep the point under mouse fixed
                    self.x += world_before[0] - world_after[0]
                    self.y += world_before[1] - world_after[1]
                return
        
            factor = 1.0 - math.exp(-self.zoom_speed * dt)
            self.zoom += (self.target_zoom - self.zoom) * factor
            

    def on_drag(self, dx, dy, window_size, dt):
        """Déplacement avec la souris (clic milieu)"""
        w, h = window_size
        
        # Move camera in world coordinates
        self.x -= (dx / w) * 2.0 / self.zoom * self.move_speed * dt
        self.y += (dy / h) * 2.0 / self.zoom * self.move_speed * dt


    @staticmethod
    def instance() -> "Camera":
        return Camera.__instance # type: ignore