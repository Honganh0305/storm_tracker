import React from 'react';

export function MapLegend({ hasStorm }) {
  return (
    <div className="absolute top-4 right-4 z-10 rounded-lg bg-gray-900/90 backdrop-blur border border-gray-700 shadow-xl px-3 py-2 text-xs">
      <div className="font-semibold text-white mb-2">Legend</div>
      <div className="space-y-1.5 text-gray-300">
        {hasStorm && (
          <>
            <div className="flex items-center gap-2">
              <div className="w-4 h-0.5 border-t-2 border-dashed border-red-500 bg-white/20" />
              <span>Storm path</span>
            </div>
            <div className="flex items-center gap-2">
              <div className="w-3 h-3 rounded-full bg-cyan-500 border-2 border-white" />
              <span>Current position</span>
            </div>
            <div className="flex items-center gap-2">
              <div className="w-2.5 h-2.5 rounded-full border-2 border-white bg-cyan-500/60" />
              <span>Predicted (6h–72h)</span>
            </div>
          </>
        )}
        {!hasStorm && <span className="text-gray-400">No active storm</span>}
      </div>
    </div>
  );
}

export default MapLegend;
