import time
import math
from typing import Optional, Tuple
import numpy as np


class Camera:
    __instance = None

    def __init__(self) -> None:
        # Transform
        self.x: float = 0.0
        self.y: float = 0.0
        self.zoom: float = 1.0
        self.target_zoom: float = 1.0

        # Zoom limits
        self.min_zoom: float = 1e-4
        self.max_zoom: float = 1e3

        # Interpolation / responsiveness
        self.zoom_speed: float = 8.0  # higher = zoom reaches target faster
        self.zoom_interp: str = "exp"  # "exp" or "linear"
        self.zoom_wheel_base: float = 1.1  # base multiplier for one wheel step
        self.zoom_sensitivity: float = 1.0  # multiply wheel steps

        # How strongly the camera recenters to keep the mouse world point fixed.
        # 0.0 = never follow, 1.0 = follow so the world point stays fixed, >1.0 = over-follow
        self.zoom_follow_strength: float = 1.0
        # How fast the camera position moves to satisfy the follow target (higher = faster)
        self.follow_interp_speed: float = 40.0

        # If True, any manual input (pan/drag) will cancel an ongoing zoom-follow.
        # Set to False if you prefer panning to update the follow target instead of canceling it.
        self.clear_follow_on_input: bool = True

        # Panning
        self.pan_speed: float = 1.0  # multiplier for drag input
        self.pan_smoothing: float = 0.0  # 0 = instant, >0 = smooth with friction
        self._pan_velocity = np.array([0.0, 0.0], dtype=np.float64)

        # Position clamp to avoid runaway values
        self.enable_position_clamp: bool = True
        self.position_limit: float = 1e6  # clamp x and y to [-position_limit, position_limit]

        # Snap thresholds
        self.snap_zoom_threshold: float = 1e-5
        self.snap_position_threshold: float = 1e-6

        # Internal state
        # stored when a scroll event happens: (mouse_x, mouse_y, world_before, window_size)
        self._last_mouse_follow: Optional[Tuple[float, float, np.ndarray, Tuple[int, int]]] = None
        self._last_time: float = time.perf_counter()

        if Camera.__instance is not None:
            raise PermissionError("Camera ne peut pas avoir plus d'une instance")
        Camera.__instance = self

    # ------------------ Coordinate conversions ------------------
    def screen_to_world(self, mouse_x: float, mouse_y: float, window_size: Tuple[int, int]) -> np.ndarray:
        """Convertit des coordonnées écran (pixels) en coordonnées monde.

        window_size: (width, height)
        Coordinate system: normalized device coords x,y in [-1,1], y up.
        """
        w, h = window_size
        if w == 0 or h == 0:
            return np.array([self.x, self.y], dtype=np.float64)

        nx = (mouse_x / w) * 2.0 - 1.0
        ny = 1.0 - (mouse_y / h) * 2.0

        world_x = (nx / self.zoom) + self.x
        world_y = (ny / self.zoom) + self.y
        return np.array([world_x, world_y], dtype=np.float64)

    def world_to_screen(self, world_x: float, world_y: float, window_size: Tuple[int, int]) -> np.ndarray:
        w, h = window_size
        if w == 0 or h == 0:
            return np.array([w * 0.5, h * 0.5], dtype=np.float64)

        nx = (world_x - self.x) * self.zoom
        ny = (world_y - self.y) * self.zoom

        screen_x = (nx * 0.5 + 0.5) * w
        screen_y = (0.5 - ny * 0.5) * h
        return np.array([screen_x, screen_y], dtype=np.float64)

    # ------------------ Input handlers ------------------
    def on_scroll(self, xoffset: float, yoffset: float, mouse_pos: Tuple[float, float], window_size: Tuple[int, int]) -> None:
        """Appelé dans le callback de la molette.

        xoffset/yoffset: comme renvoyé par la plupart des librairies (yoffset vertical wheel steps)
        mouse_pos: position de la souris en pixels (x,y)
        window_size: (width, height)
        """
        mouse_x, mouse_y = mouse_pos

        # world under mouse before changing target zoom
        world_before = self.screen_to_world(mouse_x, mouse_y, window_size)

        step = yoffset * self.zoom_sensitivity
        factor = self.zoom_wheel_base ** step
        new_target = self.target_zoom * factor
        # clamp
        self.target_zoom = max(self.min_zoom, min(new_target, self.max_zoom))

        # store follow info to be used during update
        self._last_mouse_follow = (mouse_x, mouse_y, world_before, window_size)

    def on_drag(self, dx: float, dy: float, window_size: Tuple[int, int], dt: Optional[float] = None) -> None:
        """Appelé quand l'utilisateur drag (clic molette). dx/dy sont en pixels.

        If clear_follow_on_input is True, any ongoing zoom-follow is cancelled so manual panning
        is not overridden. If you prefer panning to adjust the follow target instead of cancelling,
        set clear_follow_on_input = False.
        """
        # cancel follow if user starts panning
        if self.clear_follow_on_input:
            self._last_mouse_follow = None

        # Convert pixel deltas to world deltas using current zoom
        w, h = window_size
        if w == 0 or h == 0:
            return

        ndx = (dx / w) * 2.0 / max(1e-12, self.zoom)
        ndy = (dy / h) * 2.0 / max(1e-12, self.zoom)

        # Sign: dragging right should move the camera left in world coords
        desired_move = np.array([-ndx * self.pan_speed, ndy * self.pan_speed], dtype=np.float64)

        if self.pan_smoothing <= 0.0:
            self.x += desired_move[0]
            self.y += desired_move[1]
        else:
            # apply to velocity so update() will integrate
            self._pan_velocity += desired_move

        if self.enable_position_clamp:
            self._clamp_position()

    # ------------------ Helpers ------------------
    def _clamp_position(self) -> None:
        if not self.enable_position_clamp:
            return
        lim = self.position_limit
        # avoid NaN/inf by checking finite
        if not math.isfinite(self.x) or abs(self.x) > lim:
            self.x = max(-lim, min(self.x if math.isfinite(self.x) else 0.0, lim))
        if not math.isfinite(self.y) or abs(self.y) > lim:
            self.y = max(-lim, min(self.y if math.isfinite(self.y) else 0.0, lim))

    # ------------------ Main update ------------------
    def update(self) -> None:
        """Appeler une fois par frame. Interpole le zoom et applique le follow.
        """
        now = time.perf_counter()
        dt = max(1e-6, now - self._last_time)
        self._last_time = now

        # ---- Zoom interpolation ----
        if abs(self.target_zoom - self.zoom) <= self.snap_zoom_threshold:
            self.zoom = self.target_zoom
        else:
            if self.zoom_interp == "linear":
                # linear step towards target with speed per second
                step = self.zoom_speed * dt
                t = min(1.0, step)
                self.zoom = self.zoom + (self.target_zoom - self.zoom) * t
            else:
                # exponential smoothing (stable for variable dt)
                factor = 1.0 - math.exp(-self.zoom_speed * dt)
                self.zoom += (self.target_zoom - self.zoom) * factor

        # ---- Apply follow centering if a scroll initiated it ----
        if self._last_mouse_follow is not None:
            mouse_x, mouse_y, world_before, window_size = self._last_mouse_follow

            # compute where the same screen pixel maps in world space after current zoom
            world_after = self.screen_to_world(mouse_x, mouse_y, window_size)

            # delta required so the world_before appears again under the mouse
            delta_world = world_before - world_after

            # target position shift
            desired_dx = float(delta_world[0]) * self.zoom_follow_strength
            desired_dy = float(delta_world[1]) * self.zoom_follow_strength

            if self.follow_interp_speed <= 0.0:
                # instant or direct apply
                self.x += desired_dx
                self.y += desired_dy
            else:
                # smooth move using exponential interpolation towards the desired correction
                # We treat the desired correction as a velocity impulse here. Another approach is
                # to compute a target camera pos and interpolate the camera pos itself.
                follow_factor = 1.0 - math.exp(-self.follow_interp_speed * dt)
                self.x += desired_dx * follow_factor
                self.y += desired_dy * follow_factor

            # If zoom is essentially at target and the remaining correction is tiny, clear follow
            if abs(self.target_zoom - self.zoom) < self.snap_zoom_threshold and (
                abs(desired_dx) < self.snap_position_threshold and abs(desired_dy) < self.snap_position_threshold
            ):
                self._last_mouse_follow = None

        # ---- Integrate pan velocity if smoothing used ----
        if self.pan_smoothing > 0.0:
            # velocity decay (friction)
            decay = math.exp(-self.pan_smoothing * dt)
            self._pan_velocity *= decay
            # integrate velocity
            self.x += self._pan_velocity[0] * dt
            self.y += self._pan_velocity[1] * dt

        # final clamp to avoid runaway
        if self.enable_position_clamp:
            self._clamp_position()

    # ------------------ Utility setters ------------------
    def set_zoom(self, zoom: float, center: Optional[Tuple[float, float]] = None, window_size: Optional[Tuple[int, int]] = None, instant: bool = False) -> None:
        """Set the zoom. If center provided, try to keep that screen point fixed.

        instant=True applies zoom immediately. Otherwise it sets target_zoom and follow behavior.
        """
        if window_size is None and center is not None:
            raise ValueError("window_size must be provided when specifying center")

        if center is not None:
            mx, my = center
            world_before = self.screen_to_world(mx, my, window_size)  # type: ignore
        else:
            world_before = None

        z = max(self.min_zoom, min(zoom, self.max_zoom))
        if instant:
            self.zoom = z
            self.target_zoom = z
            if world_before is not None:
                world_after = self.screen_to_world(mx, my, window_size)  # type: ignore
                self.x += float(world_before[0] - world_after[0])
                self.y += float(world_before[1] - world_after[1])
        else:
            self.target_zoom = z
            if world_before is not None:
                self._last_mouse_follow = (mx, my, world_before, window_size)  # type: ignore

    def set_position(self, x: float, y: float, instant: bool = True) -> None:
        if instant or self.follow_interp_speed <= 0.0:
            self.x = x
            self.y = y
        else:
            # weak smoothing to target position
            self.x += (x - self.x) * (1.0 - math.exp(-self.follow_interp_speed * (1.0 / 60.0)))
            self.y += (y - self.y) * (1.0 - math.exp(-self.follow_interp_speed * (1.0 / 60.0)))

        if self.enable_position_clamp:
            self._clamp_position()

    @staticmethod
    def instance() -> "Camera":
        return Camera.__instance  # type: ignore