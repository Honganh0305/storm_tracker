import React, { useState, useEffect } from 'react';

const AlertsCard = () => {
    const [alert, setAlert] = useState(null);

    useEffect(() => {
        async function loadAlerts() {
            try {
                const response = await fetch('https://api.weather.gov/alerts/active');
                const data = await response.json();
                const features = data?.features ?? [];
                if (features.length > 0) {
                    setAlert(features[0]);
                }
            } catch (error) {
                console.error('Error fetching NWS alerts:', error);
            }
        }
        loadAlerts();
    }, []);

    if (!alert?.properties) return null;

    const headline = alert.properties.headline ?? alert.properties.description;
    const severity = alert.properties.severity;
    const dateStr = alert.properties.sent
        ? new Date(alert.properties.sent).toLocaleString()
        : '';

    return (
        <div className="rounded-lg border border-gray-600 bg-gray-800/80 p-3 text-gray-200">

            {severity && severity.toLowerCase() !== 'unknown' && (
                <span className="text-xs font-semibold uppercase tracking-wide text-amber-400">
                    {severity}
                </span>
            )}

            <h4 className="mt-1 font-medium text-white">
                {headline || 'Weather Alert'}
            </h4>

            {dateStr && (
                <p className="mt-1 text-xs text-gray-500">
                    {dateStr}
                </p>
            )}
        </div>
    );
};

export default AlertsCard;
