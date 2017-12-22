#pragma once

//#ifndef KD_H
//#define KD_H

#include "nanoflann.hpp"
#include <vector>

using namespace nanoflann;
using Points = std::vector<std::vector<double>>;

template <class VectorOfVectorsType, typename num_t = double, int DIM = -1, class Distance = nanoflann::metric_L2, typename IndexType = size_t>
struct KDTreeVectorOfVectorsAdaptor
{
    typedef KDTreeVectorOfVectorsAdaptor<VectorOfVectorsType,num_t,DIM,Distance> self_t;
    typedef typename Distance::template traits<num_t,self_t>::distance_t metric_t;
    typedef nanoflann::KDTreeSingleIndexAdaptor< metric_t,self_t,DIM,IndexType>  index_t;
    
    index_t* index;
    
    KDTreeVectorOfVectorsAdaptor(const int, const VectorOfVectorsType &mat, const int leaf_max_size) : m_data(mat)
    {
        assert(mat.size() != 0 && mat[0].size() != 0);
        const size_t dims = mat[0].size();
        if (DIM>0 && static_cast<int>(dims) != DIM)
            throw std::runtime_error("Data set dimensionality does not match the 'DIM' template argument");
        index = new index_t( dims, *this, nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size ) );
        index->buildIndex();
    }
    
    ~KDTreeVectorOfVectorsAdaptor() {
        delete index;
    }
    
    const VectorOfVectorsType &m_data;
    
    inline void query(const num_t *query_point, const size_t num_closest, IndexType *out_indices, num_t *out_distances_sq, const int nChecks_IGNORED = 10) const
    {
        nanoflann::KNNResultSet<num_t,IndexType> resultSet(num_closest);
        resultSet.init(out_indices, out_distances_sq);
        index->findNeighbors(resultSet, query_point, nanoflann::SearchParams());
    }
    
    const self_t & derived() const {
        return *this;
    }
    self_t & derived()       {
        return *this;
    }
    
    inline size_t kdtree_get_point_count() const {
        return m_data.size();
    }
    
    inline num_t kdtree_get_pt(const size_t idx, int dim) const {
        return m_data[idx][dim];
    }
    
    template <class BBOX>
    bool kdtree_get_bbox(BBOX & ) const {
        return false;
    }
};

class KdTree {
    typedef KDTreeVectorOfVectorsAdaptor<Points, double> RGB_KD_TREE;

    
public:
    KdTree(const Points& points): tree_(3, points, 1) {
        tree_.index->buildIndex();
    }

    size_t GetNearest(const std::vector<double>& pt) const {
        const size_t num_results = 1;
        std::vector<size_t>   ret_indexes(num_results);
        std::vector<double> out_dists_sqr(num_results);
        
        nanoflann::KNNResultSet<double> resultSet(num_results);
        
        resultSet.init(&ret_indexes[0], &out_dists_sqr[0] );
        tree_.index->findNeighbors(resultSet, &pt[0], nanoflann::SearchParams(10));
        
        return ret_indexes[0];
    }
    
private:
    RGB_KD_TREE tree_;
};


//#endif

