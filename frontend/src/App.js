import './App.css';
import { useState, useEffect, useRef } from 'react';
import ComparisonTable from './components/ComparisonTable';
import StormInformation from './components/StormInformation';
import AlertsCard from './components/AlertsCard';
import { StormTimeline } from './components/StormTimeline';
import { MapLegend } from './components/MapLegend';
import { fetchStormData, pollStormData } from './service/api';
import mapboxgl from 'mapbox-gl';
import 'mapbox-gl/dist/mapbox-gl.css';

function App() {
  const [stormData, setStormData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const mapContainerRef = useRef(null); 
  const mapRef = useRef(null); 

  useEffect(() => {
    loadData();
    const intervalId = pollStormData(handleDataUpdate, 300000); 
    return () => clearInterval(intervalId);
  }, []);

  const loadData = async () => {
    try {
      setLoading(true);
      const data = await fetchStormData();
      handleDataUpdate(data);
      setError(null);
    } catch (error) {
      setError("Failed to load storm data");
      console.error(error);
    } finally {
      setLoading(false);
    }
  };

  const handleDataUpdate = (data) => {
    console.log("Data updated:", data);
    setStormData(data);
  };

  useEffect(() => {
    if (!mapContainerRef.current) return;

    mapboxgl.accessToken = process.env.REACT_APP_MAPBOX_ACCESS_TOKEN || "";

    const map = new mapboxgl.Map({
      container: mapContainerRef.current,
      style: 'mapbox://styles/mapbox/satellite-streets-v12',
      center: [-80, 25],
      zoom: 4
    });

    mapRef.current = map;

    const resizeMap = () => {
      if (mapRef.current) mapRef.current.resize();
    };

    map.on('load', () => {
      map.addSource('storm-path', {
        type: 'geojson',
        data: { type: 'FeatureCollection', features: [] }
      });
      map.addLayer({
        id: 'storm-path-line',
        type: 'line',
        source: 'storm-path',
        layout: { 'line-join': 'round', 'line-cap': 'round' },
        paint: {
          'line-color': '#e11d48',
          'line-width': 4,
          'line-dasharray': [2, 1.5]
        }
      });
      map.addSource('storm-points', {
        type: 'geojson',
        data: { type: 'FeatureCollection', features: [] }
      });
      map.addLayer({
        id: 'storm-path-points',
        type: 'circle',
        source: 'storm-points',
        paint: {
          'circle-radius': 8,
          'circle-color': '#ffffff',
          'circle-stroke-width': 2,
          'circle-stroke-color': '#e11d48'
        }
      });
      resizeMap();
    });

    const t1 = setTimeout(resizeMap, 100);
    const t2 = setTimeout(resizeMap, 500);
    const ro = new ResizeObserver(resizeMap);
    ro.observe(mapContainerRef.current);

    return () => {
      ro.disconnect();
      clearTimeout(t1);
      clearTimeout(t2);
      if (mapRef.current) {
        mapRef.current.remove();
        mapRef.current = null;
      }
    };
  }, []);

  useEffect(() => {
    const map = mapRef.current;
    if (!map || !map.getSource) return;

    const storm = stormData?.storm;
    const hasStorm = storm?.position?.lat != null && storm?.position?.lon != null;

    if (window._stormPathMarkers) {
      window._stormPathMarkers.forEach((m) => m.remove());
      window._stormPathMarkers = [];
    }

    if (hasStorm) {
      const lng = storm.position.lon;
      const lat = storm.position.lat;
      map.flyTo({ center: [lng, lat], zoom: 6 });

      if (window._stormMarker) window._stormMarker.remove();
      const el = document.createElement('div');
      el.className = 'storm-center-icon';
      el.innerHTML = `
        <svg width="36" height="36" viewBox="0 0 36 36" fill="none" xmlns="http://www.w3.org/2000/svg">
          <circle cx="18" cy="18" r="14" fill="white" stroke="#e11d48" stroke-width="2"/>
          <path d="M18 8 A10 10 0 0 1 18 28 A10 10 0 0 1 18 8" stroke="#e11d48" stroke-width="1.5" fill="none" stroke-dasharray="4 2"/>
          <path d="M18 12 A6 6 0 0 1 18 24 A6 6 0 0 1 18 12" stroke="#e11d48" stroke-width="1" fill="none" stroke-dasharray="2 2"/>
          <circle cx="18" cy="18" r="3" fill="#e11d48"/>
        </svg>
      `;
      const marker = new mapboxgl.Marker({ element: el })
        .setLngLat([lng, lat])
        .setPopup(new mapboxgl.Popup().setHTML(
          `<strong>${storm.name || 'Storm'}</strong><br/>${storm.category || ''} · ${(storm.windSpeed ?? 0).toFixed(0)} km/h`
        ))
        .addTo(map);
      window._stormMarker = marker;

      const path = [[lng, lat]];
      const points = [[lng, lat]];
      const labels = ['Now'];
      (storm.predictedPath || []).forEach((p) => {
        if (p?.position?.lat != null && p?.position?.lon != null) {
          path.push([p.position.lon, p.position.lat]);
          points.push([p.position.lon, p.position.lat]);
          labels.push(`${p.hours || 0}h`);
        }
      });

      const lineGeojson = { type: 'Feature', properties: {}, geometry: { type: 'LineString', coordinates: path } };
      const pathSource = map.getSource('storm-path');
      if (pathSource) pathSource.setData(lineGeojson);

      const pointFeatures = points.map((coord, i) => ({
        type: 'Feature',
        properties: { label: labels[i] },
        geometry: { type: 'Point', coordinates: coord }
      }));
      const pointsSource = map.getSource('storm-points');
      if (pointsSource) pointsSource.setData({ type: 'FeatureCollection', features: pointFeatures });

      labels.forEach((label, i) => {
        if (i === 0) return;
        const el2 = document.createElement('div');
        el2.className = 'storm-time-marker';
        el2.textContent = label;
        el2.style.cssText = 'background:rgba(0,0,0,0.75);color:white;padding:2px 6px;border-radius:4px;font-size:10px;font-weight:600;white-space:nowrap;border:1px solid white;';
        const m = new mapboxgl.Marker({ element: el2 }).setLngLat(points[i]).addTo(map);
        if (!window._stormPathMarkers) window._stormPathMarkers = [];
        window._stormPathMarkers.push(m);
      });
    } else {
      map.flyTo({ center: [-80, 25], zoom: 4 });
      if (window._stormMarker) { window._stormMarker.remove(); window._stormMarker = null; }
      if (window._stormPathMarkers) {
        window._stormPathMarkers.forEach((m) => m.remove());
        window._stormPathMarkers = [];
      }
      const pathSource = map.getSource('storm-path');
      if (pathSource) pathSource.setData({ type: 'FeatureCollection', features: [] });
      const pointsSource = map.getSource('storm-points');
      if (pointsSource) pointsSource.setData({ type: 'FeatureCollection', features: [] });
    }
  }, [stormData]);

  if (error) {
    return (
      <div className="flex items-center justify-center h-screen bg-gray-900 text-red-400 text-xl">
        {error}
      </div>
    );
  }

  const currentStorm = stormData?.storm;
  const historicalComparison = stormData?.similar || [];

  return (
    <div className="h-screen flex flex-col bg-gray-900 overflow-hidden">
      {loading && (
        <div className="absolute inset-0 flex items-center justify-center bg-gray-900/80 z-50 text-gray-400 text-xl">
          Loading storm data...
        </div>
      )}

      <div className="flex flex-1 overflow-hidden" style={{ minHeight: 0 }}>
        <ComparisonTable historicalStorms={historicalComparison} />

        <div className="flex flex-1 flex-col overflow-hidden" style={{ minHeight: 0 }}>
          <StormInformation stormData={currentStorm} />

          <div className="flex shrink-0 flex-wrap gap-2 border-t border-gray-700 bg-gray-900/50 px-3 py-2 overflow-y-auto min-h-0">
            <h3 className="w-full text-sm font-semibold text-gray-400">Alerts</h3>
            <AlertsCard />
          </div>

          <div className="flex-1 relative min-h-[400px]" style={{ minHeight: 400 }}>
            <div id="map" ref={mapContainerRef} className="absolute inset-0 w-full h-full" style={{ minHeight: 400 }} />
            <MapLegend hasStorm={!!(currentStorm && currentStorm?.position?.lat != null && currentStorm?.position?.lon != null)} />
            <StormTimeline storm={currentStorm} />
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
