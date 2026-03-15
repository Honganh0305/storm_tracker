import React from 'react';

const ComparisonTable = ({ historicalStorms }) => {


    const getSimilarityClass = (similarity) => {
        if (similarity > 70) return 'bg-red-900 bg-opacity-30';
        if (similarity > 50) return 'bg-orange-900 bg-opacity-30';
        return 'bg-yellow-900 bg-opacity-20';
    };

    return (
        <div className="w-80 bg-gray-900 p-4 overflow-y-auto border-r-2 border-gray-700">
            <h2 className="font-bold mb-4 text-cyan-400 text-xl">
                Historical Comparison
            </h2>

            <table className="w-full border-collapse rounded-lg shadow-lg overflow-hidden">
                <thead>
                    <tr className="border-b-2 border-cyan-500">
                        <th className="px-3 py-3 bg-gray-800 text-left text-xs font-bold text-gray-300">
                            Storm
                        </th>
                        <th className="px-3 py-3 bg-gray-800 text-left text-xs font-bold text-gray-300">
                            Year
                        </th>
                        <th className="px-3 py-3 bg-gray-800 text-left text-xs font-bold text-gray-300">
                            Wind
                        </th>
                        <th className="px-3 py-3 bg-gray-800 text-left text-xs font-bold text-gray-300">
                            Match
                        </th>
                    </tr>
                </thead>

                <tbody>
                    {historicalStorms.map((storm, index) => {
                        let similarity = 0;
                        if (storm.similarity !== undefined && storm.similarity !== null) {
                            similarity = typeof storm.similarity === 'number'
                                ? storm.similarity
                                : parseFloat(storm.similarity) || 0;
                        }

                        similarity = Math.max(0, Math.min(100, similarity));

                        console.log(`Storm ${storm.name}: similarity = ${similarity}`);

                        return (
                            <tr
                                key={index}
                                className={`hover:bg-gray-700 border-b border-gray-700 transition-colors ${getSimilarityClass(similarity)}`}
                                title={`Pressure: ${storm.minPressure || 'N/A'} hPa\nCasualties: ${storm.casualties || 'N/A'}\nDamage: $${storm.damage || 'N/A'}B`}
                            >
                                <td className="px-3 py-2 text-white text-sm">
                                    <strong>{storm.name || 'Unknown'}</strong>
                                </td>
                                <td className="px-3 py-2 text-gray-300 text-sm">
                                    {storm.year || 'N/A'}
                                </td>
                                <td className="px-3 py-2 text-gray-300 text-sm">
                                    {storm.maxWindSpeed ? storm.maxWindSpeed.toFixed(0) : 'N/A'}
                                </td>
                                <td className="px-3 py-2 text-cyan-400 text-sm font-bold">
                                    {similarity > 0 ? similarity.toFixed(1) : '0.0'}%
                                </td>
                            </tr>
                        );
                    })}
                </tbody>
            </table>
        </div>
    );
};

export default ComparisonTable;