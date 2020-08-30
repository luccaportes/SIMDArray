//
// Created by Lucca Portes on 29/08/20.
//

#ifndef SIMDVECTORS_SIMDARRAY_H
#define SIMDVECTORS_SIMDARRAY_H

#include <stdexcept>

class SIMDArray {
public:
    virtual ~SIMDArray(){
        this->size = 0;
//        this->real_size = 0;
    };
//    void check_same_size(SIMDArray * b){
//        if (this->size != b->size){
//            throw std::out_of_range ("Arrays must be of the same length to be summed");
//        }
//    }
//    virtual SIMDArray operator+ ( const SIMDArray& obj ) const = 0;
protected:
    int size;
    int real_size;
};

#endif //SIMDVECTORS_SIMDARRAY_H
