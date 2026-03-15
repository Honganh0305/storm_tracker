import React from 'react';

const StormInformation = ({ stormData }) => {
    if (!stormData) return <div className="p-4 text-gray-400">No storm data available</div>;

    const getCategoryClass = (category) => {
        if (!category) return 'text-gray-400';
        const cat = category.toLowerCase();
        if (cat.includes('5')) return 'text-red-900';
        if (cat.includes('4')) return 'text-red-600';
        if (cat.includes('3')) return 'text-orange-500';
        if (cat.includes('2')) return 'text-orange-400';
        if (cat.includes('1')) return 'text-yellow-400';
        return 'text-cyan-400';
    };

    return (
        <div className="bg-gradient-to-r from-gray-800 to-gray-900 p-6 border-b-2 border-cyan-500">
            <h1 className="text-3xl font-bold text-white mb-4">{stormData.name || 'Unknown Storm'}</h1>

            <div className="flex flex-wrap gap-6">
                <div className="flex flex-col">
                    <span className="text-sm text-gray-400 mb-1">Category</span>
                    <span className={`text-lg font-bold ${getCategoryClass(stormData.category)}`}>
                        {stormData.category || 'Unknown'}
                    </span>
                </div>

                <div className="flex flex-col">
                    <span className="text-sm text-gray-400 mb-1">Wind Speed</span>
                    <span className="text-lg font-bold text-cyan-400">
                        {stormData.windSpeed ? stormData.windSpeed.toFixed(0) : 'N/A'} km/h
                    </span>
                </div>

                <div className="flex flex-col">
                    <span className="text-sm text-gray-400 mb-1">Pressure</span>
                    <span className="text-lg font-bold text-cyan-400">
                        {stormData.pressure ? stormData.pressure.toFixed(0) : 'N/A'} hPa
                    </span>
                </div>

                <div className="flex flex-col">
                    <span className="text-sm text-gray-400 mb-1">Position</span>
                    <span className="text-lg font-bold text-cyan-400">
                        {stormData.position?.lat ? stormData.position.lat.toFixed(2) : 'N/A'}°,
                        {' '}
                        {stormData.position?.lon ? stormData.position.lon.toFixed(2) : 'N/A'}°
                    </span>
                </div>

                <div className="flex flex-col">
                    <span className="text-sm text-gray-400 mb-1">Velocity</span>
                    <span className="text-lg font-bold text-cyan-400">
                        {stormData.velocity ? stormData.velocity.toFixed(0) : 'N/A'} km/h @
                        {' '}
                        {stormData.direction ? stormData.direction.toFixed(0) : 'N/A'}°
                    </span>
                </div>
            </div>
        </div>
    );
};

export default StormInformation;