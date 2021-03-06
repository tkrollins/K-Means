#pragma once

#include "Definitions.hpp"
#include <mpi.h>

class Coresets
{
private:
    /**
     * @brief Private member variables
     *
     */
    int numThreads;              // the number of threads to use
    int numClusters;             // the number of clusters to cluster to data into
    int numRestarts;             // the number of times Kmeans should try to cluster the data

    value_t bestError;           // the error in the best clustering
    clusters_t clusters;         // the cluster centers
    clusters_t bestClusters;     // the best cluster centers
    clustering_t clustering;     // the cluster assignments for each data point
    clustering_t bestClustering; // the best cluster assignments
    std::vector<int> bestClusterCount;
    dataset_t bestClusterCoord;
    coreset_t coreset;           // the coreset to run clustering on if specified to do so

    MPI_Win dataWin;
    MPI_Win clusteringWin;
    MPI_Win clusterCoordWin;
    MPI_Win clusterCountWin;

    int startIdx_MPI;
    int endIdx_MPI;
    dataset_t data_MPI;
    clustering_t clustering_MPI;
    clustering_t clusteringChunk_MPI;
    std::vector<int> clusterCount_MPI;
    dataset_t clusterCoord_MPI;
    std::vector<int> vDisps_MPI;
    std::vector<int> vLens_MPI;


    /**
     * @brief  Converts a c-array of datapoints into the dataset_t object
     * @note   
     * @param  data: Pointer to the data array
     * @param  size: number of datapoints
     * @param  numFeatures: Number of features per datapoing
     * @retval 
     */
    dataset_t arrayToDataset(value_t* data, int size, int numFeatures);

    /**
     * @brief  Uses MPI window clusterCoordWin to retrieve coordinates of a datapoint
     * @note   
     * @param  idx: Index of the datapoint
     * @param  numFeatures: Number of features in each datapoint
     * @retval A datapoint_t correspointing the index given
     */
    datapoint_t getClusterCoord(int idx, int numFeatures);

    /**
     * @brief  Uses MPI window clusterCountWin to retrieve the count for the specified cluster
     * @note   
     * @param  idx: Index of cluster
     * @retval count int
     */
    int getClusterCount(int idx);

    /**
     * @brief  Uses MPI window clusteringWin to retrieve cluster assigned to datapoint
     * @note   
     * @param  idx: Index of datapoint
     * @retval cluster number
     */
    int getClustering(int idx);

    /**
     * @brief  Sets datapoint coordinates in clusterCoordWin
     * @note   
     * @param  idx: index of cluster
     * @param  numFeatures: number of features in coordinates
     * @param  coord: datapoint_t corresponding to the coordinates to be set
     * @retval None
     */
    void setClusterCoord(int idx, int numFeatures, datapoint_t* coord);

    /**
     * @brief  Sets cluster count in clusterCountWin
     * @note   
     * @param  idx: index of cluster
     * @param  count: count to set
     * @retval None
     */
    void setClusterCount(int idx, int* count);

    
    /**
     * @brief An implementation of the Kmeans++ algorithm for initializing cluster centers. Does this by trying to
     *        maximize the distance between cluster centers.
     *
     * @param data - The data that is being clustered.
     * @param func - The distance function to use.
     */
    void kPlusPlus(dataset_t &data, value_t (*func)(datapoint_t &, datapoint_t &));

    /**
     * @brief Function for finding the closest cluster center to a datapoint and assigning that data point to that
     *        cluster.
     *
     * @param point - The datapoint to be considered.
     * @param pointIdx - The index of the datapoint in the dataset.
     * @param func - The distance function to use.
     * @return value_t - The square of the minimum distance.
     */
    value_t nearest(datapoint_t &point, int &pointIdx, value_t (*func)(datapoint_t &, datapoint_t &));


public:

    /**
     * @brief  Initializes all the member variables used for scatter/gather MPI
     * @note   
     * @param  numData: Number of datapoints
     * @param  numFeatures: Features per datapoint
     * @param  data: c-array with all data
     * @retval None
     */
    void initMPIMembers(int numData, int numFeatures, value_t* data=NULL);

    /**
     * @brief Construct a new Kmeans object.
     *
     * @param numClusters - The number of clusters.
     * @param numRestarts - The number of times to repeat the Kmeans calculation before returning an answer.
     * @param numThreads - The number of threads to use. Defaults to 8.
     */
    Coresets(int numClusters, int numRestarts, int numThreads = 8);

    /**
     * @brief Destroy the Kmeans object.
     *
     */
    ~Coresets();

    /**
     * @brief Function that performs the clustering on coresets using lloyd's algorithm with Kmeans ++ initialization.
     *
     * @param data - The data to be clustered.
     * @param func - The distance function to use.
     */
    void fit_coreset(value_t (*func)(datapoint_t &, datapoint_t &));

    /**
     * @brief Get the numClusters object.
     *
     * @return int
     */
    int getNumClusters()
    {
        return numClusters;
    }

    /**
     * @brief Get the numRestarts object.
     *
     * @return int
     */
    int getNumRestarts() { return numRestarts; }

    /**
     * @brief Get the bestClustering object.
     *
     * @return clusteringPtr_t
     */
    clustering_t getClustering() { return bestClustering; }

    /**
     * @brief Get the bestClusters object.
     *
     * @return clustersPtr_t
     */
    clusters_t getClusters() { return bestClusters; }

    /**
     * @brief Get the bestError.
     *
     * @return int
     */
    value_t getError() { return bestError; };

    /**
     * @brief Get the numThreads object
     *
     * @return int
     */
    int getNumThreads() { return numThreads; };

    /**
     * @brief Set the numClusters object.
     *
     * @param numClusters
     * @return true
     * @return false
     */
    bool setNumClusters(int numClusters);

    /**
     * @brief Set the numRestarts object.
     *
     * @param numRestarts
     * @return true
     * @return false
     */
    bool setNumRestarts(int numRestarts);

    /**
     * @brief Set the numThreads object.
     *
     * @param numThreads
     * @return true
     * @return false
     */
    bool setNumThreads(int numThreads);

    /**
     * @brief The L2 norm between two data points.
     *
     * @param p1 - The first data point.
     * @param p2 - The second data point.
     * @return value_t - The distance.
     */
    static value_t distanceL2(datapoint_t &p1, datapoint_t &p2);

    /**
     * @brief Create the coreset used for representative kmeans clustering on the whole dataset. The coreset is stored
     *        in the private member variable coreset.
     *
     * @param data - The data to be clustered.
     * @param sampleSize - The number of datapoints to include in the coreset.
     * @param func - The distance function to use.
     */
    
    void createCoreSet(dataset_t &data, int &sampleSize, value_t (*func)(datapoint_t &, datapoint_t &));

    /**
     * @brief Using MPI, create the coreset used for representative kmeans clustering on the whole dataset. The coreset is stored
     *        in the private member variable coreset.
     *
     * @param data - The data to be clustered.
     * @param sampleSize - The number of datapoints to include in the coreset.
     * @param func - The distance function to use.
     */
    
    void createCoreSet_MPI(int numData, int numFeatures, value_t *data, int sampleSize, value_t (*func)(datapoint_t &, datapoint_t &));
};