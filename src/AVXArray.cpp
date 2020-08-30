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
        if (n_valid == 0){
            n_valid = 4;
        }
        for (int i = 0; i < n_valid; i++) {
            out << c.array[c.array.size()-1][3-i] << " ";
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
    if (n_valid == 0){
        n_valid = 4;
    }
    for (int i = 0; i < n_valid; i++) {
        res += this->array[this->array.size()-1][3-i];
    }
    return res;
}

double AVXArray::prod() {
    double res = 1.0;
    for (int i = 0; i < this->array.size()-1; i++){
        res *= this->array[i][0] * this->array[i][1] * this->array[i][2] * this->array[i][3];
    }
    auto n_valid = this->size % 4;
    if (n_valid == 0){
        n_valid = 4;
    }
    for (int i = 0; i < n_valid; i++) {
        res *= this->array[this->array.size()-1][3-i];
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
    if (n_valid == 0){
        n_valid = 4;
    }
    for (int i = 0; i < n_valid; i++) {
        arr.array[this->array.size()-1][3-i] = std::exp(this->array[this->array.size()-1][3-i]);
    }

    return arr;
}

double AVXArray::max() {
    std::vector<double> max_values;
    for (int i = 0; i < this->array.size()-1; i++) {
        __m256d y = _mm256_permute2f128_pd(this->array[i], this->array[i], 1); // permute 128-bit values
        __m256d m1 = _mm256_max_pd(this->array[i], y); // m1[0] = max(x[0], x[2]), m1[1] = max(x[1], x[3]), etc.
        __m256d m2 = _mm256_permute_pd(m1, 5); // set m2[0] = m1[1], m2[1] = m1[0], etc.
        __m256d m = _mm256_max_pd(m1, m2); // all m[0] ... m[3] contain the horizontal max(x[0], x[1], x[2], x[3])
        max_values.push_back(m[0]);
    }
    auto n_valid = this->size % 4;
    double max_last_array = this->array[this->array.size()-1][3];
    if (n_valid == 0){
        n_valid = 4;
    }
    for (int i = 0; i < n_valid; i++) {
        if (this->array[this->array.size()-1][3-i] > max_last_array){
            max_last_array = this->array[this->array.size()-1][3-i];
        }
    }
    max_values.push_back(max_last_array);
    return *std::max_element(max_values.begin(), max_values.end());
}

int AVXArray::argmax() {
    double max_value = this->max();
    for (int i = 0 ; i < this->size ; i++){
        if (max_value == (*this)[i]){
            return i;
        }
    }
    return -1;
}

AVXArray AVXArray::sqrt() {
    AVXArray arr = AVXArray();
    arr.size = this->size;
    for (int i=0; i < this->array.size(); i++){
        arr.array.push_back(_mm256_sqrt_pd(this->array[i]));
    }
    return arr;
}
