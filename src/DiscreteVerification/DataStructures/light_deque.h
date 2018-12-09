/* 
 * File:   light_deque.h
 * Author: Peter G. Jensen <pgj@cs.aau.dk>
 *
 * Created on 03 December 2016, 12:00
 */

#ifndef LIGHT_DEQUE_H
#define LIGHT_DEQUE_H

#include <memory>

using namespace std;

template<typename T>
class light_deque
{
    private:
        size_t _front = 0;
        size_t _back = 0;
        size_t _size = 0;
        std::vector<T> _data;
    public:
        light_deque(size_t initial_size = 64)
        {
            if(initial_size == 0) initial_size = 1;
            _data.resize(initial_size);
            _size = initial_size;
        }
        
        void push_back(T& element)
        {
            _data[_back] = element;
            ++_back;
            if(_back == _size)
            {
                _size *= 2;
               _data.resize(_size);
            }

        }
        
        bool empty()
        {
            return _front == _back;
        }
        
        T front()
        {
            return _data[_front];
        }
        
        void pop_front()
        {
            ++_front;
            if(_front >= _back)
            {
                _front = _back = 0;
            }
        }
        
        void clear()
        {
            _front = _back = 0;
        }
};


#endif /* LIGHT_DEQUE_H */

