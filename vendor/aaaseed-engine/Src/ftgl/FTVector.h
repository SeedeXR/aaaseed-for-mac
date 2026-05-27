/*
 * FTGL - OpenGL font library
 *
 * Copyright (c) 2001-2004 Henry Maddocks <ftgl@opengl.geek.nz>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef    __FTVector__
#define    __FTVector__

#include "FTGL/ftgl.h"

/**
 * Provides a non-STL alternative to the STL vector
 */
template <typename FT_VECTOR_ITEM_TYPE>
class FTVector
{
    public:
        typedef FT_VECTOR_ITEM_TYPE value_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type* iterator;
        typedef const value_type* const_iterator;

        FTVector()
        {
            Capacity = Size = 0;
            Items = 0;
        }


        virtual ~FTVector()
        {
            clear();
        }

        FTVector& operator =(const FTVector& v)
        {
            reserve(v.capacity());

            iterator ptr = begin();
            const_iterator vbegin = v.begin();
            const_iterator vend = v.end();

            while(vbegin != vend)
            {
                *ptr++ = *vbegin++;
            }

            Size = v.size();
            return *this;
        }

        FTGL::VECTOR_SIZE_TYPE size() const
        {
            return Size;
        }

        FTGL::VECTOR_SIZE_TYPE capacity() const
        {
            return Capacity;
        }

        iterator begin()
        {
            return Items;
        }

        const_iterator begin() const
        {
            return Items;
        }

        iterator end()
        {
            return begin() + size();
        }

        const_iterator end() const
        {
            return begin() + size();
        }

        bool empty() const
        {
            return size() == 0;
        }

        reference operator [](FTGL::VECTOR_SIZE_TYPE const pos)
        {
            return(*(begin() + pos));
        }

        const_reference operator [](FTGL::VECTOR_SIZE_TYPE const pos) const
        {
            return *(begin() + pos);
        }

        void clear()
        {
            if(Capacity)
            {
                delete [] Items;
                Capacity = Size = 0;
                Items = 0;
            }
        }

        void reserve( FTGL::VECTOR_SIZE_TYPE const n )
        {
            if( capacity() < n )
                expand(n);
        }

        void push_back( const value_type& x )
        {
			if( size() == capacity() )
				expand();

			(*this)[size()] = x;
			++Size;
        }

        void resize( FTGL::VECTOR_SIZE_TYPE const n, value_type x )
        {
            if(n == size())
            {
                return;
            }

            reserve(n);
            iterator ibegin, iend;

            if(n >= Size)
            {
                ibegin = this->end();
                iend = this->begin() + n;
            }
            else
            {
                ibegin = this->begin() + n;
                iend = this->end();
            }

            while(ibegin != iend)
            {
                *ibegin++ = x;
            }

            Size = n;
        }


    private:
        void expand( FTGL::VECTOR_SIZE_TYPE const capacity_hint = 0 )
        {
            FTGL::VECTOR_SIZE_TYPE new_capacity = (capacity() == 0) ? 256 : capacity() * 2;
            if( capacity_hint )
            {
                while(new_capacity < capacity_hint)
                {
                    new_capacity *= 2;
                }
            }

            value_type *new_items = new value_type[new_capacity];

            iterator ibegin = this->begin();
            iterator iend = this->end();
            value_type *ptr = new_items;

            while(ibegin != iend)
            {
                *ptr++ = *ibegin++;
            }

            if(Capacity)
            {
                delete [] Items;
            }

            Items = new_items;
            Capacity = new_capacity;
        }

        FTGL::VECTOR_SIZE_TYPE	Capacity;
        FTGL::VECTOR_SIZE_TYPE	Size;
        value_type*				Items;
};

#endif  //  __FTVector__
