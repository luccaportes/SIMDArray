//
// Created by Lucca Portes on 29/08/20.
//

#include "AVXArray.h"
#include <cmath>
#include <vector>
#include <stdexcept>

AVXArray AVXArray::zeros(int shape) {
    AVXArray arr = AVXArray();
    arr.size = shape;
    int real_size = std::ceil(shape / 4.0);
    for (int i = 0; i < real_size; i++){
        arr.array.push_back(_mm256_set1_pd(0));
    }
    return arr;
}

AVXArray AVXArray::arange(int start, int end) {
    AVXArray arr = AVXArray();
    int size = end - start;
    if (size <= 0){
        return arr;
    }
    arr.size = end - start;
    std::vector<double> tmp_arr;
    for (int i=start; i<end; i++){
        if (tmp_arr.size() == 4){
            arr.array.push_back(_mm256_set_pd(tmp_arr[0], tmp_arr[1], tmp_arr[2], tmp_arr[3]));
            tmp_arr.clear();
        }
        tmp_arr.push_back(i);
    }
    if (!tmp_arr.empty()){
        if (tmp_arr.size() == 4){
            arr.array.push_back(_mm256_set_pd(tmp_arr[0], tmp_arr[1], tmp_arr[2], tmp_arr[3]));;
        } else if (tmp_arr.size() == 3){
            arr.array.push_back(_mm256_set_pd(tmp_arr[0], tmp_arr[1], tmp_arr[2], 0));;
        } else if (tmp_arr.size() == 2){
            arr.array.push_back(_mm256_set_pd(tmp_arr[0], tmp_arr[1], 0, 0));;
        } else{
            arr.array.push_back(_mm256_set_pd(tmp_arr[0], 0, 0, 0));;
        }
    }
    return arr;
}

AVXArray::AVXArray() : SIMDArray(){

}

AVXArray::~AVXArray() {

}

AVXArray AVXArray::operator+(const AVXArray &b) {
    if (this->Size() != b.Size()){
        throw std::out_of_range ("Arrays must be of the same length to be summed");
    }
    AVXArray arr = AVXArray();
    arr.size = this->size;
    for (int i=0; i < this->array.size(); i++){
        arr.array.push_back(_mm256_add_pd(this->array[i], b.array[i]));
    }
    return arr;
}

AVXArray AVXArray::operator-(const AVXArray &b) {
    if (this->Size() != b.Size()){
        throw std::out_of_range ("Arrays must be of the same length to be summed");
    }
    AVXArray arr = AVXArray();
    arr.size = this->size;
    for (int i=0; i < this->array.size(); i++){
        arr.array.push_back(_mm256_sub_pd(this->array[i], b.array[i]));
    }
    return arr;
}

AVXArray AVXArray::operator/(const AVXArray &b) {
    if (this->Size() != b.Size()){
        throw std::out_of_range ("Arrays must be of the same length to be summed");
    }
    AVXArray arr = AVXArray();
    arr.size = this->size;
    for (int i=0; i < this->array.size(); i++){
        arr.array.push_back(_mm256_div_pd(this->array[i], b.array[i]));
    }
    return arr;
}

AVXArray AVXArray::operator*(const AVXArray &b) {
    if (this->Size() != b.Size()){
        throw std::out_of_range ("Arrays must be of the same length to be summed");
    }
    AVXArray arr = AVXArray();
    arr.size = this->size;
    for (int i=0; i < this->array.size(); i++){
        arr.array.push_back(_mm256_mul_pd(this->array[i], b.array[i]));
    }
    return arr;
}

int AVXArray::Size() const {
    return this->size;
}

std::ostream &operator<<(std::ostream &out, const AVXArray &c) {
    if (c.array.empty()){
        out << "[]";
    } else {
        out << "[ ";
        for (int i = 0; i < c.array.size() - 1; i++) {
            out << c.array[i][3] << " " << c.array[i][2] << " " << c.array[i][1] << " " << c.array[i][0] << " ";
        }
        auto n_valid = c.size % 4;
        for (int i = 3; i >= n_valid; i--) {
            out << c.array[c.array.size()-1][i] << " ";
        }
        out << "]";
        return out;
    }
}

double AVXArray::operator[](int index){
    if (index < 0 || index >= this->size){
        throw std::out_of_range ("invalid index");
    }
    int father_index = index / 4;
    int child_index = 3 - (index % 4);
    return this->array[father_index][child_index];
}

void AVXArray::set(double v, int index) {
    if (index < 0 || index >= this->size){
        throw std::out_of_range ("invalid index");
    }
    int father_index = index / 4;
    int child_index = 3 - (index % 4);
    this->array[father_index][child_index] = v;
}

double AVXArray::sum() {
    double res = 0.0;
    for (int i = 0; i < this->array.size()-1; i++){
        __m256d s = _mm256_hadd_pd(this->array[i],this->array[i]);
        res += ((double*)&s)[0] + ((double*)&s)[2];
    }

    auto n_valid = this->size % 4;
    for (int i = 3; i >= n_valid; i--) {
        res += this->array[this->array.size()-1][i];
    }
    return res;
}

double AVXArray::prod() {
    double res = 1.0;
    for (int i = 0; i < this->array.size()-1; i++){
        res *= this->array[i][0] * this->array[i][1] * this->array[i][2] * this->array[i][3];
    }
    auto n_valid = this->size % 4;
    for (int i = 3; i >= n_valid; i--) {
        res *= this->array[this->array.size()-1][i];
    }
    return res;
}

AVXArray AVXArray::exp() {
    AVXArray arr = AVXArray();
    arr.size = this->size;
    for (int i = 0; i < this->array.size()-1; i++){
        arr.array.push_back(
                _mm256_set_pd(
                        std::exp(this->array[i][3]),
                        std::exp(this->array[i][2]),
                        std::exp(this->array[i][1]),
                        std::exp(this->array[i][0])));
    }
    auto n_valid = this->size % 4;
    arr.array.push_back(_mm256_set1_pd(0));
    for (int i = 3; i >= n_valid; i--) {
        arr.array[this->array.size()-1][i] = std::exp(this->array[this->array.size()-1][i]);
    }

    return arr;
}
