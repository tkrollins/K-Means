#pragma once

#include <vector>

typedef float value_t;

/**
 * @brief Wrapper class around std::vector<value_t> that helps manipulate the vector as if it were a 2D nested vector.
 *        Keeping the data as a 1D vector rather than a nested vector makes it easier to implement with MPI.
 */
struct Matrix
{
    // public member variables
    std::vector<value_t> data;
    int numRows; // this is the number of datapoints in the matrix
    int numCols; // this is the number of features of each datapoint in the matrix

    /**
     * @brief Function to access the beginning of a "row" of the matrix, where each row of the matrix is a datapoint.
     *        This function returns an iterator to the beginning of the row which can be used to access the rest of the
     *        row.
     *
     * @param row - The number of the row to retrieve.
     * @return std::vector<value_t>::iterator
     */
    std::vector<value_t>::iterator at(const int &row)
    {
        return data.begin() + (row * numCols);
    }

    /**
     * @brief Function to access a specific value in the matrix.
     *
     * @param row - The row number that the value is in.
     * @param col - The column number that the value is in.
     * @return value_t&
     */
    value_t &at(const int &row, const int &col)
    {
        return *(this->at(row) + col);
    }
};

/**
 * @brief Class to wrap the clustering data generated by Kmeans.
 */
struct ClusterData
{
    // public member variables
    std::vector<int> clustering;    // the cluster assignments of each datapoint
    Matrix clusters;                // the cluster centers
    std::vector<int> clusterCounts; // the number of datapoints assigned to each cluster center

    /**
     * @brief Default constructor.
     */
    ClusterData(){};

    /**
     * @brief Construct a new ClusterData object.
     *
     * @param numData - The number of datapoints that are being clustered.
     * @param numFeatures - The number of features each datapoint has.
     * @param numClusters - The number of clusters that the data is being clustered into.
     */
    ClusterData(const int &numData, const int &numFeatures, const int &numClusters)
    {
        clustering = std::vector<int>(numData, -1);
        clusterCounts = std::vector<int>(numClusters, 0);
        clusters = {std::vector<value_t>(), numClusters, numFeatures};
        clusters.data.reserve(numClusters * numFeatures);
    }

    /**
     * @brief Overloaded assignment operator.
     *
     * @param other - An instance of ClusterData that is to be copied into the calling instance of ClusterData.
     */
    void operator=(const ClusterData &other)
    {
        this->clustering = std::move(other.clustering);
        this->clusters.data = std::move(other.clusters.data);
        this->clusterCounts = std::move(other.clusterCounts);
        this->clusters.numRows = other.clusters.numRows;
        this->clusters.numCols = other.clusters.numCols;
    }
};

/**
 * @brief A return structure that couples the distance between a point and its closest cluster and the index of that
 *        cluster together.
 */
struct ClosestCluster
{
    int clusterIdx;
    value_t distance;
};

typedef std::vector<value_t> datapoint_t;
typedef std::vector<datapoint_t> dataset_t;
typedef std::vector<int> clustering_t;

typedef struct
{
    dataset_t data;
    std::vector<value_t> weights;
} coreset_t;

typedef struct
{
    int count;
    datapoint_t coords;
} cluster_t;

typedef std::vector<cluster_t> clusters_t;