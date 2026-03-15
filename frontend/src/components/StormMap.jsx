import React, { useEffect, useState, useMemo } from "react";
import "leaflet/dist/leaflet.css";
import {
    MapContainer,
    TileLayer,
    CircleMarker,
    Popup,
    Polyline,
    useMap,
} from "react-leaflet";

const FitBounds = ({ coords }) => {
    const map = useMap();

    useEffect(() => {
        if (coords.length > 0) {
            map.fitBounds(coords, { padding: [50, 50] });
        }
    }, [coords, map]);

    return null;
};

const StormMap = ({ stormData }) => {
    const [animatedPath, setAnimatedPath] = useState([]);

    const fullPath = useMemo(() => {
        if (!stormData?.predictedPath || !Array.isArray(stormData.predictedPath)) return [];
        return stormData.predictedPath
            .map(p => {
                if (p?.position?.lat != null && p?.position?.lon != null) {
                    return [p.position.lat, p.position.lon];
                }
                return null;
            })
            .filter(coord => coord !== null);
    }, [stormData?.predictedPath]);

    useEffect(() => {
        if (!fullPath.length) {
            setAnimatedPath([]);
            return;
        }

        setAnimatedPath([]);
        let i = 0;
        const interval = setInterval(() => {
            if (i < fullPath.length && fullPath[i]) {
                setAnimatedPath(prev => [...prev, fullPath[i]]);
                i++;
            }
            if (i >= fullPath.length) clearInterval(interval);
        }, 300);

        return () => clearInterval(interval);
    }, [fullPath]);

    // Default view when no storm: Atlantic basin
    const defaultCenter = [25, -80];
    const defaultZoom = 4;
    const hasValidStorm = stormData?.position?.lat != null && stormData?.position?.lon != null;
    const center = hasValidStorm ? [stormData.position.lat, stormData.position.lon] : defaultCenter;
    const zoom = hasValidStorm ? 6 : defaultZoom;
    const { position, name, category, color, windSpeed, pressure, predictedPath } = stormData || {};

    return (
        <div className="w-full h-full" style={{ height: "100%", width: "100%", position: "relative", zIndex: 0, minHeight: "400px" }}>
            <MapContainer
                center={center}
                zoom={zoom}
                style={{ height: "100%", width: "100%", minHeight: "400px" }}
                scrollWheelZoom={true}
                whenCreated={(mapInstance) => {
                    setTimeout(() => {
                        mapInstance.invalidateSize();
                    }, 100);
                }}
            >
                <TileLayer
                    url="https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png"
                    attribution='&copy; OpenStreetMap contributors &copy; CARTO'
                />

                {hasValidStorm && (
                    <>
                        <CircleMarker
                            center={center}
                            radius={20}
                            fillColor={color || "#00FFFF"}
                            color="#000"
                            weight={3}
                            fillOpacity={0.8}
                        >
                            <Popup>
                                <div style={{ color: '#333' }}>
                                    <h3 style={{ margin: '0 0 8px 0', fontSize: '16px' }}>{name}</h3>
                                    <p style={{ margin: '4px 0' }}>
                                        <strong>Category:</strong> {category}
                                    </p>
                                    <p style={{ margin: '4px 0' }}>
                                        <strong>Wind:</strong> {(windSpeed ?? 0).toFixed(0)} km/h
                                    </p>
                                    <p style={{ margin: '4px 0' }}>
                                        <strong>Pressure:</strong> {(pressure ?? 0).toFixed(0)} hPa
                                    </p>
                                </div>
                            </Popup>
                        </CircleMarker>

                        {animatedPath.length > 1 && animatedPath.every(coord => coord && coord[0] != null && coord[1] != null) && (
                            <>
                                <Polyline
                                    positions={animatedPath.filter(coord => coord && coord[0] != null && coord[1] != null)}
                                    color={color || "#00FFFF"}
                                    weight={4}
                                    opacity={0.7}
                                    dashArray="10, 10"
                                />
                                <FitBounds coords={[center, ...animatedPath.filter(coord => coord && coord[0] != null && coord[1] != null)]} />
                            </>
                        )}

                        {predictedPath?.map((p, idx) => {
                            if (!p?.position || p.position.lat == null || p.position.lon == null) {
                                return null;
                            }
                            return (
                                <CircleMarker
                                    key={idx}
                                    center={[p.position.lat, p.position.lon]}
                                    radius={8}
                                    fillColor={color || "#00FFFF"}
                                    color="#000"
                                    weight={2}
                                    fillOpacity={0.6}
                                >
                                    <Popup>
                                        <div style={{ color: '#333' }}>
                                            <strong>{name}</strong><br />
                                            {p.hours}h prediction
                                        </div>
                                    </Popup>
                                </CircleMarker>
                            );
                        })}
                    </>
                )}
            </MapContainer>
        </div>
    );
};

export default StormMap;