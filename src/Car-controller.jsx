import { useState, useEffect } from "react";
import { FaCar, FaBatteryFull, FaLightbulb, FaBullhorn, FaArrowUp, FaArrowDown, FaArrowLeft, FaArrowRight, FaStop } from "react-icons/fa";
import "./CarController.css";

export default function CarController() {
  const [direction, setDirection] = useState("Neutral");
  const [speed, setSpeed] = useState(50);
  const [headlightsOn, setHeadlightsOn] = useState(false);
  const [hornOn, setHornOn] = useState(false);
  const [battery, setBattery] = useState(85); // Example battery percentage
  const [webSocket, setWebSocket] = useState(null);

  // Initialize WebSocket connection
  useEffect(() => { 
    const ws = new WebSocket('ws://192.168.4.1:81'); // Replace with your server IP(ws://Laptop:81)

    ws.onopen = () => {
      console.log("WebSocket connection established");
      setWebSocket(ws);
    };

    ws.onclose = () => {
      console.log("WebSocket connection closed");
      setWebSocket(null);
    };

    ws.onerror = (error) => {
      console.error("WebSocket error:", error);
    };

    return () => {
      if (ws) {
        ws.close();
      }
    };
  }, []);

  const stopCar = () => {
    setDirection("Neutral");
    setSpeed(0);
  };

  // Function to handle mouse down (user presses the button)
  const handleMouseDown = (newDirection) => {
    setDirection(newDirection);
  };

  // Function to handle mouse up (user releases the button)
  const handleMouseUp = () => {
    // Do nothing, direction remains the same
  };

  // useEffect hook to send data in JSON format whenever state changes
  useEffect(() => {
    const carData = {
      direction,
      speed,
      headlightsOn,
      hornOn,
      battery
    };

    console.log(JSON.stringify(carData, null, 2));

    if (webSocket && webSocket.readyState === WebSocket.OPEN) {
      webSocket.send(JSON.stringify(carData));
    }
  }, [direction, speed, headlightsOn, hornOn, battery, webSocket]);

  return (
    <div className="car-controller">
      <h1>
        <FaCar /> Car Controller
      </h1>

      {/* Status Section */}
      <div className="status-container">
        <p>
          <button className="horn-control" onClick={() => setHornOn(!hornOn)}>
            <FaBullhorn /> {hornOn ? "Horn On" : "Horn Off"}
          </button>
        </p>

        <p>
          <button className="headlights-control" onClick={() => setHeadlightsOn(!headlightsOn)}>
            <FaLightbulb /> {headlightsOn ? "Lights On" : "Lights Off"}
          </button>
        </p>

        <p className="battery-status">
          <FaBatteryFull /> Battery: {battery}%
        </p>
      </div>

      {/* Display Current Direction */}
      <div className="direction-display">Direction: {direction}</div>

      <div className="car-main-controller">
        {/* Speed Control */}
        <div className="speed-control">
          <input
            type="range"
            min="0"
            max="100"
            value={speed}
            onChange={(e) => setSpeed(Number(e.target.value))}
            className="vertical-slider"
          />
          <span>Speed:</span>
          {speed}%
        </div>

        {/* Stop Button */}
        <button className="control-btn stop-btn" onClick={stopCar}>
          <FaStop /> Stop
        </button>

        {/* Direction Controls */}
        <div className="direction-box">
          {/* (i). Forward */}
          <button
            className="control-btn"
            onMouseDown={() => handleMouseDown("Forward")}
            onMouseUp={handleMouseUp}
          >
            <FaArrowUp />
          </button>

          <div className="middle-row">
            {/* (ii). Left */}
            <button
              className="control-btn"
              onMouseDown={() => handleMouseDown("Left")}
              onMouseUp={handleMouseUp}
            >
              <FaArrowLeft />
            </button>

            {/* (iii). Right */}
            <button
              className="control-btn"
              onMouseDown={() => handleMouseDown("Right")}
              onMouseUp={handleMouseUp}
            >
              <FaArrowRight />
            </button>
          </div>

          {/* (iv). Reverse */}
          <button
            className="control-btn"
            onMouseDown={() => handleMouseDown("Reverse")}
            onMouseUp={handleMouseUp}
          >
            <FaArrowDown />
          </button>
        </div>
      </div>
    </div>
  );
}