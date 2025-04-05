// Constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const double GRAVITATIONAL_CONSTANT = 6.67430e-11;
const double TIME_STEP = 1; // Simulation time step in seconds
const double SCALE_FACTOR = 1e-9; // Scale for rendering orbital distances
const double MIN_SCALE_FACTOR = 1e-13; // Zoomed all the way out
const double MAX_SCALE_FACTOR = 1e-4; // Zoomed all the way in
const double ZOOM_SPEED = 1.2; // How quickly zoom changes per scroll