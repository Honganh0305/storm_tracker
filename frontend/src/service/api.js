export const fetchStormData = async () => {
  try {
    const response = await fetch('/data/storm_data.json');
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    const data = await response.json();
    console.log('Storm data loaded:', data);
    return data;
    
  } catch (error) {
    console.error('Error fetching storm data:', error);
    throw error;
  }
};

export const pollStormData = (callback, intervalMs = 300000) => {
  // Initial fetch
  fetchStormData()
    .then(data => callback(data))
    .catch(error => console.error('Initial fetch failed:', error));
  
  // Set up interval for updates (default: 5 minutes)
  return setInterval(() => {
    fetchStormData()
      .then(data => callback(data))
      .catch(error => console.error('Polling fetch failed:', error));
  }, intervalMs);
};