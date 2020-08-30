//
// Created by Lucca Portes on 29/08/20.
//

#ifndef SIMDVECTORS_AVXARRAY_H
#define SIMDVECTORS_AVXARRAY_H

#include "SIMDArray.h"
#include "alignedAllocator.h"

#include <immintrin.h>
#include <vector>
#include <iostream>

typedef std::vector<__m256d, aligned_allocator<__m256d, sizeof(__m256d)> > aligned_vector;


class AVXArray : public SIMDArray{
public:
    AVXArray();
    ~AVXArray();
    static AVXArray zeros(int shape);
    static AVXArray arange(int start, int end);
    double sum();
    double prod();
    AVXArray exp();
    AVXArray operator+(const AVXArray& b);
    AVXArray operator-(const AVXArray& b);
    AVXArray operator/(const AVXArray& b);
    AVXArray operator*(const AVXArray& b);
    double operator[] (int index);
    void set(double v, int index);
    friend std::ostream & operator<< (std::ostream &out, const AVXArray &c);
    int Size() const;

private:
    aligned_vector array;
};


#endif //SIMDVECTORS_AVXARRAY_H
