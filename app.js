// Global variables
let running = false;
let animationFrameId = null;
let cellSize = 10; // Size of each cell in pixels
let lastUpdateTime = 0;
let updateInterval = 100; // Update every 100ms

// Canvas setup
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");

// Wait for the WASM module to load
Module.onRuntimeInitialized = async () => {
  // Wrap the exported C++ functions
  const initializeGrid = Module.cwrap("initializeGrid", null, []);
  const updateGrid = Module.cwrap("updateGrid", null, []);
  const getWidth = Module.cwrap("getWidth", "number", []);
  const getHeight = Module.cwrap("getHeight", "number", []);
  const getGridDataPtr = Module.cwrap("getGridData", "number", []);

  // Get the grid dimensions
  const width = getWidth();
  const height = getHeight();

  // Initialize the grid with random cells
  initializeGrid();

  // Function to render the grid
  function render() {
    const gridDataPtr = getGridDataPtr();

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for (let y = 0; y < height; y++) {
      for (let x = 0; x < width; x++) {
        // Access the grid data through the WASM memory
        const index = y * width + x;
        const alive = Module.HEAPU8[gridDataPtr + index] !== 0;

        if (alive) {
          ctx.fillStyle = "black";
          ctx.fillRect(x * cellSize, y * cellSize, cellSize, cellSize);
        }
      }
    }
  }

  // Animation loop
  function animate(timestamp) {
    if (running) {
      // Only update at specified interval
      if (timestamp - lastUpdateTime > updateInterval) {
        updateGrid();
        lastUpdateTime = timestamp;
      }

      render();
      animationFrameId = requestAnimationFrame(animate);
    }
  }

  // Button event handlers
  const startStopButton = document.getElementById("startStopButton");
  startStopButton.addEventListener("click", () => {
    running = !running;
    startStopButton.textContent = running ? "Stop" : "Start";

    if (running) {
      lastUpdateTime = performance.now();
      animationFrameId = requestAnimationFrame(animate);
    } else {
      cancelAnimationFrame(animationFrameId);
    }
  });

  const resetButton = document.getElementById("resetButton");
  resetButton.addEventListener("click", () => {
    initializeGrid();
    render();
  });

  // Initial render
  render();
};
