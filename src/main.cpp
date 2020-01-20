#include "KPlusPlus.hpp"
#include "Lloyd.hpp"
#include "DistanceFunctors.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include <chrono>
#include "Kmeans.hpp"
#include <iostream>
#include "Definitions.hpp"
// #include "boost/random.hpp"
// #include "boost/generator_iterator.hpp"

// typedef boost::mt19937 RNGType;

int main(int argc, char *argv[])
{
    int numRestarts = 5;
    int numData = 100000;
    int numFeatures = 2;
    int numClusters = 30;

    std::vector<double> times;

    VectorReader reader;
    std::string inFile = "../test_" + std::to_string(numData) + "_" + std::to_string(numFeatures) + ".txt";
    reader.read(inFile, numData, numFeatures);
    Matrix matrix = {reader.getData(), numData, numFeatures};

    // KPlusPlus kplusplus;
    // OptimizedKPlusPlus kplusplus;
    // OMPKPlusPlus kplusplus;
    // OMPOptimizedKPlusPlus kplusplus;
    MPIKPlusPlus kplusplus;
    // Lloyd lloyd;
    // OptimizedLloyd lloyd;
    // OMPLloyd lloyd;
    // OMPOptimizedLloyd lloyd;
    MPILloyd lloyd;
    EuclideanDistance distanceFunc;
    // Kmeans kmeans(&kplusplus, &lloyd, &distanceFunc);
    MPIKmeans kmeans(&kplusplus, &lloyd, &distanceFunc);

    MPI_Init(&argc, &argv);
    int rank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    auto start = std::chrono::high_resolution_clock::now();
    kmeans.fit(&matrix, numClusters, numRestarts);
    auto stop = std::chrono::high_resolution_clock::now();
    auto durationSerialKPP = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    if (rank == 0)
    {
        ClusterDataWriter writer(kmeans.getClusterData(), numData, numFeatures);
        std::string serialKppClustersFile = "../clusters_serial_kpp_" + std::to_string(numData) + "_" + std::to_string(numFeatures) + ".txt";
        std::string serialKppClusteringFile = "../clustering_serial_kpp_" + std::to_string(numData) + "_" + std::to_string(numFeatures) + ".txt";
        writer.writeClusters(serialKppClustersFile);
        writer.writeClustering(serialKppClusteringFile);

        std::cout << "Serial KPlusPlus Done! Error: " << kmeans.getError() << " Time: " << durationSerialKPP.count() << std::endl;
        auto clusterdata = kmeans.getClusterData();
        // std::cout << "Clustering" << std::endl;
        // for (auto &val : clusterdata->clustering)
        // {
        //     std::cout << val << " ";
        // }

        std::cout << std::endl
                  << std::endl;
        std::cout << "Clusters" << std::endl;
        for (auto &val : clusterdata.clusters.data)
        {
            std::cout << val << " ";
        }
    }
    MPI_Finalize();
}