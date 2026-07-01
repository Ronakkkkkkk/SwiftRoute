const API_BASE = "http://localhost:8080";

// --- Graph data (kept in sync manually with the C++ backend) ---
const nodePositions = {
  0: [12.9716, 77.5946],
  1: [12.9760, 77.6010],
  2: [12.9650, 77.5900],
  3: [12.9800, 77.5850],
  4: [12.9600, 77.6050],
  5: [12.9550, 77.5950],
  6: [12.9500, 77.6000],
};

const edges = [
  [0, 1], [0, 2], [1, 3], [2, 3], [2, 4], [3, 5], [4, 5], [5, 6],
];

const restaurantNodes = [2, 3, 4];
const customerNodes = [5, 6, 1];

// --- Map setup ---
const map = L.map("map").setView(nodePositions[0], 14);

L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
  attribution: "&copy; OpenStreetMap contributors",
}).addTo(map);

// Draw edges (roads)
edges.forEach(([a, b]) => {
  L.polyline([nodePositions[a], nodePositions[b]], {
    color: "#9ca3af",
    weight: 2,
    opacity: 0.6,
  }).addTo(map);
});

// Draw graph nodes (intersections)
Object.entries(nodePositions).forEach(([id, pos]) => {
  L.circleMarker(pos, {
    radius: 4,
    color: "#f59e0b",
    fillOpacity: 0.6,
  })
    .addTo(map)
    .bindPopup(`Node ${id}`);
});

// --- Rider markers (these get updated live) ---
let riderMarkers = {}; // riderId -> Leaflet marker

function riderIcon(available) {
  const color = available ? "#2563eb" : "#9ca3af";
  return L.divIcon({
    className: "",
    html: `<div style="background:${color};width:14px;height:14px;border-radius:50%;border:2px solid white;"></div>`,
  });
}

async function fetchRiders() {
  try {
    const res = await fetch(`${API_BASE}/riders`);
    const riders = await res.json();

    riders.forEach((rider) => {
      const pos = nodePositions[rider.currentNode];

      if (riderMarkers[rider.id]) {
        // Update existing marker
        riderMarkers[rider.id].setLatLng(pos);
        riderMarkers[rider.id].setIcon(riderIcon(rider.available));
        riderMarkers[rider.id].setPopupContent(
          `Rider ${rider.id} — ${rider.available ? "Available" : "Busy"}<br>At node ${rider.currentNode}`
        );
      } else {
        // Create new marker
        riderMarkers[rider.id] = L.marker(pos, { icon: riderIcon(rider.available) })
          .addTo(map)
          .bindPopup(`Rider ${rider.id} — ${rider.available ? "Available" : "Busy"}<br>At node ${rider.currentNode}`);
      }
    });
  } catch (err) {
    console.error("Failed to fetch riders:", err);
  }
}

// Poll every 2 seconds
fetchRiders();
setInterval(fetchRiders, 2000);

// --- New Order button ---
const newOrderBtn = document.getElementById("newOrderBtn");
const orderStatus = document.getElementById("orderStatus");

newOrderBtn.addEventListener("click", async () => {
  newOrderBtn.disabled = true;
  newOrderBtn.textContent = "Assigning...";

  const restaurantNode = restaurantNodes[Math.floor(Math.random() * restaurantNodes.length)];
  const customerNode = customerNodes[Math.floor(Math.random() * customerNodes.length)];

  try {
    const res = await fetch(`${API_BASE}/order`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ restaurantNode, customerNode }),
    });
    const data = await res.json();

    orderStatus.textContent = `Order #${data.orderId} → restaurant node ${restaurantNode}, customer node ${customerNode} — ${data.status}`;

    await fetchRiders(); // refresh immediately instead of waiting for next poll
  } catch (err) {
    console.error("Failed to place order:", err);
    orderStatus.textContent = "Failed to place order — is the server running?";
  } finally {
    newOrderBtn.disabled = false;
    newOrderBtn.textContent = "New Order";
  }
});