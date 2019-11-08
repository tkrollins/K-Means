#pragma once

#include "Definitions.hpp"

class Kmeans
{
private:
    /**
     * @brief Private member variables
     *
     */
    int numClusters;             // the number of clusters to cluster to data into
    int numRestarts;             // the number of times Kmeans should try to cluster the data
    float bestError;             // the error in the best clustering
    clusters_t clusters;         // the cluster centers
    clusters_t bestClusters;     // the best cluster centers
    clustering_t clustering;     // the cluster assignments for each data point
    clustering_t bestClustering; // the best cluster assignments

    /**
     * @brief An implementation of the Kmeans++ algorithm for initializing cluster centers. Does this by trying to
     *        maximize the distance between cluster centers.
     *
     * @param data - A pointer to the data that is being clustered.
     * @param func - The distance function to use.
     */
    void kPlusPlus(dataset_t data, float (*func)(datapoint_t &, datapoint_t &));

    /**
     * @brief Function for finding the closest cluster center to a datapoint and assigning that data point to that
     *        cluster.
     *
     * @param point - The data point to be considered.
     * @param pointIdx - The index of the data point in the dataset.
     * @param func - The distance function to use.
     * @return float - The square of the minimum distance.
     */
    float nearest(datapoint_t &point, int &pointIdx, float (*func)(datapoint_t &, datapoint_t &));

public:
    /**
     * @brief Construct a new Kmeans object.
     *
     * @param numClusters - The number of clusters.
     * @param numRestarts - The number of times to repeat the Kmeans calculation before returning an answer.
     */
    Kmeans(int numClusters, int numRestarts);

    /**
     * @brief Destroy the Kmeans object.
     *
     */
    ~Kmeans();

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
    float getError() { return bestError; };

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
};